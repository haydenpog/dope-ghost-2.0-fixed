#include "pch.h"
#include "UserInterface.h"

#include "../Cheat/Settings.h"
#include "../Cheat/UserConfigs.h"
#include "../Cheat/InjectionHelper.h"
#include "../Cheat/Security/Authentification/Authentification.h"

#include "../../vendors/imgui/imgui.h"
#include "../../vendors/imgui/imgui_custom.hpp"
#include "../../vendors/imgui/imgui_internal.h"
#include "../../vendors/imgui/imgui_impl_win32.h"
#include "../../vendors/imgui/imgui_impl_dx11.h"
#include "../../vendors/imgui/fonts.hpp"
#include "../../vendors/imgui/img.hpp"
#include "../../vendors/json.h"

#include <d3d11.h>
#include <D3DX11tex.h>
#include <dwmapi.h>
#include <mutex>
#include <shellapi.h>
#include <numbers>



static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;
static bool						g_wndMinimized = false;

void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

bool CreateDeviceD3D(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void CleanupRenderTarget()
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

extern IMGUI_IMPL_API long long ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			CreateRenderTarget();
		}

		g_wndMinimized = (wParam == SIZE_MINIMIZED);
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

void UserInterface::Display()
{
	auto globals = Instance<Globals>::Get();

	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC , WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, "#w", NULL };
	::RegisterClassEx(&wc);

	globals->m_Hwnd = CreateWindowExA(0L, wc.lpszClassName, "", WS_POPUP, ((GetSystemMetrics(SM_CXSCREEN) - m_Width) / 2.f), ((GetSystemMetrics(SM_CYSCREEN) - m_Height) / 2.f), m_Width, m_Height, NULL, NULL, wc.hInstance, NULL);

	MARGINS margins = { -1 };
	DwmExtendFrameIntoClientArea(globals->m_Hwnd, &margins); //make transparent

	if (!CreateDeviceD3D(globals->m_Hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClass(wc.lpszClassName, wc.hInstance);
		return;
	}

	::ShowWindow(globals->m_Hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(globals->m_Hwnd);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = nullptr;
	ImGuiContext& g = *GImGui;
	auto style = &ImGui::GetStyle();

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(globals->m_Hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	ImVec4 clear_color = ImVec4(0.f, 0.f, 0.f, 0.f);

	ID3D11ShaderResourceView* Image = nullptr;
	D3DX11_IMAGE_LOAD_INFO info;
	ID3DX11ThreadPump* pump{ nullptr };
	D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, imgs::rawData, sizeof(imgs::rawData), &info,
		pump, &Image, 0);

	ID3D11ShaderResourceView* lilImage = nullptr;
	D3DX11_IMAGE_LOAD_INFO info2;
	ID3DX11ThreadPump* pump2{ nullptr };
	D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, imgs::logoRaw, sizeof(imgs::logoRaw), &info2,
		pump2, &lilImage, 0);


	/**/
	ImFont* oliviasans = io.Fonts->AddFontFromMemoryCompressedTTF(fonts::oliviasans_compressed_data, fonts::oliviasans_compressed_size, 20.f);
	static const ImWchar icons_ranges[] = { ICON_MIN_MD, ICON_MAX_MD, 0 };
	ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
	io.Fonts->AddFontFromMemoryCompressedTTF(fonts::materialicons_compressed_data, fonts::materialicons_compressed_size, 20.f, &icons_config, icons_ranges);

	ImFont* iconFont = io.Fonts->AddFontFromMemoryCompressedTTF(fonts::materialicons_compressed_data, fonts::materialicons_compressed_size, 30.f, NULL, icons_ranges);

	ImFont* reforma = io.Fonts->AddFontFromMemoryCompressedTTF(fonts::reforma_compressed_data, fonts::reforma_compressed_size, 15.f);
	ImFont* opensan = io.Fonts->AddFontFromMemoryCompressedTTF(fonts::opensans_med_compressed_data, fonts::opensans_med_compressed_size, 22.5f);
	ImFont* medopensan = io.Fonts->AddFontFromMemoryCompressedTTF(fonts::opensans_med_compressed_data, fonts::opensans_med_compressed_size, 20.f);
	ImFont* lilopensan = io.Fonts->AddFontFromMemoryCompressedTTF(fonts::opensans_med_compressed_data, fonts::opensans_med_compressed_size, 15.f);
	ImFont* bigopensan = io.Fonts->AddFontFromMemoryCompressedTTF(fonts::opensans_med_compressed_data, fonts::opensans_med_compressed_size, 32.5f);

	ImFont* iconFont2 = io.Fonts->AddFontFromMemoryCompressedTTF(fonts::icons_compressed_data, fonts::icons_compressed_size, 25.f);
	ImFont* hugeIcons = io.Fonts->AddFontFromMemoryCompressedTTF(fonts::icons_compressed_data, fonts::icons_compressed_size, 50.f);

	int menuMovementX = 0;
	int menuMovementY = 0;
	int currentTab = 0;
	int currentModule = 0;
	bool bTitleHovered = false;
	bool bDraging = false;
	bool bBlockEspMenu = false;
	bool bConfigMenu = false;

	bool bLogged = false;
	bool bLoaded = false;
	bool bLoginProceceed = false;
	bool bInstanceChoose = false;
	bool bMinimized = false;
	int iLoadingPercentage = 0;

	std::vector<std::string> userConfigs;
	std::vector<std::string> playerEspItems{ "2D","3D", "3D Filled"};
	std::vector<std::string> aimAssistItems{ "Center", "Multipoint" };
	std::vector<std::string> throwpotItems{ "One Throw", "Double", "Smart" };

	constexpr auto easeOutSine = [](float t)
	{
		return abs(sin((t * 3.1415926f) / 2));
	};

	constexpr auto easeInCubic = [](float x) {
		return x * x * x;
	};

	constexpr auto easeOutCirc = [](float x) {
		return sqrtf(1 - powf(x - 1, 2));
	};

	constexpr auto easeInSine = [](float x) {
		return 1.f - cosf((x * std::numbers::pi) / 2);
	};

	constexpr auto easeInQuint = [](float x) {
		return x * x * x * x * x;
	};

	constexpr auto easeInOutCubic = [](float x) {
		return x < 0.5 ? 4 * x * x * x : 1 - powf(-2 * x + 2, 3) / 2;
	};

	while (!Instance<Settings>::Get()->m_Destruct)
	{
		MSG msg;
		while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);

			if (msg.message == WM_QUIT) {
				Instance<Settings>::Get()->m_Destruct = true;
			}
			else if (Instance<Globals>::Get()->Flags.m_Flag1 == 0x1 || Instance<Globals>::Get()->Flags.m_Flag2 == 0x2 || Instance<Globals>::Get()->Flags.m_Flag3 == 0x3) {
			}
			else {
			}
		}

		if (g_wndMinimized)
		{
			Sleep(1);
			continue;
		}
		else {
			bMinimized = false;
		}

		if(GetAsyncKeyState(Instance<Settings>::Get()->Combat.RightClicker.m_Bind) & 0x8000)
		{
			if(Instance<Settings>::Get()->Combat.RightClicker.m_Enabled == true)
			{
				Instance<Settings>::Get()->Combat.RightClicker.m_Enabled = false;
				Sleep(500);
			}
			else
			{
				Instance<Settings>::Get()->Combat.RightClicker.m_Enabled = true;
				Sleep(500);
			}
		}

		if (GetAsyncKeyState(Instance<Settings>::Get()->Combat.LeftClicker.m_Bind) & 0x8000)
		{
			if (Instance<Settings>::Get()->Combat.LeftClicker.m_Enabled == true)
			{
				Instance<Settings>::Get()->Combat.LeftClicker.m_Enabled = false;
				Sleep(500);
			}
			else
			{
				Instance<Settings>::Get()->Combat.LeftClicker.m_Enabled = true;
				Sleep(500);
			}
		}

		if (GetAsyncKeyState(Instance<Settings>::Get()->Combat.AimAssist.m_Bind) & 0x8000)
		{
			if (Instance<Settings>::Get()->Combat.AimAssist.m_Enabled == true)
			{
				Instance<Settings>::Get()->Combat.AimAssist.m_Enabled = false;
				Sleep(500);
			}
			else
			{
				Instance<Settings>::Get()->Combat.AimAssist.m_Enabled = true;
				Sleep(500);
			}
		}
		if (GetAsyncKeyState(Instance<Settings>::Get()->Movement.TimerSpeed.m_Bind) & 0x8000)
		{
			if (Instance<Settings>::Get()->Movement.TimerSpeed.m_Enabled == true)
			{
				Instance<Settings>::Get()->Movement.TimerSpeed.m_Enabled = false;
				Sleep(500);
			}
			else
			{
				Instance<Settings>::Get()->Movement.TimerSpeed.m_Enabled = true;
				Sleep(500);
			}
		}

		if (GetAsyncKeyState(Instance<Settings>::Get()->Movement.Bhop.m_Bind) & 0x8000)
		{
			if (Instance<Settings>::Get()->Movement.Bhop.m_Enabled == true)
			{
				Instance<Settings>::Get()->Movement.Bhop.m_Enabled = false;
				Sleep(500);
			}
			else
			{
				Instance<Settings>::Get()->Movement.Bhop.m_Enabled = true;
				Sleep(500);
			}
		}


		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		static uint32_t cnt = 0;
		static float freq = .01f;

		if (globals->Gui.m_Rainbow) {
			globals->Gui.m_AccentColor[0] = (std::sin(freq * cnt + 0) * 127 + 128) / 255.f;
			globals->Gui.m_AccentColor[1] = (std::sin(freq * cnt + 2) * 127 + 128) / 255.f;
			globals->Gui.m_AccentColor[2] = (std::sin(freq * cnt + 4) * 127 + 128) / 255.f;

			if (cnt++ >= (uint32_t)-1)
				cnt = 0;
		}
		/**/

		if (Instance<Globals>::Get()->m_Pid) {
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, Instance<Globals>::Get()->m_Pid);
			if (!hProcess) {
				Instance<Settings>::Get()->m_Destruct = true;
			}
			else {
				DWORD dwExitCode;
				BOOL bExit = GetExitCodeProcess(hProcess, &dwExitCode);
				if (dwExitCode != STILL_ACTIVE)
				{
					Instance<Settings>::Get()->m_Destruct = true;
				}
			}
			CloseHandle(hProcess);
		}

		if (Instance<Settings>::Get()->m_Destruct)
			break;

		/**/

		ImGui::SetNextWindowPos({ 0.F, 0.f });
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.f, 0.f });
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);

			if (!bLoaded || !bLogged) {
				ImGui::SetNextWindowSize({ m_Width, m_Height });
				ImGui::Begin("##mainWindow", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
					ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
				{
					ImGui::SetCursorPos({ 0,0 });

					ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1.f, 1.f, 1.f, 0.25f));
					ImGui::SetCursorPos({ 0.f, 34.f });
					ImGui::BeginChild("##borderChild", { m_Width, 2.f }, false);
					ImGui::EndChild();
					ImGui::PopStyleColor(1);

					ImGui::SetCursorPos({ 0.f, 0.f });
					ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(30.f / 255.f, 27.f / 255.f, 30.f / 255.f, 1.f));
					ImGui::BeginChild("##titleChild", { m_Width, 35.f }, false);
					{
						ImGui::PushFont(reforma);
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(153.f / 255.f, 155.f / 255.f, 155.f / 255.f, 1.f));
						ImGui::PopFont();
						{
							ImGui::SetCursorPos({ m_Width - 55.f, 7.5f });
							ImGui::Text(ICON_MD_MINIMIZE);
							if (ImGui::IsItemHovered())
							{
								ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
								if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
									bMinimized = true;
								}
							}

							ImGui::SetCursorPos({ m_Width - 30.f, 12.5f });
							ImGui::Text(ICON_MD_CLOSE);
							if (ImGui::IsItemHovered())
							{
								ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
								if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
									Instance<Settings>::Get()->m_Destruct = true;
							}
						}
						ImGui::PopStyleColor(1);
					}
					ImGui::EndChild();

					ImGui::PopStyleColor(1);
				}

				auto mousePos = ImGui::GetMousePos();
				bTitleHovered = mousePos.y <= 20.f;

				static bool isLogoLoading{ true };
				static float loadingTime{ 0.0f };
				static int animStage{ 1 };

				if (isLogoLoading)
				{
					if (loadingTime < 1.f) {
						const float speed = 1.5f;
						loadingTime += speed * io.DeltaTime;
					}

					if (loadingTime >= 1.f) {
						animStage++;
						loadingTime = 0.f;
					}

					if (animStage == 4) {
						isLogoLoading = false;
						loadingTime = 1.f;
					}
				}

				ImVec2 pos = { 0.f , (m_Height / 2) - (115.f / 2) };
				if (animStage == 1) {
					pos.x = (float)(((m_Width - 288.f) / 2) * easeInOutCubic(loadingTime));
					iLoadingPercentage = 10 * easeInOutCubic(loadingTime);
				}

				if (animStage > 1) {
					pos.x = (m_Width - 288.f) / 2;
				}

				if (animStage > 2) {
					float yPos = (m_Height / 2) - (115.f / 2);
					pos.y = 50.f;
				}

				if (animStage == 2) {
					float yPos = (m_Height / 2) - (115.f / 2);
					pos.y = yPos - ((yPos - 50.f) * easeInOutCubic(loadingTime));
					iLoadingPercentage = 10 + 5 * easeInOutCubic(loadingTime);
				}

				if (animStage >= 3) {
					const auto size = ImVec2(50.f * 2, (50.f + style->FramePadding.y) * 2);
					ImGui::SetCursorPos({ (m_Width / 2) - (size.x / 2), (m_Height / 2) - (50.f / 2) });

					ImGui::SpinnerAngEclipse("##loadingspin",
						50.f,
						3.f,
						ImVec4(1.f, 1.f, 1.f, loadingTime <= 1.f ? loadingTime : 1.f),
						loadingTime <= 1.f ? 2.8f * loadingTime : 2.8f
					);
				}

				ImGui::SetCursorPos(pos);
				ImGui::Image(Image, { 288.f , 115.f }, { 0.f, 0.f }, { 1.f, 1.f }, { 1.f, 1.f, 1.f, animStage == 1 ? loadingTime : 1.f });

				ImGui::PushFont(medopensan);
				ImGui::SetCursorPos({10.f, m_Height - 30.f});
				ImGui::Text("Loaded at %i%%", iLoadingPercentage);
				ImGui::PopFont();

				if (animStage == 4) {
					std::thread([&] {
						globals->User.userName = "Compiled by HAYDEN";
						bLoginProceceed = true;
						bLogged = true;
						iLoadingPercentage = 40;
					}).detach();

					if (bLoginProceceed) {
						if (!Instance<Globals>::Get()->m_Pid) {
							bInstanceChoose = true;
						}

						iLoadingPercentage = 80;
						if (Instance<Globals>::Get()->m_Pid && !bInstanceChoose) {

							const auto pid = Instance<Globals>::Get()->m_Pid;
							void* buf = nullptr;
							auto size = 0;
							HANDLE hFile = CreateFileW(L"dope internal.dll", FILE_GENERIC_READ, 0x7, 0, OPEN_EXISTING, 0, 0);
							if (hFile != INVALID_HANDLE_VALUE)
							{
								DWORD bytes = 0;
								size = GetFileSize(hFile, NULL);
								buf = VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE);
								ReadFile(hFile, buf, size, &bytes, NULL);
								CloseHandle(hFile);
							}
							Instance<InjectionHelper>::Get()->Inject(Instance<Globals>::Get()->m_Pid, buf, size);
							iLoadingPercentage = 100;
							bLoaded = true;
						}
					}
				}
			}
			else {
				ImGui::SetNextWindowSize({ m_Width, m_Height });
				ImGui::Begin("##mainWindow", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
					ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
				{
					ImGui::SetCursorPos({ 0,0 });

					ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1.f, 1.f, 1.f, 0.25f));
					ImGui::SetCursorPos({ 0.f, 89.f });
					ImGui::BeginChild("##borderChild", { m_Width, 2.f }, false);
					ImGui::EndChild();
					ImGui::PopStyleColor(1);

					ImGui::SetCursorPos({ 0.f, 0.f });
					ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(30.f / 255.f, 27.f / 255.f, 30.f / 255.f, 1.f));
					ImGui::BeginChild("##titleChild", { m_Width, 90.f }, false);
					{
						ImGui::PushFont(reforma);
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(153.f / 255.f, 155.f / 255.f, 155.f / 255.f, 1.f));

						ImGui::PopFont();
						{
							ImGui::SetCursorPos({ m_Width - 55.f, 7.5f });
							ImGui::Text(ICON_MD_MINIMIZE);
							if (ImGui::IsItemHovered())
							{
								ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
								if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
									bMinimized = true;
							}

							ImGui::SetCursorPos({ m_Width - 30.f, 12.5f });
							ImGui::Text(ICON_MD_CLOSE);
							if (ImGui::IsItemHovered())
							{
								ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
								if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
									Instance<Settings>::Get()->m_Destruct = true;
							}
						}

						ImGui::PopStyleColor(1);

						ImGui::PushFont(oliviasans);
						const auto userSize = ImGui::CalcTextSize(globals->User.userName.data());

						ImGui::SetCursorPos({ 20.f, 25.f });

						ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.f);
						ImGui::BeginChild("##userbtn", { (82.5f + userSize.x + 60.f) - 40.f, 60.f });
						ImGui::EndChild();
						ImGui::PopStyleVar(1);
						ImGui::PopFont();

						ImGui::PushFont(oliviasans);
						ImGui::SetCursorPos({ 80.f, 45.f });
						ImGui::Text(globals->User.userName.data());
						ImGui::PopFont();

						ImGui::SetCursorPos({ 40.f, 40.f });
						ImGui::PushFont(iconFont);
						ImGui::Text(ICON_MD_ACCOUNT_CIRCLE);
						ImGui::PopFont();

						auto CreateCategory = [&currentTab, &currentModule, this](char const* icon, int id) {
							ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.f);
							ImGui::PushStyleColor(ImGuiCol_Text,
								currentTab == id ? ImVec4(1.f, 1.f, 1.f, 1.f) :
								ImVec4(153.f / 255.f, 155.f / 255.f, 155.f / 255.f, 1.f));

							const auto modulesCount = 5;
							const auto startPadding = m_Width - 60.f - ((modulesCount - 1) * 45.f);
							ImGui::SetCursorPos({ startPadding + (45.f * id), 40.f });
							if (ImGui::Button(icon, { 35.f, 35.f })) {
								if (currentTab != id) {
									currentModule = 0;
								}
								currentTab = id;
							}

							ImGui::PopStyleColor(1);
							ImGui::PopStyleVar(1);
						};

						ImGui::PushFont(iconFont2);
						CreateCategory("\x23", 0);
						CreateCategory("\x25", 1);
						CreateCategory("\x24", 2);
						CreateCategory("\x22", 3);
						CreateCategory("\x21", 4);
						ImGui::PopFont();
					}

					ImGui::EndChild();
					ImGui::PopStyleColor(1);

					auto mousePos = ImGui::GetMousePos();
					bTitleHovered = mousePos.y <= 20.f && ImGui::GetCurrentWindow()->ID == ImGui::FindWindowByName("##mainWindow")->ID;

					ImGui::SetCursorPos({ 0.f, 120.f });
					ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetColorU32(ImGuiCol_WindowBg));
					ImGui::BeginChild("##modulesChild", { 160.f, m_Height - 150.f }, false);
					{
						ImGui::SetCursorPos({ 20.f, 0.f });
						ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(30.f / 255.f, 27.f / 255.f, 30.f / 255.f, 1.f));
						ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.f);
						ImGui::BeginChild("##modulesBg", { 140.f, m_Height - 150.f });
						{
							static bool bModuleClicking = false;
							static float bModuleClickingTime = 0.f;
							auto CreateModuleTab = [&currentModule, &g](char const* name, int id) {
								ImGui::SetCursorPosX(20.f);
								ImGui::PushStyleColor(ImGuiCol_Text,
									currentModule == id ? ImVec4(1.f, 1.f, 1.f, 1.f) :
									ImVec4(153.f / 255.f, 155.f / 255.f, 155.f / 255.f, 1.f));

								if (bModuleClicking)
									bModuleClickingTime += 1.5f * g.IO.DeltaTime;

								if (bModuleClickingTime >= 1.f) {
									bModuleClickingTime = 0.f;
									bModuleClicking = false;
								}

								ImGui::SetCursorPosY(20.f + id * 45.f);
								ImGui::Text(name);

								if (ImGui::IsItemClicked() && currentModule != id) {
									bModuleClickingTime = 0.f;
									bModuleClicking = true;
									currentModule = id;
								}

								if (id == currentModule) {
									ImGui::SetCursorPosX(bModuleClicking ? 20.f * easeInOutCubic(bModuleClickingTime) : 20.f);
									ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1.f, 1.f, 1.f,
										bModuleClicking ? 0.95f * easeInOutCubic(bModuleClickingTime) : 0.95f)
									);
									ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 7.5f);
									ImGui::BeginChild("##zz", { 15.f, 4.f });
									ImGui::EndChild();
									ImGui::PopStyleVar(1);
									ImGui::PopStyleColor(1);
								}

								ImGui::PopStyleColor(1);
							};

							const auto logoPos = ImVec2{
								30.f,
								m_Height - 150.f - 45.f - 10.f
							};
							ImGui::SetCursorPos({ logoPos.x, logoPos.y });
							ImGui::Image(lilImage, { 163.f / 2.f, 67.f / 2.f });
							const auto logoWidth = 163.f / 2.f;
							const auto versionWidth = ImGui::CalcTextSize("CRACK");
							ImGui::SetCursorPos({ logoPos.x + (logoWidth / 2) - (versionWidth.x / 2), logoPos.y + (67.f / 2.f)});
							ImGui::PushFont(lilopensan);
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 0.75f));
							ImGui::Text("  CRACK");
							ImGui::PopStyleColor(1);
							ImGui::PopFont();

							ImGui::PushFont(opensan);
							switch (currentTab) {
							case 0:
								CreateModuleTab("Right Clicker", 0);
								CreateModuleTab("Left Clicker", 1);
								CreateModuleTab("Aim Assist", 2);
								CreateModuleTab("Reach", 3);
								CreateModuleTab("Velocity", 4);
								break;
							case 1:
								CreateModuleTab("Player ESP", 0);
								CreateModuleTab("Block ESP", 1);
								CreateModuleTab("Array List", 2);
								CreateModuleTab("Tracers", 3);
								break;
							case 2:
								CreateModuleTab("Timer", 0);
								CreateModuleTab("Bhop", 1);
								break;
							case 3:
								CreateModuleTab("Bridge Assist", 0);
								CreateModuleTab("Throw Pearl", 1);
								CreateModuleTab("Throw Pots", 2);
								CreateModuleTab("Blink", 3);
								break;
							case 4:
								CreateModuleTab("Misc", 0);
								break;
							};
							ImGui::PopFont();
						}
						ImGui::EndChild();
						ImGui::PopStyleColor(1);
						ImGui::PopStyleVar(1);
					}
					ImGui::EndChild();
					ImGui::PopStyleColor(1);

					ImGui::SetCursorPos({ 30.f + 150.f, 120.f });
					ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 1.5f);
					ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetColorU32(ImGuiCol_WindowBg));
					ImGui::BeginChild("##moduleChild", { m_Width - 200.f , m_Height - 100.f }, false);
					{
						auto CreateModuleTab = [&opensan, &medopensan, &lilopensan, &g](char const* modName, char const* modDesc, int* bind)
						{
							ImGui::SetCursorPos({ 5.f, 0.f });
							ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(30.f / 255.f, 27.f / 255.f, 30.f / 255.f, 1.f));
							ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.f);
							ImGui::BeginChild("##modchild", { 550.f, 50.f }, false);
							{
								ImGui::PushFont(opensan);
								const auto textSize = ImGui::CalcTextSize(modName);
								ImGui::SetCursorPos({ 15.f, (50.f / 2) - (textSize.y / 2) });
								ImGui::Text(modName);
								ImGui::PopFont();

								auto curPos = ImGui::GetCurrentWindow()->Pos;
								if (g.GroupStack.Size > 0 && g.GroupStack.back().WindowID == ImGui::GetCurrentWindow()->ID)
									curPos.x += ImGui::GetCurrentWindow()->DC.Indent.x;

								ImGui::SameLine();
								ImGui::GetWindowDrawList()->AddRectFilled({ curPos.x + textSize.x + 25.f, ImGui::GetCurrentWindow()->DC.CursorPos.y },
									{ curPos.x + textSize.x + 26.f, ImGui::GetCurrentWindow()->DC.CursorPos.y + textSize.y },
									ImColor(61, 67, 98, 255),
									2.5f
								);

								ImGui::SameLine();
								ImGui::PushFont(lilopensan);
								ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(136, 136, 145, 255)));
								const auto descSize = ImGui::CalcTextSize(modDesc);
								ImGui::SetCursorPos({ ImGui::GetCursorPosX() + 15.f, (50.f / 2) - (descSize.y / 2) });
								ImGui::Text(modDesc);
								ImGui::PopStyleColor(1);
								ImGui::PopFont();

								if (bind != 0) {
									ImGui::PushFont(medopensan);
									ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(55.f / 255.f, 60.f / 255.f, 86.f / 255.f, 1.f));
									ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 0.85f));
									ImGui::SetCursorPos({ 550.f - 110.f, (50.f / 2) - (25.f / 2) });
									ImGui::Hotkey("##bind", bind, { 100.f, 25.f });
									ImGui::PopStyleColor(2);
									ImGui::PopFont();
								}
							}
							ImGui::EndChild();
							ImGui::PopStyleVar(1);
							ImGui::PopStyleColor(1);
						};

						auto CreateCheck = [](char const* id, char const* name, bool* buf)
						{
							ImGui::SetCursorPosX(9.f);
							ImGui::Text(name);
							ImGui::SameLine(500.f);
							ImGui::Checkbox(id, buf);
						};

						auto CreateColorPicker = [](const char* name, const char* itemId, float* color)
						{
							ImGui::SetCursorPosX(9.f);
							const auto cursorPos = ImGui::GetCursorPos();
							ImGui::Text(name);

							ImGui::SetCursorPos({ 505.f, cursorPos.y + 2.5f });
							ImGui::CustomColorPicker(itemId, color, { 35.f, 15.f });
						};

						ImGui::PushFont(medopensan);
						switch (currentTab) {
						case 0:
							switch (currentModule)
							{
							case 0:
								CreateModuleTab("Right Clicker", "This module will right click at given CPS for you.", &Instance<Settings>::Get()->Combat.RightClicker.m_Bind);
								ImGui::Dummy({ 0.f, 5.f });
								CreateCheck("##clickerneabled", "Enabled", &Instance<Settings>::Get()->Combat.RightClicker.m_Enabled);
								CreateCheck("##blockol", "Block Only", &Instance<Settings>::Get()->Combat.RightClicker.m_OnlyBlock);

								ImGui::Spacing();
								ImGui::SliderInt("Average CPS", &Instance<Settings>::Get()->Combat.RightClicker.m_Average, 5, 25);
								break;
							case 1:
								CreateModuleTab("Left Clicker", "This module will left click at given CPS for you.", &Instance<Settings>::Get()->Combat.LeftClicker.m_Bind);
								ImGui::Dummy({ 0.f, 5.f });
								CreateCheck("##clickerneabled", "Enabled", &Instance<Settings>::Get()->Combat.LeftClicker.m_Enabled);
								CreateCheck("##weapnol", "Weapon Only", &Instance<Settings>::Get()->Combat.LeftClicker.m_OnlyWeapon);
								CreateCheck("##breakblock", "Break Block", &Instance<Settings>::Get()->Combat.LeftClicker.m_BreakBlock);
								CreateCheck("##invfill", "Inventory Fill", &Instance<Settings>::Get()->Combat.LeftClicker.m_InInventory);
								CreateCheck("##smartbh", "Smart Blockhit", &Instance<Settings>::Get()->Combat.LeftClicker.m_SmartBlockHit);

								ImGui::Spacing();
								ImGui::SliderInt("Average CPS", &Instance<Settings>::Get()->Combat.LeftClicker.m_Average, 5, 25);
								break;
							case 2:
								CreateModuleTab("Aim Assist", "This module will aim towards the closest player.", &Instance<Settings>::Get()->Combat.AimAssist.m_Bind);
								ImGui::Dummy({ 0.f, 5.f });
								CreateCheck("##aimenabled", "Enabled", &Instance<Settings>::Get()->Combat.AimAssist.m_Enabled);
								CreateCheck("##weaponly", "Weapon Only", &Instance<Settings>::Get()->Combat.AimAssist.m_OnlyWeapon);
								CreateCheck("##throughwall", "Through Wall", &Instance<Settings>::Get()->Combat.AimAssist.m_ThroughWall);
								CreateCheck("##onlywhileclicking", "Only while Clicking", &Instance<Settings>::Get()->Combat.AimAssist.m_ClickingOnly);
								CreateCheck("##pitchrand", "Pitch Randomization", &Instance<Settings>::Get()->Combat.AimAssist.m_PitchRand);

								ImGui::Spacing();
								ImGui::SliderFloat("Horizontal Speed", &Instance<Settings>::Get()->Combat.AimAssist.m_Speed, 1.f, 10.f, "%.2f");
								ImGui::Dummy({ 0, 10 });
								ImGui::SliderInt("Field of View", &Instance<Settings>::Get()->Combat.AimAssist.m_Fov, 20, 180);
								ImGui::Dummy({ 0, 10 });
								ImGui::SliderFloat("Range", &Instance<Settings>::Get()->Combat.AimAssist.m_Distance, 1.0f, 10.0f, "%.2f");

								ImGui::Spacing();
								ImGui::SetCursorPosX(9.f);
								ImGui::CustomCombo1("Mode", aimAssistItems, aimAssistItems.size(), &Instance<Settings>::Get()->Combat.AimAssist.Mode.m_Mode);
								break;
							case 3:
								CreateModuleTab("Reach", "This module will extend the distance you can hit players.", &Instance<Settings>::Get()->Combat.Reach.m_Bind);
								ImGui::Dummy({ 0.f, 5.f });
								CreateCheck("##reachenabled", "Enabled", &Instance<Settings>::Get()->Combat.Reach.m_Enabled);
								CreateCheck("##groundonly", "Only on Ground", &Instance<Settings>::Get()->Combat.Reach.m_OnGround);
								CreateCheck("##wponly", "Only with Weapon", &Instance<Settings>::Get()->Combat.Reach.m_OnlyWeapon);
								CreateCheck("##liqcheck", "Liquid Check", &Instance<Settings>::Get()->Combat.Reach.m_LiquidCheck);
								CreateCheck("##cmbmode", "Combo Mode", &Instance<Settings>::Get()->Combat.Reach.m_ComboMode);

								ImGui::Spacing();
								ImGui::SliderFloat("Distance", &Instance<Settings>::Get()->Combat.Reach.m_Distance, 3.0f, 6.0f, "%.2f");
								ImGui::Dummy({ 0, 10 });
								ImGui::SliderFloat("Hitbox", &Instance<Settings>::Get()->Combat.Reach.m_Hitbox, 0.0f, 1.0f, "%.2f");
								break;
							case 4:
								CreateModuleTab("Velocity", "This module reduce the amount of knockback you take.", &Instance<Settings>::Get()->Combat.Velocity.m_Bind);
								ImGui::Dummy({ 0.f, 5.f });
								CreateCheck("##velocityenabled", "Enabled", &Instance<Settings>::Get()->Combat.Velocity.m_Enabled);
								CreateCheck("##aironly", "In Air Only", &Instance<Settings>::Get()->Combat.Velocity.m_AirOnly);
								CreateCheck("##movingonly", "Only while Moving", &Instance<Settings>::Get()->Combat.Velocity.m_Moving);
								CreateCheck("##wponly", "Only with Weapon", &Instance<Settings>::Get()->Combat.Velocity.m_WeaponOnly);

								ImGui::Spacing();
								ImGui::SliderDouble("Horizontal", &Instance<Settings>::Get()->Combat.Velocity.m_Horizontal, 0.0, 100.0, "%.2f");
								ImGui::Dummy({ 0, 10 });
								ImGui::SliderDouble("Vertical", &Instance<Settings>::Get()->Combat.Velocity.m_Vertical, 0.0, 100.0, "%.2f");
								ImGui::Dummy({ 0, 10 });
								ImGui::SliderInt("Chance", &Instance<Settings>::Get()->Combat.Velocity.m_Chance, 0, 100);
								ImGui::Dummy({ 0, 10 });
								ImGui::SliderInt("Delay", &Instance<Settings>::Get()->Combat.Velocity.m_Delay, 0, 3);
								ImGui::Dummy({ 0, 10 });
								break;
							};
							break;
						case 1:
							switch (currentModule)
							{
							case 0:
								CreateModuleTab("Player ESP", "This module will draw a box on every player.", &Instance<Settings>::Get()->Visuals.ESP.m_Bind);
								ImGui::Dummy({ 0.f, 5.f });
								CreateCheck("##espenabled", "Enabled", &Instance<Settings>::Get()->Visuals.ESP.m_Enabled);
								CreateCheck("##espbox", "Box", &Instance<Settings>::Get()->Visuals.ESP.m_Boxes);
								CreateCheck("##espheal", "Healthbar", &Instance<Settings>::Get()->Visuals.ESP.m_Healthbar);
								CreateCheck("##espnametag", "Nametag", &Instance<Settings>::Get()->Visuals.ESP.m_Names);

								CreateColorPicker("Filled Color", "##boxfilledcolor", Instance<Settings>::Get()->Visuals.ESP.m_FilledColor);
								ImGui::Dummy({ 0, 5 });

								CreateColorPicker("Outline Color", "##boxoutlinecolor", Instance<Settings>::Get()->Visuals.ESP.m_OutlineColor);
								ImGui::Dummy({ 0, 5 });

								ImGui::CustomCombo1("Box Type", playerEspItems, playerEspItems.size(), &Instance<Settings>::Get()->Visuals.ESP.m_Mode);
								ImGui::Spacing();
								break;
							case 1:
								CreateModuleTab("Block ESP", "This module will draw a box on every added blocks.", &Instance<Settings>::Get()->Visuals.BlockESP.m_Bind);
								ImGui::Dummy({ 0.f, 5.f });
								CreateCheck("##blockespeanbled", "Enabled", &Instance<Settings>::Get()->Visuals.BlockESP.m_Enabled);

								ImGui::Spacing();
								ImGui::SliderInt("Scan Frequency (Ms)", &Instance<Settings>::Get()->Visuals.BlockESP.m_Delay, 2500, 30000);
								ImGui::Dummy({ 0, 10 });
								ImGui::SliderInt("Radius (Meters)", &Instance<Settings>::Get()->Visuals.BlockESP.m_Radius, 0, 90);
								ImGui::Dummy({ 0, 10 });

								ImGui::SetCursorPosX(9.f);
								ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.f);
								if (ImGui::Button("Block List", { 540.f, 30.f })) {
									bBlockEspMenu = true;
								}
								ImGui::PopStyleVar(1);
								break;
							case 2:
								CreateModuleTab("Array List", "This module will draw every enabled modules.", &Instance<Settings>::Get()->Visuals.ArrayList.m_Bind);
								ImGui::Dummy({ 0.f, 5.f });
								CreateCheck("##arraylistneabled", "Enabled", &Instance<Settings>::Get()->Visuals.ArrayList.m_Enabled);
								CreateCheck("##bg", "Background", &Instance<Settings>::Get()->Visuals.ArrayList.m_Background);
								CreateCheck("##colbar", "Color Bar", &Instance<Settings>::Get()->Visuals.ArrayList.m_ColorBar);
								CreateCheck("##chroma", "Chroma Wave", &Instance<Settings>::Get()->Visuals.ArrayList.m_Rainbow);
								CreateColorPicker("Color", "##arraylistcolor", Instance<Settings>::Get()->Visuals.ArrayList.m_Color);
								ImGui::Dummy({ 0, 10 });
								ImGui::SliderFloat("Wave Saturation", &Instance<Settings>::Get()->Visuals.ArrayList.m_RainbowSaturation, 0.5f, 1.f, "%.2f");
								break;
							case 3:
								CreateModuleTab("Tracers", "This module will draw a line between you and every player.", &Instance<Settings>::Get()->Visuals.Tracers.m_Bind);
								ImGui::Dummy({ 0.f, 5.f });
								CreateCheck("##tracersenabled", "Enabled", &Instance<Settings>::Get()->Visuals.Tracers.m_Enabled);
								CreateColorPicker("Color", "##tracerscolors", Instance<Settings>::Get()->Visuals.Tracers.m_Color);
								ImGui::Dummy({ 0, 10 });
								ImGui::SliderFloat("Thickness", &Instance<Settings>::Get()->Visuals.Tracers.thickness, 0.5f, 5.f, "%.2f");
								break;
							};
							break;
						case 2:
							switch (currentModule)
							{
							case 0:
								CreateModuleTab("Timer", "This module will change the game speed.", &Instance<Settings>::Get()->Movement.TimerSpeed.m_Bind);
								ImGui::Dummy({ 0.f, 5.f });
								CreateCheck("##timerenabled", "Enabled", &Instance<Settings>::Get()->Movement.TimerSpeed.m_Enabled);
								CreateCheck("##timerweapon", "Weapon Only", &Instance<Settings>::Get()->Movement.TimerSpeed.m_OnlyWeapon);
								CreateCheck("##timermoving", "Only while Moving", &Instance<Settings>::Get()->Movement.TimerSpeed.m_Moving);
								ImGui::Dummy({ 0, 10 });
								ImGui::SliderFloat("Value", &Instance<Settings>::Get()->Movement.TimerSpeed.m_Speed, 1.f, 10.f, "%.2f");
								break;
							case 1:
								CreateModuleTab("Bhop", "This module will make you jump like a bunny ^.^", &Instance<Settings>::Get()->Movement.Bhop.m_Bind);
								ImGui::Dummy({ 0.f, 5.f });
								CreateCheck("##bhopenabled", "Enabled", &Instance<Settings>::Get()->Movement.Bhop.m_Enabled);
								CreateCheck("##bhopliquid", "Disable in Liquid", &Instance<Settings>::Get()->Movement.Bhop.m_LiquidCheck);
								ImGui::Dummy({ 0, 10 });
								ImGui::SliderFloat("Power", &Instance<Settings>::Get()->Movement.Bhop.m_Power, 1.f, 10.f, "%.2f");
								break;
							};
							break;
						case 3:
							switch (currentModule)
							{
							case 0:
								CreateModuleTab("Bridge Assist", "This module will legitely assist you while briding.", &Instance<Settings>::Get()->Utilities.BridgeAssist.m_Bind);
								ImGui::Dummy({ 0.f, 5.f });
								CreateCheck("##bridgeenabled", "Enabled", &Instance<Settings>::Get()->Utilities.BridgeAssist.m_Enabled);
								CreateCheck("##bridgerclick", "Only while Right Clicking", &Instance<Settings>::Get()->Utilities.BridgeAssist.m_OnlyRightClick);
								break;
							case 1:
								CreateModuleTab("Throw Pearl", "Just as a macro, this module will throw a pearl for you.", &Instance<Settings>::Get()->Utilities.ThrowPearl.m_Bind);
								ImGui::Dummy({ 0.f, 5.f });
								CreateCheck("##throwpearlenabled", "Enabled", &Instance<Settings>::Get()->Utilities.ThrowPearl.m_Enabled);
								ImGui::Dummy({ 0, 10 });
								ImGui::SliderInt("Throw Delay", &Instance<Settings>::Get()->Utilities.ThrowPearl.m_ThrowDelay, 25, 100);
								ImGui::Dummy({ 0, 10 });
								ImGui::SliderInt("Switch Delay", &Instance<Settings>::Get()->Utilities.ThrowPearl.m_SwitchDelay, 0, 180);
								break;
							case 2:
								CreateModuleTab("Throw Pots", "Just as a macro, this module will throw pots for you.", &Instance<Settings>::Get()->Utilities.ThrowPot.m_Bind);
								ImGui::Dummy({ 0.f, 5.f });
								CreateCheck("##throwpearlenabled", "Enabled", &Instance<Settings>::Get()->Utilities.ThrowPot.m_Enabled);
								ImGui::Dummy({ 0, 10 });
								ImGui::SliderInt("Throw Delay", &Instance<Settings>::Get()->Utilities.ThrowPot.m_ThrowDelay, 25, 100);
								ImGui::Dummy({ 0, 10 });
								ImGui::SliderInt("Switch Delay", &Instance<Settings>::Get()->Utilities.ThrowPot.m_SwitchDelay, 0, 180);
								break;
							case 3:
								CreateModuleTab("Blink", "This module will simulate lag so you're TPing for your opponent.", &Instance<Settings>::Get()->Utilities.Blink.m_Bind);
								ImGui::Dummy({ 0.f, 5.f });
								CreateCheck("##blinkenabled", "Enabled", &Instance<Settings>::Get()->Utilities.Blink.m_Enabled);
								CreateCheck("##blinkpath", "Show Path", &Instance<Settings>::Get()->Utilities.Blink.m_ShowPath);
								CreateCheck("##blinkshowtimer", "Show Timer", &Instance<Settings>::Get()->Utilities.Blink.m_ShowTimer);
								CreateColorPicker("Path Color", "##arraylistcolor", Instance<Settings>::Get()->Utilities.Blink.m_MarkerColor);
								ImGui::Dummy({ 0, 10 });
								ImGui::SliderFloat("Timer Limit (s)", &Instance<Settings>::Get()->Utilities.Blink.m_TimerLimit, 2.5f, 15.f, "%.2f");
								break;
							};
							break;
						case 4:
							switch (currentModule)
							{
							case 0:
								CreateModuleTab("Miscellaneous", "In this page you will find extra settings or modules.", nullptr);
								ImGui::Dummy({ 0.f, 5.f });

								ImGui::SetCursorPosX(60.f);
								ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.f);
								ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
								ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.f, 1.f, 1.f, 0.1f));
								ImGui::PushFont(hugeIcons);
								if (ImGui::Button("\x29", { 210.f, 160.f })) {
									bConfigMenu = true;
								}

								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

								ImGui::PopFont();
								ImGui::PopStyleColor(1);
								ImGui::PopStyleVar(2);

								ImGui::SameLine();
								ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.f);
								ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
								ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.f, 1.f, 1.f, 0.1f));
								ImGui::PushFont(hugeIcons);
								if (ImGui::Button("\x30", { 210.f, 160.f })) {
									ShellExecuteA(NULL, "open", "https://www.youtube.com/channel/UCAwD-wl9AH1BVTSz2LXh5Xg", NULL, NULL, SW_SHOWNORMAL);
								}

								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

								ImGui::PopFont();
								ImGui::PopStyleColor(1);
								ImGui::PopStyleVar(2);
								break;
							};
							break;
						};
						ImGui::PopFont();
					}
					ImGui::EndChild();
					ImGui::PopStyleColor(1);
					ImGui::PopStyleVar(1);

				}
			}
			ImGui::End();

			{
				static uint64_t initialized = -1;

				if (bBlockEspMenu || bConfigMenu) {
					if (initialized == -1)
						initialized = GetTickCount64();

					const auto mousePos = ImGui::GetMousePos();
					const auto height = bBlockEspMenu ? 200.f : m_Height * 0.9f;
					if (ImGui::IsMouseClicked(0) && (
						(mousePos.x < m_Width * 0.1f || mousePos.x > m_Width * 0.9f) ||
						(mousePos.y < m_Height * 0.1f || mousePos.y > height)
						)
						&& (GetTickCount64() - initialized) > 250) {

						bConfigMenu = false;
						bBlockEspMenu = false;
						initialized = -1;
					}
				}
			}

			/**/
			{
				if (bBlockEspMenu) {
					ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(16.f / 255.f, 19.f / 255.f, 34.f / 255.f, 0.875f));
					ImGui::SetNextWindowPos({ 0.f, 0.f });
					ImGui::SetNextWindowSize({ m_Width, m_Height });

					ImGui::Begin("##blockESPList", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
					{
						ImGui::SetCursorPos({ m_Width * 0.1f, m_Height * 0.1f });
						ImGui::PopStyleColor(1);

						ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetColorU32(ImGuiCol_WindowBg));
						{
							ImGui::BeginChild("##blockespchild", { m_Width * 0.8f, 138.f }, true, ImGuiWindowFlags_NoScrollbar);
							{
								ImGui::PushFont(medopensan);

								static char blockid[4];

								auto IsUnsignedNumber = [](char const* str) {
									if (str[0] == 0)
										return false;

									for (int idx = 0; str[idx] != 0; idx++) {
										if (str[idx] < '0' || str[idx] > '9')
											return false;
									}

									return true;
								};

								ImGui::SetCursorPos({ 5.f, 5.f });
								ImGui::InputTextEx("##blockid", "Block ID", blockid, 4, ImVec2{ (m_Width * 0.8f) - 10.f, 30.f }, ImGuiInputTextFlags_None);

								static ImColor col = { 0, 197, 255 };
								ImGui::SetCursorPosX(7.f);
								ImGui::HorizontalColorPicker(&col, { (m_Width * 0.8f) - 10.f - 4.f, 30.f - 4.f }, true);

								ImGui::SetCursorPosX(5.f);
								if (ImGui::Button("Add Block", { (m_Width * 0.8f) - 10.f, 30.f })) {
									if (IsUnsignedNumber(blockid)) {
										int blockId = std::stoi(blockid);
										int idx = 0;
										while (Instance<Settings>::Get()->Visuals.BlockESP.blockList[idx] != 0) {
											unsigned char curBlockId = Instance<Settings>::Get()->Visuals.BlockESP.blockList[idx];
											if (blockId == (int)curBlockId) {
												break;
											}
											idx++;
										}

										int assembledBlock = 0;
										assembledBlock |= (unsigned char)(col.Value.x * 255.f);
										assembledBlock <<= 8;
										assembledBlock |= (unsigned char)(col.Value.y * 255.f);
										assembledBlock <<= 8;
										assembledBlock |= (unsigned char)(col.Value.z * 255.f);
										assembledBlock <<= 8;
										assembledBlock |= (unsigned char)blockId;
										Instance<Settings>::Get()->Visuals.BlockESP.blockList[idx] = assembledBlock;
										memset(blockid, NULL, 3);
									}
									else {
										MessageBoxA(NULL, "Invalid ID.", "", MB_ICONEXCLAMATION);
									}
								}

								ImGui::SetCursorPosX(5.f);
								if (ImGui::Button("Delete Block", { (m_Width * 0.8f) - 10.f, 30.f })) {
									if (IsUnsignedNumber(blockid)) {
										int blockId = std::stoi(blockid);

										int blockIdx = -1;
										for (int i = 0; Instance<Settings>::Get()->Visuals.BlockESP.blockList[i] != 0; i++) {
											unsigned char curBlockId = Instance<Settings>::Get()->Visuals.BlockESP.blockList[i];
											if (blockId == (int)curBlockId) {
												blockIdx = i;
												break;
											}
										}

										if (blockIdx != -1) {
											for (int i = blockIdx; Instance<Settings>::Get()->Visuals.BlockESP.blockList[i] != 0; i++) {
												int next = Instance<Settings>::Get()->Visuals.BlockESP.blockList[i + 1];
												Instance<Settings>::Get()->Visuals.BlockESP.blockList[i] = next;
											}
										}

										memset(blockid, NULL, 3);
									}
									else {
										MessageBoxA(NULL, "Invalid ID.", "", MB_ICONEXCLAMATION);
									}
								}

								ImGui::PopFont();
							}
							ImGui::EndChild();
						}
						ImGui::PopStyleColor(1);
					}
					ImGui::End();
				}
			}
			/**/

			/**/
			{
				if (bConfigMenu) {
					static std::once_flag flag;
					std::call_once(flag, [&userConfigs]()
					{
						auto configs = new UserConfigs(Instance<Globals>::Get()->User.m_UserID);
						configs->Reload(userConfigs);
						delete configs;
					});

					ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ 16.f / 255.f, 19.f / 255.f, 34.f / 255.f, 0.875f });
					ImGui::SetNextWindowPos({ 0.f, 0.f });
					ImGui::SetNextWindowSize({ m_Width, m_Height });

					ImGui::Begin("##ConfigsList", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
					{
						ImGui::SetCursorPos({ m_Width * 0.1f, m_Height * 0.1f });
						ImGui::PopStyleColor(1);

						ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetColorU32(ImGuiCol_WindowBg));
						{
							ImGui::BeginChild("##settingschild", { m_Width * 0.8f, m_Height * 0.8f }, true, ImGuiWindowFlags_NoScrollbar);
							{
								ImGui::SetCursorPos({ 5.f, 5.f });

								ImGui::PushFont(medopensan);
								static char configName[MAX_PATH];
								ImGui::InputTextEx("##configname", "Ex: Best PvP Config", configName, MAX_PATH, ImVec2{ ((m_Width * 0.8f) / 2.f) - 10.f, 30.f }, ImGuiInputTextFlags_None);

								ImGui::SameLine();

								if (ImGui::Button("Create", { ((m_Width * 0.8f) / 2.f) - 10.f, 30.f })) {
									if (strlen(configName) > 26 || strlen(configName) < 3)
										MessageBoxA(NULL, "Please use a shorter/or bigger name.", "", MB_ICONEXCLAMATION);
									else {
										auto configs = new UserConfigs(Instance<Globals>::Get()->User.m_UserID);
										configs->Create(UserConfigs::CreateData(configName));
										configs->Reload(userConfigs);
										delete configs;
										memset(configName, NULL, MAX_PATH);
									}
								}
								ImGui::PopFont();

								auto createConfig = [&](std::string name, int ID)
								{
									ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(30.f / 255.f, 27.f / 255.f, 30.f / 255.f, 1.f));
									ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.f);
									ImGui::BeginChild(std::string("##config" + std::to_string(ID)).data(), { 160.f, 140.f }, false, ImGuiWindowFlags_NoScrollbar);
									{
										ImGui::SetCursorPos({ ImGui::GetCursorPosX() + 5.f, ImGui::GetCursorPosY() + 2.5f });
										ImGui::Text(name.data());

										ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0, 0, 0, 0)));
										ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(ImColor(0, 0, 0, 0)));
										ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(ImColor(0, 0, 0, 0)));

										ImGui::SetCursorPos({ ImGui::GetCursorPosX() + 2.5f, ImGui::GetCursorPosY() + 85.f });
										ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(59.f / 255.f, 120.f / 255.f, 254.f / 255.f, 1.f));
										if (ImGui::Button("Load")) {
											auto configs = new UserConfigs(Instance<Globals>::Get()->User.m_UserID, ID);
											configs->Load(userConfigs);
											configs->Reload(userConfigs);
											delete configs;

											bConfigMenu = false;
										}
										ImGui::PopStyleColor(1);

										if (ImGui::IsItemHovered())
											g.MouseCursor = ImGuiMouseCursor_Hand;

										const char* actionName = "Update";
										auto size = ImGui::CalcTextSize(actionName);
										ImGui::SameLine(145.f / 2 - (size.x / 2));
										ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(34, 177, 62)));
										if (ImGui::Button("Update")) {
											auto configs = new UserConfigs(Instance<Globals>::Get()->User.m_UserID, ID);
											configs->Update(UserConfigs::CreateData(name));
											configs->Reload(userConfigs);
											delete configs;
										}
										ImGui::PopStyleColor(1);

										if (ImGui::IsItemHovered())
											g.MouseCursor = ImGuiMouseCursor_Hand;

										ImGui::SameLine(105.f);
										ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(124, 24, 23)));
										if (ImGui::Button("Delete")) {
											auto configs = new UserConfigs(Instance<Globals>::Get()->User.m_UserID, ID);
											configs->Delete();
											configs->Reload(userConfigs);
											delete configs;
										}
										ImGui::PopStyleColor(1);

										if (ImGui::IsItemHovered())
											g.MouseCursor = ImGuiMouseCursor_Hand;

										ImGui::PopStyleColor(3);
									}
									ImGui::EndChild();
									ImGui::PopStyleVar(1);
									ImGui::PopStyleColor(1);

									ImGui::SameLine();
									if (ImGui::GetCursorPosX() >= 504) {
										ImGui::Spacing();
									}

									if (ImGui::GetCursorPosX() < 25)
										ImGui::SetCursorPosX(15.f);
								};

								ImGui::Spacing();

								ImGui::PushFont(medopensan);

								ImGui::SetCursorPosX(15.f);

								for (const auto& configJson : userConfigs)
								{
									if (!configJson.empty()) {
										nlohmann::json j = nlohmann::json::parse(configJson);
										createConfig(j["name"].get<std::string>(), std::stoi(j["id"].get<std::string>()));
									}
								}

								ImGui::PopFont();
							}
							ImGui::EndChild();
						}
						ImGui::PopStyleColor(1);
					}

					ImGui::End();
				}
			}
			/**/

			/**/
			if (bInstanceChoose) {
				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(16.f / 255.f, 19.f / 255.f, 34.f / 255.f, 0.875f));
				ImGui::SetNextWindowPos({ 0.f, 0.f });
				ImGui::SetNextWindowSize({ m_Width, m_Height });

				ImGui::Begin("##instanceChoose", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
				{
					ImGui::SetCursorPos({ m_Width * 0.1f, (m_Height / 2) - (417.f / 2) });
					ImGui::PopStyleColor(1);

					ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetColorU32(ImGuiCol_WindowBg));
					{
						ImGui::BeginChild("##instancesChild", { m_Width * 0.8f, 417.f }, true, ImGuiWindowFlags_NoScrollbar);
						{
							{
								auto size = ImGui::CalcTextSize(ICON_MD_CLOSE);
								ImGui::SetCursorPos({ (m_Width * 0.8f) - 25.f, (20.f / 2) - 1.f });

								ImGui::Text(ICON_MD_CLOSE);
								if (ImGui::IsItemHovered())
								{
									ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
									if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
										Instance<Settings>::Get()->m_Destruct = true;
								}
							}

							ImGui::SetCursorPos({ (m_Width * 0.8f) / 2 - (288.f / 2), 5.f });
							ImGui::Image(Image, { 288.f , 116.4375f });

							ImGui::PushFont(lilopensan);
							ImVec2 txtSize = ImGui::CalcTextSize("Make sure to load ONLY when you are in game (Not in the menu). Otherwise visuals won't work.");
							ImGui::Spacing();
							ImGui::SetCursorPosX((m_Width * 0.8f) / 2 - (txtSize.x / 2));
							ImGui::Text("Make sure to load ONLY when you are in game (Not in the menu). Otherwise some module won't work.");

							ImGui::SetCursorPosY(150.f);

							auto LoadInstance = [](HWND hWnd)
							{
								GetWindowThreadProcessId(hWnd, &Instance<Globals>::Get()->m_Pid);
							};

							static auto isChecked = false;
							static auto instanceCount = 0;
							for (HWND hwnd = GetTopWindow(NULL); hwnd != NULL; hwnd = GetNextWindow(hwnd, GW_HWNDNEXT))
							{
								if (!IsWindowVisible(hwnd))
									continue;

								int length = GetWindowTextLength(hwnd);
								if (length == 0)
									continue;

								CHAR cName[MAX_PATH];
								GetClassNameA(hwnd, cName, _countof(cName));
								if (strcmp(cName, "LWJGL") != 0) {
									continue;
								}

								instanceCount++;

								char* title = new char[length + 1];
								GetWindowTextA(hwnd, title, length + 1);

								ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.f);
								ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 0.f, 0.f });

								ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(255, 255, 255, 175)));

								ImGui::SetCursorPosX((m_Width * 0.8f) / 2 - (285.f / 2));
								if (ImGui::Button(title, { 275.f , 30.f })) {
									LoadInstance(hwnd);
									bInstanceChoose = false;
								}
								ImGui::PopStyleColor(1);
								ImGui::PopStyleVar(2);
							}

							if (instanceCount == 1 && !isChecked) {
								LoadInstance(FindWindowA("LWJGL", nullptr));
								bInstanceChoose = false;
							}
							isChecked = true;
							ImGui::PopFont();
						}
						ImGui::EndChild();
					}
					ImGui::PopStyleColor(1);

				}
				ImGui::End();
			}
			/**/
			if (!bDraging && !bBlockEspMenu && !bConfigMenu && !bInstanceChoose)
				bDraging = bTitleHovered && ImGui::IsMouseDragging(ImGuiMouseButton_Left);

			if (bDraging && !ImGui::IsMouseDragging(ImGuiMouseButton_Left))
				bDraging = false;

			if (ImGui::IsMouseClicked(0))
			{
				POINT CursorPosition;
				RECT MenuPosition;

				GetCursorPos(&CursorPosition);
				GetWindowRect(globals->m_Hwnd, &MenuPosition);

				menuMovementX = CursorPosition.x - MenuPosition.left;
				menuMovementY = CursorPosition.y - MenuPosition.top;
			}

			if (bDraging)
			{
				POINT cursor_position;

				GetCursorPos(&cursor_position);
				SetWindowPos(globals->m_Hwnd, nullptr, cursor_position.x - menuMovementX, cursor_position.y - menuMovementY, 0, 0, SWP_NOSIZE);
			}

			ImGui::PopStyleVar(2);
		}

		// Rendering
		ImGui::Render();
		const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		g_pSwapChain->Present(1, 0); // Present with vsync

		if (bMinimized) {
			ShowWindow(globals->m_Hwnd, SW_MINIMIZE);
		}
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(globals->m_Hwnd);
	::UnregisterClass(wc.lpszClassName, wc.hInstance);
}