#include "pch.h"
#include "Reach.h"

#include "../Settings.h"
#include "../../../Helper/Communication.h"

#include "../../../Game/Classes/Minecraft.h"
#include "../../../Game/Classes/Player.h"
#include "../../../Game/Classes/World.h"
#include "../../../Game/Classes/AxisAlignedBB.h"
#include "../../../Game/Classes/ItemStack.h"
#include "../../../Game/Classes/MovingObjectPosition.h"

#include <numbers>

void Reach::Run(JNIEnv* env)
{
	if (!Communication::GetSettings()->Combat.Reach.m_Enabled) {
		Sleep(5);
		return;
	}

	const auto thePlayer = (Player*)Minecraft::GetThePlayer(env);
	const auto theWorld = (World*)Minecraft::GetTheWorld(env);
	const auto ob = (MovingObjectPosition*)Minecraft::GetObjectMouseOver(env);

	if (Communication::GetSettings()->Combat.Reach.m_OnGround && !thePlayer->IsOnGround(env)) {
		Sleep(5);
		return;
	}

	if (Communication::GetSettings()->Combat.Reach.m_OnlyWeapon) {
		const auto heldItem = (ItemStack*)thePlayer->GetHeldItem(env);

		if (!heldItem->IsWeapon(env)) {
			Sleep(5);
			return;
		}
	}

	if (Communication::GetSettings()->Combat.Reach.m_LiquidCheck && thePlayer->IsInWater(env)) {
		Sleep(5);
		return;
	}

	Player* target = nullptr;
	const Vec3D thePlayerPos = thePlayer->GetPos(env);
	{
		double dist = (std::numeric_limits<double>::max)();

		for (const auto& pl : theWorld->GetPlayerEntities(env))
		{
			if (env->IsSameObject((jobject)pl, (jobject)thePlayer))
				continue;

			const Vec3D playerPos = pl->GetPos(env);
			if (const double dist_bet = sqrt(pow(thePlayerPos.x - playerPos.x, 2.0) + pow(thePlayerPos.y - playerPos.y, 2.0) + pow(thePlayerPos.z - playerPos.z, 2.0)); dist_bet <= dist && dist_bet <= 6.0)
			{
				dist = dist_bet;
				target = pl;
			}
		}
	}

	static ULONGLONG lastLocalHit = 0, lastTargetHit = 0, firstTargetHit = 0;
	static int targetHitCount = 0;
	static bool inCombo = false;

	if (target != nullptr && Communication::GetSettings()->Combat.Reach.m_ComboMode) {
		if (thePlayer->GetHurtResistantTime(env) >= thePlayer->GetMaxHurtResistantTime(env)) {
			lastLocalHit = GetTickCount64();
		}

		if (target->GetHurtResistantTime(env) >= target->GetMaxHurtResistantTime(env) && ob->IsAimingEntity(env)) {
			if (targetHitCount == 0) {
				firstTargetHit = GetTickCount64();
			}

			lastTargetHit = GetTickCount64();
			targetHitCount++;
		}

		if (targetHitCount >= 2 && (lastLocalHit < firstTargetHit) &&
			firstTargetHit != NULL) {
			inCombo = true;
		}

		if (inCombo && lastLocalHit > firstTargetHit || inCombo && GetTickCount64() - lastTargetHit > 1500) {
			inCombo = false;
			lastLocalHit = NULL;
			firstTargetHit = NULL;
			targetHitCount = NULL;
			lastTargetHit = NULL;
		}
	}

	if (Communication::GetSettings()->Combat.Reach.m_ComboMode && inCombo) {
		return;
	}

	if (target != nullptr)
	{
		const Vec3D playerPos = target->GetPos(env);
		const auto distBetween = thePlayerPos.distance(playerPos);
		
		if ((distBetween > Communication::GetSettings()->Combat.Reach.m_Distance + 0.5 || distBetween <= 3.000) && Communication::GetSettings()->Combat.Reach.m_Hitbox == 0.f)
			return;
		
		auto x = (float)playerPos.x;
		auto z = (float)playerPos.z;

		const float hypothenuse_distance = hypotf((float)(thePlayerPos.x - playerPos.x), (float)(thePlayerPos.z - playerPos.z));
		float dist = Communication::GetSettings()->Combat.Reach.m_Distance - 3.f;

		while (distBetween > 3.0 && (distBetween < (dist + 3.0)) && dist > 0.05)
			dist -= 0.05f;

		if (hypothenuse_distance < dist)
			dist -= hypothenuse_distance;

		auto GetAngle = [](float ex, float ez, Vec3D mypos)
		{
			const auto x = ex - (float)mypos.x;
			const auto z = ez - (float)mypos.z;

			auto y = (float)(-atanf(x / z) * 180.f / (float)std::numbers::pi);
			if (z < 0.f && x < 0.f) {
				y = 90.f + (float)(atanf(z / x) * 180.f / (float)std::numbers::pi);
			}
			else if (z < 0.f && x > 0.f) {
				y = -90.f + (float)(atanf(z / x) * 180.f / (float)std::numbers::pi);
			}
			return y;
		};

		const float r = GetAngle(x, z, thePlayerPos);
		const float ax = cosf((float)((r + 90.f) * std::numbers::pi / 180.f));
		const float b = sinf((float)((r + 90.f) * std::numbers::pi / 180.f));

		x = (float)playerPos.x;
		z = (float)playerPos.z;

		x -= ax * dist;
		z -= b * dist;

		const float newWidth = (0.6f + Communication::GetSettings()->Combat.Reach.m_Hitbox) /*entity width*/ / 2.0f;
		const AxisAlignedBB_t curHitbox = ((AxisAlignedBB*)target->GetBoundingBox(env))->GetNativeBoundingBox(env);

		AxisAlignedBB_t bb{};
		bb.minX = x - newWidth;
		bb.minY = curHitbox.minY;
		bb.minZ = z - newWidth;

		bb.maxX = x + newWidth;
		bb.maxY = curHitbox.maxY;
		bb.maxZ = z + newWidth;

		((AxisAlignedBB*)target->GetBoundingBox(env))->SetNativeBoundingBox(bb, env);
	}
}
