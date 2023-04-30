#include "pch.h"
#include "AimAssist.h"

#include "../Settings.h"
#include "../../../Helper/Communication.h"
#include "../../../Helper/Utils.h"

#include "../../../Game/Classes/Minecraft.h"
#include "../../../Game/Classes/Player.h"
#include "../../../Game/Classes/World.h"
#include "../../../Game/Classes/ItemStack.h"
#include "../../../Game/Classes/MovingObjectPosition.h"

#include <timeapi.h>
#include <numbers>

static Player* target = nullptr;
static ULONGLONG targetUpdate = NULL;
static float lastPitch;
void AimAssist::Run(JNIEnv* env)
{
	if (!Communication::GetSettings()->Combat.AimAssist.m_Enabled) {
		Sleep(5);
		return;
	}

	if (Communication::GetSettings()->Combat.AimAssist.m_ClickingOnly && !(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
		Sleep(5);
		return;
	}

    const auto thePlayer = (Player*)Minecraft::GetThePlayer(env);
    const auto theWorld = (World*)Minecraft::GetTheWorld(env);
    const auto ob = (MovingObjectPosition*)Minecraft::GetObjectMouseOver(env);

	if (!thePlayer || !theWorld || !ob) {
		Sleep(5);
		return;
	}

    if (Communication::GetSettings()->Combat.AimAssist.m_OnlyWeapon)
    {
        const auto heldItem = (ItemStack*)thePlayer->GetHeldItem(env);

		if (!heldItem->IsWeapon(env)) {
			Sleep(5);
			return;
		}
    }

	if (!Communication::GetSettings()->Combat.AimAssist.m_ThroughWall && ob->IsAimingBlock(env)) {
		Sleep(5);
		return;
	}

    const Vec3D thePlayerPos = thePlayer->GetPos(env);
    if(GetTickCount64() - targetUpdate > 100)
    {
        {
            double dist = (std::numeric_limits<double>::max)();

            for (const auto& pl : theWorld->GetPlayerEntities(env))
            {
                if (env->IsSameObject((jobject)pl, (jobject)thePlayer))
                    continue;

                const Vec3D playerPos = pl->GetPos(env);
                if (const double dist_bet = sqrt(pow(thePlayerPos.x - playerPos.x, 2.0) + pow(thePlayerPos.y - playerPos.y, 2.0) + pow(thePlayerPos.z - playerPos.z, 2.0)); dist_bet <= dist)
                {
                    dist = dist_bet;
                    target = pl;
                }
            }
        }

        targetUpdate = GetTickCount64();
    }

    auto SortVector = [](std::vector<double>& v)
    {
        for (size_t i = 0; i < v.size(); i++)
        {
            for (size_t j = i + 1; j < v.size(); j++)
            {
                if (v[i] > v[j])
                {
                    const double x = v[i];
                    v[i] = v[j];
                    v[j] = x;
                }
            }
        }
    };

    auto IsInRange = [](Vec3D v) {
        if (v.z > v.y && (v.x > v.z || v.x < v.y) ||
            v.y > v.z && (v.x > v.y || v.x < v.z))
                return false;

        return true;
    };

    auto WrapTo180 = [](float value)
    {
        if (value >= 180.f)
            value -= 360.f;
        if (value < -180.f)
            value += 360.f;
        return value;
    };

    auto RandomFloat = [](float min, float max)
    {
        float f = (float)rand() / RAND_MAX;
        return min + f * (max - min);
    };

    if(target != nullptr)
    {
        if (target->GetHealth(env) <= 0.f)
            return;
        
        const auto targetPos = target->GetPos(env);
        if(const double targetDistance = targetPos.distance(thePlayerPos); targetDistance <= Communication::GetSettings()->Combat.AimAssist.m_Distance)
        {
            const auto lastYaw = thePlayer->GetPrevRotationYaw(env);
            const auto yaw = thePlayer->GetRotationYaw(env);
            const auto pitch = thePlayer->GetRotationPitch(env);

            double distanceX = targetPos.x - thePlayerPos.x;
            double distanceZ = targetPos.z - thePlayerPos.z;

            if(Communication::GetSettings()->Combat.AimAssist.Mode.m_Mode == 1)
            {
                const std::vector<Vec3D> corners
                {
                    { targetPos.x - 0.30, targetPos.y, targetPos.z + 0.30 },
                    { targetPos.x - 0.30, targetPos.y, targetPos.z - 0.30 },
                    { targetPos.x + 0.30, targetPos.y, targetPos.z - 0.30 },
                    { targetPos.x + 0.30, targetPos.y, targetPos.z + 0.30 },
                };
                
                const double distanceCorner = sqrt(pow(corners.at(0).x - thePlayerPos.x, 2) + pow(corners.at(0).y - thePlayerPos.y, 2) + pow(corners.at(0).z - thePlayerPos.z, 2));
                const double distanceCorner2 = sqrt(pow(corners.at(1).x - thePlayerPos.x, 2) + pow(corners.at(1).y - thePlayerPos.y, 2) + pow(corners.at(1).z - thePlayerPos.z, 2));
                const double distanceCorner3 = sqrt(pow(corners.at(2).x - thePlayerPos.x, 2) + pow(corners.at(2).y - thePlayerPos.y, 2) + pow(corners.at(2).z - thePlayerPos.z, 2));
                const double distanceCorner4 = sqrt(pow(corners.at(3).x - thePlayerPos.x, 2) + pow(corners.at(3).y - thePlayerPos.y, 2) + pow(corners.at(3).z - thePlayerPos.z, 2));

                std::vector<double> closestCorners{ distanceCorner, distanceCorner2, distanceCorner3, distanceCorner4 };
                SortVector(closestCorners);

                Vec3D closestCorner = corners.at(0);
                if (closestCorners.at(0) == distanceCorner) closestCorner = corners.at(0);
                if (closestCorners.at(0) == distanceCorner2) closestCorner = corners.at(1);
                if (closestCorners.at(0) == distanceCorner3) closestCorner = corners.at(2);
                if (closestCorners.at(0) == distanceCorner4) closestCorner = corners.at(3);

                distanceX = closestCorner.x - thePlayerPos.x;
                distanceZ = closestCorner.z - thePlayerPos.z;
            }

            auto fov = atan2f(static_cast<float>(distanceZ), static_cast<float>(distanceX)) * 180.f / std::numbers::pi_v<float> - 90.f;
            fov = WrapTo180(fmodf(fov - yaw, 360.f));
            
            if (static_cast<int>(abs(fov)) <= Communication::GetSettings()->Combat.AimAssist.m_Fov && abs(fov) > 2.f)
            {
                const float speed = Communication::GetSettings()->Combat.AimAssist.m_Speed + RandomFloat(-1.f, 0.25f);
                fov *= RandomFloat(0.25f, 0.65f) * (speed + RandomFloat(1.f, 5.f));

                if (abs(fov) <= 5.f && RandomInteger(1, 8) <= 2) {
                    fov *= -1;
                }

                const float rndAdd = RandomFloat(0.005f, 0.001f);
                const float newYaw = thePlayer->GetRotationYaw(env) + fov * rndAdd;
                const float newPrevYaw = thePlayer->GetPrevRotationYaw(env) + fov * rndAdd;

                thePlayer->SetRotationYaw(newYaw, env);
                thePlayer->SetPrevRotationYaw(newPrevYaw, env);

                if (Communication::GetSettings()->Combat.AimAssist.m_PitchRand) {
                    if (lastPitch == pitch && RandomInteger(1, 10) == 1 && abs(pitch) < 80) {
                        const float newRand = RandomFloat(-0.075f, 0.075f);
                        thePlayer->SetRotationPitch(thePlayer->GetRotationPitch(env) + newRand, env);
                        thePlayer->SetPrevRotationPitch(thePlayer->GetPrevRotationPitch(env) + newRand, env);
                    }

                    lastPitch = thePlayer->GetRotationPitch(env);
                }
            }
        }
    }
}
