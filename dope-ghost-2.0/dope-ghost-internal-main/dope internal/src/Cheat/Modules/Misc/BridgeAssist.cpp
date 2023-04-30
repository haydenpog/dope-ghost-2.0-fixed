#include "pch.h"
#include "BridgeAssist.h"

#include "../Settings.h"
#include "../../../Helper/Communication.h"
#include "../../../Helper/Utils.h"

#include "../../../Game/Classes/Minecraft.h"
#include "../../../Game/Classes/Player.h"
#include "../../../Game/Classes/World.h"
#include "../../../Game/Classes/Timer.h"
#include "../../../Game/Classes/ItemStack.h"
#include "../../../Game/Classes/InventoryPlayer.h"
#include "../../../Game/Classes/KeyBinding.h"
#include "../../../Game/Classes/GameSettings.h"
#include "../../../Game/Classes/Block.h"
#include "../../../Game/Classes/BlockPos.h"
#include "../../../Game/Classes/MovingObjectPosition.h"

static bool sneaked = false;
void BridgeAssist::Run(JNIEnv* env)
{
	const auto bridgeAssistSettings = Communication::GetSettings()->Utilities.BridgeAssist;
	if (!bridgeAssistSettings.m_Enabled) {
		Sleep(5);
		return;
	}

	const auto thePlayer = (Player*)Minecraft::GetThePlayer(env);
	const auto theWorld = (World*)Minecraft::GetTheWorld(env);
	const auto gameSettings = (GameSettings*)(Minecraft::GetGameSettings(env));
	const auto ob = (MovingObjectPosition*)Minecraft::GetObjectMouseOver(env);
	const auto sneakBind = (KeyBinding*)(gameSettings->GetKeyBindSneak(env));

	if (!((ItemStack*)thePlayer->GetHeldItem(env))->IsBlock(env) && thePlayer->GetRotationPitch(env) >= 50) {
		Sleep(5);
		return;
	}

	if (bridgeAssistSettings.m_OnlyRightClick && !(GetAsyncKeyState(VK_RBUTTON) & 0x8000))
	{
		if (sneaked) {
			sneakBind->SetPressed(false, env);
			sneaked = false;
		}

		Sleep(5);
		return;
	}

	const auto myPos = thePlayer->GetPos(env);
	const bool is1_8 = g_GameVersion == CASUAL_1_7_10 || g_GameVersion == FORGE_1_7_10 || g_GameVersion == LUNAR_1_7_10;
	const double y = !is1_8 ? myPos.y - 1.0 : myPos.y - 1.63;

	const auto currentBlockObject = (Block*)BlockPos::GetBlock(Vec3D{ myPos.x, y, myPos.z }, (jobject)theWorld, env);
	const auto nextBlockObject = (Block*)BlockPos::GetBlock(Vec3D{ myPos.x, y - 1.0, myPos.z }, (jobject)theWorld, env);

	if (currentBlockObject->IsAir(env) && nextBlockObject->IsAir(env) && thePlayer->IsOnGround(env)) {
		if (ob->IsAimingBlock(env)) {
			sneaked = true;
			sneakBind->SetPressed(true, env);
		}
	}
	else {
		if (thePlayer->IsOnGround(env)) {
			sneaked = false;
			sneakBind->SetPressed(false, env);
		}
	}

	Sleep(5);
}
