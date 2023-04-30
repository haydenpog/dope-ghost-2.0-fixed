#include "pch.h"
#include "BlockESP.h"

#include "../Settings.h"

#include "../../../Game/Classes/Minecraft.h"
#include "../../../Game/Classes/Player.h"
#include "../../../Game/Classes/World.h"
#include "../../../Game/Classes/RenderManager.h"
#include "../../../Game/Classes/ActiveRenderInfo.h"
#include "../../../Game/Classes/Timer.h"
#include "../../../Game/Classes/AxisAlignedBB.h"
#include "../../../Game/Classes/Block.h"
#include "../../../Game/Classes/BlockPos.h"

#include "../../../Helper/Utils.h"
#include "../../../Helper/Communication.h"

#include <gl/GL.h>

static std::vector<std::pair<int, Vec3D>> blockList;
static uint64_t timer;
void BlockESP::Run(JNIEnv* env)
{
	const auto espSettings = Communication::GetSettings()->Visuals.BlockESP;
	if (!espSettings.m_Enabled)
		return;

	if (!(Minecraft::GetTheWorld(env) && !Minecraft::GetCurrentScreen(env)))
		return;

	auto IsValidBlock = [](int id, const int* allowedBlocks)
	{
		int idx = 0;
		while (allowedBlocks[idx] != 0x0) {
			unsigned char blockId = allowedBlocks[idx];
			if (id == blockId)
				return true;

			idx++;
		}

		return false;
	};

	const auto thePlayer = (Player*)Minecraft::GetThePlayer(env);
	const auto theWorld = (World*)Minecraft::GetTheWorld(env);
	const auto myPos = thePlayer->GetPos(env);

	std::vector<std::pair<int, Vec3D>> tmpList;
	if (GetTickCount64() - timer >= espSettings.m_Delay) {
		int radius = espSettings.m_Radius;
		for (int x = -radius; x < radius; x++)
		{
			for (int y = -radius; y < radius; y++)
			{
				for (int z = -radius; z < radius; z++)
				{
					if ((myPos.y + y) < 0 || (myPos.y + y) > 255)
						continue;

					const int newX = myPos.x + x;
					const int newY = myPos.y + y;
					const int newZ = myPos.z + z;
					const auto blockObject = (Block*)BlockPos::GetBlock(Vec3D{ (double)newX, (double)newY, (double)newZ}, (jobject)theWorld, env);
					const auto blockId = blockObject->GetID(env);
					if (blockId == 0)
						continue;

					if (IsValidBlock(blockId, espSettings.blockList)) {
						tmpList.push_back(std::make_pair(blockId, Vec3D{ myPos.x + x, myPos.y + y, myPos.z + z }));
					}
				}
			}
		}
	}

	blockList = tmpList;
}

void BlockESP::OnRender(JNIEnv* env)
{
	const auto espSettings = Communication::GetSettings()->Visuals.BlockESP;
	if (!espSettings.m_Enabled)
		return;

	auto drawOutlinedBox = [](AxisAlignedBB_t boundingBox)
	{
		glBegin(3);
		glVertex3f(boundingBox.minX, boundingBox.minY, boundingBox.minZ);
		glVertex3f(boundingBox.maxX, boundingBox.minY, boundingBox.minZ);
		glVertex3f(boundingBox.maxX, boundingBox.minY, boundingBox.maxZ);
		glVertex3f(boundingBox.minX, boundingBox.minY, boundingBox.maxZ);
		glVertex3f(boundingBox.minX, boundingBox.minY, boundingBox.minZ);
		glEnd();

		glBegin(3);
		glVertex3f(boundingBox.minX, boundingBox.maxY, boundingBox.minZ);
		glVertex3f(boundingBox.maxX, boundingBox.maxY, boundingBox.minZ);
		glVertex3f(boundingBox.maxX, boundingBox.maxY, boundingBox.maxZ);
		glVertex3f(boundingBox.minX, boundingBox.maxY, boundingBox.maxZ);
		glVertex3f(boundingBox.minX, boundingBox.maxY, boundingBox.minZ);
		glEnd();

		glBegin(1);
		glVertex3f(boundingBox.minX, boundingBox.minY, boundingBox.minZ);
		glVertex3f(boundingBox.minX, boundingBox.maxY, boundingBox.minZ);
		glVertex3f(boundingBox.maxX, boundingBox.minY, boundingBox.minZ);
		glVertex3f(boundingBox.maxX, boundingBox.maxY, boundingBox.minZ);
		glVertex3f(boundingBox.maxX, boundingBox.minY, boundingBox.maxZ);
		glVertex3f(boundingBox.maxX, boundingBox.maxY, boundingBox.maxZ);
		glVertex3f(boundingBox.minX, boundingBox.minY, boundingBox.maxZ);
		glVertex3f(boundingBox.minX, boundingBox.maxY, boundingBox.maxZ);
		glEnd();
	};

	if (Minecraft::GetTheWorld(env) && !Minecraft::GetCurrentScreen(env))
	{
		glPushMatrix();
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(ActiveRenderInfo::GetProjection(env).data());
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(ActiveRenderInfo::GetModelView(env).data());

		glPushMatrix();
		glEnable(GL_LINE_SMOOTH);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
		glDepthMask(false);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glLineWidth(1.f);

		auto GetColorFromId = [&espSettings](int id) {
			int idx = 0;
			while (espSettings.blockList[idx] != 0x0) {
				unsigned char blockId = espSettings.blockList[idx];
				if (id == blockId) {
					unsigned char r = espSettings.blockList[idx] >> 24;
					unsigned char g = espSettings.blockList[idx] >> 16;
					unsigned char b = espSettings.blockList[idx] >> 8;
					return Vec3((r * 1.f) / 255.f, (g * 1.f) / 255.f, (b * 1.f) / 255.f);
				}

				idx++;
			}

			return Vec3(-1.f, -1.f, -1.f);
		};

		const auto renderManager = (RenderManager*)Minecraft::GetRenderManager(env);
		for (const auto& bb : blockList)
		{
			const auto blockId = bb.first;
			const auto blockColor = GetColorFromId(blockId);
			if (blockColor.x == -1.f)
				continue;

			const auto renderPos = renderManager->GetRenderPos(env);
			const auto xPos = (int)(bb.second.x) - renderPos.x;
			const auto yPos = (int)(bb.second.y) - renderPos.y;
			const auto zPos = (int)(bb.second.z) - renderPos.z;

			glColor4f(blockColor.x, blockColor.y, blockColor.z, 1.f);
			drawOutlinedBox(AxisAlignedBB_t{ static_cast<float>(xPos), static_cast<float>(yPos), static_cast<float>(zPos), static_cast<float>(xPos + 1), static_cast<float>(yPos + 1), static_cast<float>(zPos + 1) });
		}

		glDisable(GL_BLEND);
		glDepthMask(true);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_LINE_SMOOTH);
		glPopMatrix();

		glPopMatrix();
	}
}
