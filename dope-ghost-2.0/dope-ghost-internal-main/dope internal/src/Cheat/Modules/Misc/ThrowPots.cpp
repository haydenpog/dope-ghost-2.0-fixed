#include "pch.h"
#include "ThrowPots.h"

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

void ThrowPots::Run(JNIEnv* env)
{
	const auto throwPotsSettings = Communication::GetSettings()->Utilities.ThrowPot;
	if (!throwPotsSettings.m_Enabled) {
		Sleep(5);
		return;
	}
	
	const auto thePlayer = (Player*)Minecraft::GetThePlayer(env);
	const auto inventoryPlayer = (InventoryPlayer*)thePlayer->GetInventoryPlayer(env);
	if (GetAsyncKeyState(throwPotsSettings.m_Bind) & 0x8000)
	{
		int throwd = 0;
		bool isHealed = false;
		if (throwPotsSettings.Mode.m_Mode == 2)
			isHealed = thePlayer->GetHealth(env) > 16;

		const auto initialSlot = inventoryPlayer->GetSlot(env);
		for (int i = 0; i < 9 && !isHealed; i++)
		{
			const auto itemstack = (ItemStack*)inventoryPlayer->GetStackInSlot(i, env);
			const auto metadata = itemstack->GetMetadata(env);
			if (metadata == 16421 /*instant health 2*/)
			{
				Sleep(throwPotsSettings.m_SwitchDelay);
				inventoryPlayer->SetSlot(i, env);

				sendClick(FindWindowA("LWJGL", nullptr), throwPotsSettings.m_ThrowDelay / 2, true);

				if (throwPotsSettings.Mode.m_Mode == 0) {
					isHealed = true;
				}
				else if (throwPotsSettings.Mode.m_Mode == 1) {
					throwd++;

					if (throwd == 2)
						isHealed = true;
				}
				else if (throwPotsSettings.Mode.m_Mode == 2) {
					isHealed = thePlayer->GetHealth(env) > 16;
				}


				if (isHealed) {
					Sleep(throwPotsSettings.m_SwitchDelay);
					inventoryPlayer->SetSlot(initialSlot, env);
				}
			}

			if (i == 8 && inventoryPlayer->GetSlot(env) != initialSlot)
			{
				Sleep(throwPotsSettings.m_SwitchDelay);
				inventoryPlayer->SetSlot(initialSlot, env);
			}
		}

		isHealed = false;
		throwd = 0;
	}

	Sleep(5);
}
