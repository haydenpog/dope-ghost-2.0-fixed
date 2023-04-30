#include "pch.h"
#include "RightClicker.h"

#include "../Settings.h"
#include "../../../Helper/Communication.h"
#include "../../../Helper/Utils.h"

#include "../../../Game/Classes/Minecraft.h"
#include "../../../Game/Classes/Player.h"
#include "../../../Game/Classes/World.h"
#include "../../../Game/Classes/ItemStack.h"

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

void RightClicker::Run(JNIEnv* env)
{
	static bool wasHeld = false;
	if (!Communication::GetSettings()->Combat.RightClicker.m_Enabled) {
		Sleep(5);
		return;
	}

	const auto minecraftWindow = FindWindowA("LWJGL", nullptr);
	if (GetForegroundWindow() != minecraftWindow) {
		Sleep(5);
		return;
	}

	const auto thePlayer = (Player*)Minecraft::GetThePlayer(env);
	const auto theWorld = (World*)Minecraft::GetTheWorld(env);
	if (!thePlayer || !theWorld) {
		Sleep(5);
		return;
	}

	if (Communication::GetSettings()->Combat.RightClicker.m_OnlyBlock && !((ItemStack*)thePlayer->GetHeldItem(env))->IsBlock(env)) {
		Sleep(5);
		return;
	}

	auto wrapTo180 = [](float value)
	{
		if (value >= 180.f)
			value -= 360.f;
		if (value < -180.f)
			value += 360.f;
		return value;
	};

	const int delay = RandomDelay(Communication::GetSettings()->Combat.RightClicker.m_Average);
	if (!(GetAsyncKeyState(VK_RBUTTON) & 0x8000))
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
	sendClick(minecraftWindow, delay, true);
}