#include "pch.h"
#include "ThrowPearl.h"

#include "../Settings.h"
#include "../../../Helper/Communication.h"
#include "../../../Helper/Utils.h"

#include "../../../Game/Classes/Minecraft.h"
#include "../../../Game/Classes/Player.h"
#include "../../../Game/Classes/World.h"
#include "../../../Game/Classes/ActiveRenderInfo.h"
#include "../../../Game/Classes/RenderManager.h"
#include "../../../Game/Classes/Timer.h"
#include "../../../Game/Classes/ItemStack.h"
#include "../../../Game/Classes/InventoryPlayer.h"

void ThrowPearl::Run(JNIEnv* env)
{
	const auto throwPearlSettings = Communication::GetSettings()->Utilities.ThrowPearl;
	if (!throwPearlSettings.m_Enabled) {
		Sleep(5);
		return;
	}

	const auto thePlayer = (Player*)Minecraft::GetThePlayer(env);
	const auto inventoryPlayer = (InventoryPlayer*)thePlayer->GetInventoryPlayer(env);
	if (GetAsyncKeyState(throwPearlSettings.m_Bind) & 0x8000)
	{
		bool isThrowd = false;

		const auto initialSlot = inventoryPlayer->GetSlot(env);
		for (int i = 0; i < 9; i++)
		{
			const auto itemstack = (ItemStack*)inventoryPlayer->GetStackInSlot(i, env);
			if (itemstack->IsEnderPearl(env)) {
				Sleep(throwPearlSettings.m_SwitchDelay);
				inventoryPlayer->SetSlot(i, env);

				sendClick(FindWindowA("LWJGL", nullptr), throwPearlSettings.m_ThrowDelay / 2, true);
				isThrowd = true;
			}

			if (isThrowd) {
				Sleep(throwPearlSettings.m_SwitchDelay);
				inventoryPlayer->SetSlot(initialSlot, env);
			}
		}

		isThrowd = false;
	}

	Sleep(5);
}
