#include "pch.h"
#include "Module.h"

#include "Settings.h"
#include "../Hack.h"
#include  "../../Helper/Communication.h"

#pragma region Modules includes
#include "Misc/Blink.h"
#include "Misc/ThrowPots.h"
#include "Misc/ThrowPearl.h"
#include "Misc/BridgeAssist.h"
#include "Visuals/ESP.h"
#include "Visuals/ArrayList.h"
//#include "Visuals/Chams.h"
#include "Visuals/BlockESP.h"
#include "Combat/Velocity.h"
#include "Combat/Reach.h"
#include "Combat/AimAssist.h"
#include "Combat/LeftClicker.h"
#include "Combat/RightClicker.h"
#include "Movement/Bhop.h"
#include "Movement/TimerSpeed.h"
#pragma endregion

#pragma region Game classes includes
#include "../../Game/Classes/Minecraft.h"
#include "../../Game/Classes/ActiveRenderInfo.h"
#include "../../Game/Classes/GuiScreen.h"
#pragma endregion

void Modules::InitializeModules()
{
	m_Modules.push_back(new ESP());
	m_Modules.push_back(new ArrayList());
	m_Modules.push_back(new Velocity());
	m_Modules.push_back(new Reach());
	m_Modules.push_back(new Blink());
	m_Modules.push_back(new AimAssist());
	m_Modules.push_back(new LeftClicker());
	m_Modules.push_back(new RightClicker());
	//m_Modules.push_back(new Chams());
	m_Modules.push_back(new Bhop());
	m_Modules.push_back(new TimerSpeed());
	m_Modules.push_back(new ThrowPots());
	m_Modules.push_back(new ThrowPearl());
	m_Modules.push_back(new BlockESP());
	m_Modules.push_back(new BridgeAssist());

	for (auto& mod : m_Modules)
	{
		auto thread = std::thread([&mod]()
		{
			JNIEnv* env;
			jint res = g_Instance->GetJVM()->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
			
			if (res == JNI_EDETACHED) {
				res = g_Instance->GetJVM()->AttachCurrentThreadAsDaemon(reinterpret_cast<void**>(&env), nullptr);
				if (res != JNI_OK) {
					return;
				}
			}

			while (!Communication::GetSettings()->m_Destruct)
			{
				if (!Minecraft::GetTheMinecraft(env)) {
					Sleep(25);
					continue;
				}

				if (Minecraft::GetThePlayer(env) && Minecraft::GetTheWorld(env)
					&& ( ((GuiScreen*)Minecraft::GetCurrentScreen(env))->IsChat(env) || ((GuiScreen*)Minecraft::GetCurrentScreen(env))->IsInventory(env) || !((GuiScreen*)Minecraft::GetCurrentScreen(env))->IsInMenu()) ) {
					mod->Run(env);
				}
				else {
					Sleep(25);
				}

				Sleep(1);
			}

			g_Instance->GetJVM()->DetachCurrentThread();
		});
		
		m_ModuleThread.push_back(std::move(thread));
	}
}

void Modules::DestroyModules()
{
	for (auto& thread : m_ModuleThread)
	{
		if (thread.joinable())
			thread.join();
	}

	for (const auto mod : m_Modules)
	{
		delete mod;
	}

	m_Modules.clear();
}

std::vector<Module*> Modules::m_Modules;
std::vector<std::thread> Modules::m_ModuleThread;