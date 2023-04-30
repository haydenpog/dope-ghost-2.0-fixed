#include "pch.h"
#include "Hack.h"

#include "Modules/Module.h"
#include "Modules/Settings.h"

#include "../Helper/Communication.h"

#include "../Game/Mapper.h"
#include "../Helper/Utils.h"

#include "../../vendors/minhook/MinHook.h"
#include "../../vendors/imgui/imgui.h"
#include "../../vendors/imgui/imgui_impl_opengl2.hpp"

Hack::Hack()
	: m_InitializationState(WORKING), m_Jvm(nullptr), m_CleanJVMTI(true)
{
	m_Memory = std::make_unique<Memory>();
}

void Hack::Attach()
{
	jsize vmCount = 0;
	if (JNI_GetCreatedJavaVMs(&m_Jvm, 1, &vmCount) != JNI_OK || vmCount == 0) {
		m_InitializationState = ERR;
		return;
	}

	ImGui::CreateContext();
	ImGui_ImplOpenGL2_Init();
	ImGui::GetIO().IniFilename = nullptr;

	jint res = g_Instance->GetJVM()->GetEnv(reinterpret_cast<void**>(&m_Env), JNI_VERSION_1_6);

	if (res == JNI_EDETACHED) {
		res = g_Instance->GetJVM()->AttachCurrentThread(reinterpret_cast<void**>(&m_Env), nullptr);
		if (res != JNI_OK) {
			m_InitializationState = ERR;
		}
	}
}

void Hack::Detach()
{
	ImGui_ImplOpenGL2_Shutdown();
	ImGui::DestroyContext();

	MH_Uninitialize();
	m_Jvm->DetachCurrentThread();
	Communication::Unload();

	Modules::DestroyModules();
	//m_Jvm->DestroyJavaVM(); /*weird function, does it really destroys its jvm? does it even work? @TODO*/
	m_Jvm = nullptr;
}

HackState Hack::CleanupJVMTI() const
{
	if (!m_CleanJVMTI)
		return INCOMPLETE;

	const auto jvm = (uintptr_t)GetModuleHandleW(L"jvm.dll");
	
#pragma region offsets
	const auto isJvm16 = g_GameVersion == LUNAR_1_7_10
		|| g_GameVersion == LUNAR_1_8;

	const auto isCustomJvm8 = g_GameVersion == FEATHER_1_8;

	const auto _globally_initialized_offset = isJvm16 ? 0x16 : 0x7;
	const auto always_capabilities_startOffset = isJvm16 ? 0x0 : 0x50;
	const auto always_capabilities_size = isJvm16 ? 0x68 : 0x58;
	const auto ext_functions_offset = (isJvm16 || isCustomJvm8) ? 0x1A : 0x16; // 0x1A feather jvm
	const auto ext_functions_startOffset = isJvm16 ? 0x0 : 0x8;
	constexpr auto ext_functions_size = 0x10; // 8 & 16
	constexpr auto _head_environment_startOffset = 0x8; // 8 & 16
	constexpr auto _head_environment_size = 0x10; // 8 & 16
	const auto JvmtiEventControllerPrivate__initialized_offset = isJvm16 ? 0x10 : 0x8;
	constexpr auto JvmtiEventControllerPrivate__initialized_size = 0x10; // 8 & 16
#pragma endregion
	
	const auto _initialized = m_Memory->GetAddress(m_Memory->FindPattern(jvm, "80 3D ? ? ? 00 00 0F 85 ? ? ? ? 83 3D ? ? ? ? ? 48 89 58 F8") + 0x2, 0x1);
	const auto _globally_initialized = m_Memory->GetAddress(m_Memory->FindPattern(jvm, "75 16 E8 ? ? ? ? E8 ? ? ? ?") - (_globally_initialized_offset - 0x2), 0x1);
	const auto always_capabilities = m_Memory->GetAddress(m_Memory->FindPattern(jvm, "22 05 ? ? ? ? 41 88 00") + 0x2);
	const auto ext_functions = m_Memory->GetAddress(m_Memory->FindPattern(jvm, "0F 8C ? ? ? ? 8B 00") - (ext_functions_offset - 0x3));
	
	if (IsBadReadPtr(_initialized) || IsBadReadPtr(_globally_initialized) || IsBadReadPtr(always_capabilities) || IsBadReadPtr(ext_functions))
	{
		return ERR;
	}

	*_initialized = 0;
	*_globally_initialized = 0;

	ZeroMemory(always_capabilities - always_capabilities_startOffset, always_capabilities_size);
	ZeroMemory(ext_functions - ext_functions_startOffset, ext_functions_size);
	ZeroMemory(_globally_initialized - JvmtiEventControllerPrivate__initialized_offset, JvmtiEventControllerPrivate__initialized_size);
	return SUCCESS;
}

