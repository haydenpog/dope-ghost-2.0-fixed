#include "pch.h"
#include "wglSwapBuffers.h"

#include "../Hack.h"

#include "../Modules/Module.h"
#include "../Modules/Settings.h"

#include "../../Helper/Communication.h"
#include "../../Helper/HookFunction.h"

#include "../../../vendors/minhook/MinHook.h"
#include "../../../vendors/imgui/imgui.h"
#include "../../../vendors/imgui/imgui_impl_opengl2.hpp"
#include "../../../vendors/imgui/font.hpp"

#include <gl/GL.h>

int(__stdcall* g_origWglSwapBuffers)(HDC);

static std::once_flag fontsInitialized;
static bool contextInitialized = false;
static JNIEnv* env;
bool __stdcall wglSwapBuffersHook(HDC hdc)
{
	if (!contextInitialized && !Communication::GetSettings()->m_Destruct)
	{
		jint res = g_Instance->GetJVM()->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
		if (res == JNI_EDETACHED) {
			res = g_Instance->GetJVM()->AttachCurrentThread(reinterpret_cast<void**>(&env), nullptr);
		}

		contextInitialized = true;
	}

	if (!Communication::GetSettings()->m_Destruct) {
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		glViewport(0, 0, viewport[2], viewport[3]);

		for (const auto& mod : Modules::GetRegisteredModules())
		{
			mod->OnRender(env);
		}

		/*imgui*/
		auto& io = ImGui::GetIO();
		io.DisplaySize = ImVec2{ (float)viewport[2] , (float)viewport[3] };

		std::call_once(fontsInitialized, [&io]() {
			io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::sfui_medium_compressed_data, Fonts::sfui_medium_compressed_size, 20.f);
		});

		ImGui_ImplOpenGL2_NewFrame();
		ImGui::NewFrame();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
		ImGui::Begin(("##"), nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);
		{
			ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
			ImGui::SetWindowSize(ImVec2(static_cast<float>(viewport[2]), static_cast<float>(viewport[3])), ImGuiCond_Always);
			{
				for (const auto& mod : Modules::GetRegisteredModules())
				{
					mod->OnImGuiRender(env);
				}
			}
		}
		ImGui::End();
		ImGui::PopStyleVar(2);

		ImGui::Render();
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
		/**/
	}

	if (contextInitialized && Communication::GetSettings()->m_Destruct)
	{
		g_Instance->GetJVM()->DetachCurrentThread();
		contextInitialized = false;
	}

	return g_origWglSwapBuffers(hdc);
}

static HookFunction hookFunc([]()
{
	MH_Initialize();
	MH_CreateHookApi(L"opengl32.dll", "wglSwapBuffers", wglSwapBuffersHook, (void**)&g_origWglSwapBuffers);
	MH_EnableHook(MH_ALL_HOOKS);
});