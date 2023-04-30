#include "pch.h"
#include "TimerSpeed.h"

#include "../Settings.h"
#include "../../../Helper/Communication.h"

#include "../../../Game/Classes/Minecraft.h"
#include "../../../Game/Classes/Player.h"
#include "../../../Game/Classes/World.h"
#include "../../../Game/Classes/ActiveRenderInfo.h"
#include "../../../Game/Classes/RenderManager.h"
#include "../../../Game/Classes/Timer.h"
#include "../../../Game/Classes/ItemStack.h"

void TimerSpeed::Run(JNIEnv* env)
{
	if (!(Minecraft::GetTheWorld(env) && !Minecraft::GetCurrentScreen(env)))
		return;

	const auto thePlayer = (Player*)Minecraft::GetThePlayer(env);
	const auto theTimer = (Timer*)Minecraft::GetTimer(env);
	const auto timerSettings = Communication::GetSettings()->Movement.TimerSpeed;
	if (!timerSettings.m_Enabled) {
		if (theTimer->GetTimerSpeed(env) != 1.f)
			theTimer->SetTimerSpeed(1.f, env);

		Sleep(100);
		return;
	}

	if (timerSettings.m_OnlyWeapon && !((ItemStack*)thePlayer->GetHeldItem(env))->IsWeapon(env)) {
		if (theTimer->GetTimerSpeed(env) != 1.f) {
			theTimer->SetTimerSpeed(1.f, env);
		}

		Sleep(100);
		return;
	}

	Vec3D local_prev = thePlayer->GetPreviousPos(env);
	Vec3D local_pos = thePlayer->GetPos(env);
	if (timerSettings.m_Moving && (local_prev.x == local_pos.x && local_prev.y == local_pos.y && local_prev.z == local_pos.z))
	{
		if (theTimer->GetTimerSpeed(env) != 1.f) {
			theTimer->SetTimerSpeed(1.f, env);
		}

		Sleep(100);
		return;
	}

	theTimer->SetTimerSpeed(timerSettings.m_Speed, env);
	Sleep(100);
}