void Hack::InitializeGame()
{
	const auto _initialized = m_Memory->GetAddress(m_Memory->FindPattern((uintptr_t)GetModuleHandleW(L"jvm.dll"), "80 3D ? ? ? 00 00 0F 85 ? ? ? ? 83 3D ? ? ? ? ? 48 89 58 F8") + 0x2, 0x1);
	m_CleanJVMTI = (*_initialized == 0);

	jvmtiEnv* jvmtiEnvironment;
	if (m_Jvm->GetEnv((void**)&jvmtiEnvironment, JVMTI_VERSION_1_1) != JNI_OK) {
		m_InitializationState = ERR;
		return;
	}

	jclass* classes; jint classCount;
	jvmtiEnvironment->GetLoadedClasses(&classCount, &classes);
	for (int i = 0; i < classCount; i++)
	{
		auto klass = (Klass*)(classes[i]);
		m_CachedKlass.emplace(std::make_pair(klass->GetName(m_Env), klass));
	}

	/**/
	
	HWND currentWindowHandle;
	for (currentWindowHandle = GetTopWindow(NULL); currentWindowHandle != NULL; currentWindowHandle = GetNextWindow(currentWindowHandle, GW_HWNDNEXT))
	{
		if (!IsWindowVisible(currentWindowHandle))
			continue;

		int length = GetWindowTextLength(currentWindowHandle);
		if (length == 0)
			continue;

		CHAR cName[MAX_PATH];
		GetClassNameA(currentWindowHandle, cName, _countof(cName));
		if (strcmp(cName, "LWJGL") != 0) {
			continue;
		}

		DWORD pid;
		GetWindowThreadProcessId(currentWindowHandle, &pid);
		if (pid == GetCurrentProcessId()) {
			break;
		}
	}

	const auto windowTitle = GetWindowTitle(currentWindowHandle);
	const auto is1_7 = std::strstr(windowTitle, "1.7.10") != nullptr;
	const auto vanillaMappings = std::strstr(windowTitle ,"Badlion Minecraft") != nullptr;
	if (std::strstr(windowTitle, "Lunar Client") != nullptr)
	{
		g_GameVersion = is1_7 ? LUNAR_1_7_10 : LUNAR_1_8;
	}
	
	if (g_GameVersion != LUNAR_1_7_10 && g_GameVersion != LUNAR_1_8)
	{
		const auto launchWrapper = FindClass("net/minecraft/launchwrapper/LaunchClassLoader");
		const auto launchClazz = FindClass("net/minecraft/launchwrapper/Launch");
		const auto classLoader = launchClazz->GetField(m_Env, "classLoader", "Lnet/minecraft/launchwrapper/LaunchClassLoader;", true);

		if (launchClazz && launchWrapper && classLoader && !vanillaMappings)
		{
			if (!vanillaMappings)
			{
				g_GameVersion = is1_7 ? FORGE_1_7_10 : FORGE_1_8;
				if (FindClass("net/minecraft/client/Minecraft") == NULL)
				{
					g_GameVersion = is1_7 ? CASUAL_1_7_10 : CASUAL_1_8;
				}
			}
		}
		else {
			g_GameVersion = is1_7 ? CASUAL_1_7_10 : CASUAL_1_8;
		}

		if(FindClass("net/digitalingot/featheropt/FeatherCoreMod"))
			g_GameVersion = FEATHER_1_8;
	}
	
	Mapper::Initialize(g_GameVersion);
	if (FindClass(Mapper::Get("net/minecraft/client/Minecraft")) == NULL)
	{
		m_InitializationState = ERR;
		return;
	}

	uint64_t timer = GetTickCount64();
	while (FindClass(Mapper::Get("net/minecraft/client/renderer/ActiveRenderInfo")) == NULL && !Communication::GetSettings()->m_Destruct) {
		m_CachedKlass.clear();

		jclass* classes; 
		jint classCount;
		jvmtiEnvironment->GetLoadedClasses(&classCount, &classes);
		for (int i = 0; i < classCount; i++)
		{
			auto klass = (Klass*)(classes[i]);
			m_CachedKlass.emplace(std::make_pair(klass->GetName(m_Env), klass));
		}

		if (GetTickCount64() - timer > 30000) {
			MessageBoxA(NULL, "Initialization is taking longer than usual, aborting. Please report this issue happen again.", "", MB_OK | MB_ICONERROR);
			m_InitializationState = ERR;
			return;
		}

		Sleep(1);
	}

	m_InitializationState = SUCCESS;
}

Klass* Hack::FindClass(std::string klassName) const
{
	std::ranges::replace(klassName.begin(), klassName.end(), '/', '.');
	if (m_CachedKlass.contains(klassName))
		return m_CachedKlass.at(klassName);

	return nullptr;
}

std::unique_ptr<Hack> g_Instance;