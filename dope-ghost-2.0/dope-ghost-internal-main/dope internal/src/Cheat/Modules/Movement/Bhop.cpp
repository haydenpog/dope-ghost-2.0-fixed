#include "pch.h"
#include "Bhop.h"

#include "../Settings.h"
#include "../../../Helper/Communication.h"

#include "../../../Game/Classes/Minecraft.h"
#include "../../../Game/Classes/Player.h"
#include "../../../Game/Classes/World.h"
#include "../../../Game/Classes/ActiveRenderInfo.h"
#include "../../../Game/Classes/RenderManager.h"
#include "../../../Game/Classes/Timer.h"

static uint64_t timer = GetTickCount64();
static double playerSpeed = 0.0;
static bool slowDown{ false };
void Bhop::Run(JNIEnv* env)
{
	const auto bhopSettings = Communication::GetSettings()->Movement.Bhop;
	if (!bhopSettings.m_Enabled) {
		Sleep(5);
		return;
	}

	if (Minecraft::GetTheWorld(env) && !Minecraft::GetCurrentScreen(env))
	{
		const auto thePlayer = (Player*)Minecraft::GetThePlayer(env);
		const auto theTimer = (Timer*)Minecraft::GetTimer(env);

		if (!thePlayer || !theTimer)
			return;

		if (thePlayer->IsInWater(env) && bhopSettings.m_LiquidCheck)
			return;

		if (thePlayer->IsOnGround(env) && (thePlayer->GetMoveForward(env) != 0 || thePlayer->GetMoveStrafing(env) != 0) && GetTickCount64() - timer > 300) {
			timer = GetTickCount64();
			thePlayer->SetMotionY(0.41, env);
			playerSpeed = ((bhopSettings.m_Power / 1.45) * 0.2873) * 1.90;
			slowDown = true;
		}
		else {
			if (playerSpeed != 0.0) {
				if (slowDown) {
					playerSpeed -= 0.7 * (playerSpeed = 0.2873);
					slowDown = false;
				}
				else {
					playerSpeed -= playerSpeed / 159.0;
				}
			}
		}

		playerSpeed = max(playerSpeed, (bhopSettings.m_Power / 1.45) * 0.2873);
		auto forward = [&thePlayer, &theTimer, &env](float speed)
		{
			float forward = thePlayer->GetMoveForward(env);
			float side = thePlayer->GetMoveStrafing(env);
			float yaw = thePlayer->GetPrevRotationYaw(env) + (thePlayer->GetRotationYaw(env) - thePlayer->GetPrevRotationYaw(env)) * theTimer->GetRenderPartialTicks(env);
			if (forward != 0.0f)
			{
				if (side > 0.0f) {
					yaw += ((forward > 0.0f) ? -45 : 45);
				}
				else if (side < 0.0f) {
					yaw += ((forward > 0.0f) ? 45 : -45);
				}
				side = 0.0f;
				if (forward > 0.0f) {
					forward = 1.0f;
				}
				else if (forward < 0.0f) {
					forward = -1.0f;
				}
			}

			float sin = sinf(((yaw + 90.f) * std::numbers::pi / 180.f));
			float cos = cosf(((yaw + 90.f) * std::numbers::pi / 180.f));
			double posX = (double)(forward * speed * cos + side * speed * sin);
			double posZ = (double)(forward * speed * sin - side * speed * cos);
			return Vec3D(posX, 0, posZ);
		};

		Vec3D dir = forward((float)playerSpeed);
		if (abs(dir.x) < 10.0 && abs(dir.z) < 10.0) { // lil check just incase
			thePlayer->SetMotionX(dir.x, env);
			thePlayer->SetMotionZ(dir.z, env);
		}
	}

	Sleep(5);
}
