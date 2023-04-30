#include "pch.h"
#include "LeftClicker.h"

#include "../Settings.h"
#include "../../../Helper/Communication.h"
#include "../../../Helper/Utils.h"

#include "../../../Game/Classes/Minecraft.h"
#include "../../../Game/Classes/Player.h"
#include "../../../Game/Classes/World.h"
#include "../../../Game/Classes/ItemStack.h"
#include "../../../Game/Classes/MovingObjectPosition.h"
#include "../../../Game/Classes/GuiScreen.h"

#include "../../../Helper/Utils.h"

static std::vector<int> sentDelays = {};
static int RandomDelay(int avg)
{
    if (avg <= 5)
        avg = 7;

    const int curAvg = RandomInteger(avg - RandomInteger(1, RandomInteger(1, 4)), avg + RandomInteger(1, RandomInteger(1, 3)));
    int delay = RandomInteger(RandomInteger(396, 479), RandomInteger(500, 610)) / curAvg;

    delay -= RandomInteger(RandomInteger(-2, -1), RandomInteger(0, 1));
    if (RandomInteger(0, 100) < RandomInteger(0, 60))
        delay -= RandomInteger(RandomInteger(-4, -2), RandomInteger(0, 6));

    if (sentDelays.size() > 0) {
        int count = std::count(sentDelays.begin(), sentDelays.end(), delay);
        if ((100 * count) / sentDelays.size() >= 7) {
            return RandomDelay(avg);
        }

        int latestDelay = sentDelays.at(sentDelays.size() - 1);
        if (abs(latestDelay - delay) <= 3) {
            return RandomDelay(avg);
        }
    }

    if (sentDelays.size() == 100) {
        sentDelays.clear();
        return RandomDelay(avg - 2);
    }

    return delay;
}

static bool wasHeld = false, shouldBlockHit = false;
static unsigned long long lastBlockhit = 0;
static int hurtCount = 0, targetHurtCount = 0;
void LeftClicker::Run(JNIEnv* env)
{
	if (!Communication::GetSettings()->Combat.LeftClicker.m_Enabled) {
		Sleep(5);
		return;
	}


    const auto minecraftWindow = FindWindowA("LWJGL", nullptr);
    if (GetForegroundWindow() != minecraftWindow) {
        Sleep(5);
        return;
    }

    const auto thePlayer = (Player*)Minecraft::GetThePlayer(env);
    const auto ob = (MovingObjectPosition*)Minecraft::GetObjectMouseOver(env);
    const auto currentScreen = (GuiScreen*)Minecraft::GetCurrentScreen(env);
    const auto theWorld = (World*)Minecraft::GetTheWorld(env);
	if (!thePlayer || !theWorld) {
		Sleep(5);
		return;
	}

	if (Communication::GetSettings()->Combat.LeftClicker.m_BreakBlock && ob->IsAimingBlock(env)) {
		Sleep(5);
		return;
	}

	if (Communication::GetSettings()->Combat.LeftClicker.m_OnlyWeapon && !((ItemStack*)thePlayer->GetHeldItem(env))->IsWeapon(env) && !currentScreen->IsInMenu()) {
		Sleep(5);
		return;
	}

    if (Communication::GetSettings()->Combat.LeftClicker.m_InInventory)
    {
		if (!currentScreen->IsInventory(env) && currentScreen->IsInMenu()) {
			Sleep(5);
			return;
		}
    }
    else
    {
		if (currentScreen->IsInMenu()) {
			Sleep(5);
			return;
		}
    }

    auto wrapTo180 = [](float value)
    {
        if (value >= 180.f)
            value -= 360.f;
        if (value < -180.f)
            value += 360.f;
        return value;
    };

    if (Communication::GetSettings()->Combat.LeftClicker.m_SmartBlockHit)
    {
        Player* target = nullptr;
        const Vec3D thePlayerPos = thePlayer->GetPos(env);
        const float thePlayerYaw = thePlayer->GetRotationYaw(env);
        {
            float dist = (std::numeric_limits<float>::max)();

            for (const auto& pl : theWorld->GetPlayerEntities(env))
            {
                if (env->IsSameObject((jobject)pl, (jobject)thePlayer))
                    continue;

                const Vec3D playerPos = pl->GetPos(env);
                const double distanceX = thePlayerPos.x - playerPos.x;
                const double distanceZ = thePlayerPos.z - playerPos.z;
                const float fov = atan2f((float)distanceZ, (float)distanceX) * 180.f / std::numbers::pi - 90.f;
                dist = wrapTo180(fmodf(fov - thePlayerYaw, 360.f));
            
                if (fabsf(fov) <= dist)
                {
                    dist = fabsf(fov);
                    target = pl;
                }
            }
        }

        if (target != nullptr)
        {
            if (thePlayer->GetHurtResistantTime(env) >= (thePlayer->GetMaxHurtResistantTime(env) - 4)) {
                hurtCount++;
            }

            if (target->GetHurtResistantTime(env) >= (target->GetMaxHurtResistantTime(env) - 4)) {
                targetHurtCount++;
            }

            if (ob->IsAimingEntity(env))
            {
                if (GetTickCount64() - lastBlockhit > RandomInteger(500, 1500)) {
                    if (hurtCount >= 2) {
                        shouldBlockHit = true;
                        hurtCount = 0;
                    }
                    else {
                        if (targetHurtCount >= 2) {
                            shouldBlockHit = true;
                            targetHurtCount = 0;
                        }
                        else if (hurtCount == 0 && targetHurtCount == 0) {
                            shouldBlockHit = true;
                        }
                    }
                }
            }
        }
    }

	const int delay = RandomDelay(Communication::GetSettings()->Combat.LeftClicker.m_Average);
	if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
	{
		wasHeld = false;
		Sleep(delay);
		return;
	}
	else {
		if (!wasHeld) {
			Sleep(delay);
			wasHeld = true;
		}
	}

    sentDelays.push_back(delay);
    sendClick(minecraftWindow, delay);
    if (shouldBlockHit) {
        sendClick(minecraftWindow, delay, true);
        shouldBlockHit = false;
        lastBlockhit = GetTickCount64();
    }
}