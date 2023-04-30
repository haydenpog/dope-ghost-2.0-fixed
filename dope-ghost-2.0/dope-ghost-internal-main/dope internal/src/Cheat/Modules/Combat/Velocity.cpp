#include "pch.h"
#include "Velocity.h"

#include "../Settings.h"
#include "../../../Helper/Communication.h"

#include "../../../Game/Classes/Minecraft.h"
#include "../../../Game/Classes/Player.h"
#include "../../../Game/Classes/ItemStack.h"

void Velocity::Run(JNIEnv* env)
{
	if (!Communication::GetSettings()->Combat.Velocity.m_Enabled) {
		Sleep(5);
		return;
	}

	const auto thePlayer = (Player*)Minecraft::GetThePlayer(env);
	const auto myPrevPos = thePlayer->GetPreviousPos(env);
	const auto myPos = thePlayer->GetPos(env);

	if (Communication::GetSettings()->Combat.Velocity.m_AirOnly && thePlayer->IsOnGround(env)) {
		Sleep(5);
		return;
	}

	if (Communication::GetSettings()->Combat.Velocity.m_Moving && (myPrevPos.x == myPos.x && myPrevPos.y == myPos.y && myPrevPos.z == myPos.z)) {
		Sleep(5);
		return;
	}

	if (Communication::GetSettings()->Combat.Velocity.m_WeaponOnly) {
		const auto heldItem = (ItemStack*)thePlayer->GetHeldItem(env);
		if (!heldItem->IsWeapon(env)) {
			Sleep(5);
			return;
		}
	}

	if (rand() % 100 <= Communication::GetSettings()->Combat.Velocity.m_Chance)
	{
		const int target_change = (thePlayer->GetMaxHurtResistantTime(env) - 1) - Communication::GetSettings()->Combat.Velocity.m_Delay;
		const int hurt_resistant_time = thePlayer->GetHurtResistantTime(env);

		if (hurt_resistant_time == target_change) {
			const double horizontal = Communication::GetSettings()->Combat.Velocity.m_Horizontal / 100;
			const double vertical = Communication::GetSettings()->Combat.Velocity.m_Vertical / 100;

			thePlayer->SetMotionX(thePlayer->GetMotionX(env) * horizontal, env);
			thePlayer->SetMotionY(thePlayer->GetMotionY(env) * vertical, env);
			thePlayer->SetMotionZ(thePlayer->GetMotionZ(env) * horizontal, env);
		}
	}
}
