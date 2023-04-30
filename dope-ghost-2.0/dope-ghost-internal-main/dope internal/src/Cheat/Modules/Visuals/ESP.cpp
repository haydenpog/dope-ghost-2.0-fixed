#include "pch.h"
#include "ESP.h"

#include "../Settings.h"

#include "../../../Game/Classes/Minecraft.h"
#include "../../../Game/Classes/Player.h"
#include "../../../Game/Classes/World.h"
#include "../../../Game/Classes/RenderManager.h"
#include "../../../Game/Classes/ActiveRenderInfo.h"
#include "../../../Game/Classes/Timer.h"
#include "../../../Game/Classes/AxisAlignedBB.h"

#include "../../../Helper/Utils.h"
#include "../../../Helper/Communication.h"

#include "../../../../vendors/imgui/imgui.h"

#include <gl/GL.h>
#include <sstream>

/*TODO: cleanup*/

struct Entity_t
{
	std::string name;
	double distToLocal;
	float health;
};

static std::vector<std::pair<Entity_t, std::vector<Vec3>>> coordsToDraw = {}; // for performances purposes, we do all the maths and stuff in another thread that is not the render one
static std::vector<AxisAlignedBB_t> hitboxToDraw = {};
void ESP::Run(JNIEnv* env)
{
	if (!Communication::GetSettings()->Visuals.ESP.m_Enabled && !Communication::GetSettings()->Visuals.Tracers.m_Enabled && !Communication::GetSettings()->Visuals.ESP.m_Boxes)
		return;

	const auto renderManager = (RenderManager*)Minecraft::GetRenderManager(env);
	const auto thePlayer = (Player*)Minecraft::GetThePlayer(env);
	if (!thePlayer || !renderManager)
		return;

	std::vector<std::pair<Entity_t, std::vector<Vec3>>> tmpCoords;
	std::vector<AxisAlignedBB_t> tmpHitboxes;
	for (const auto& pl : ((World*)Minecraft::GetTheWorld(env))->GetPlayerEntities(env))
	{
		if (env->IsSameObject((jobject)pl, (jobject)thePlayer))
			continue;

		const auto renderPartialTicks = ((Timer*)Minecraft::GetTimer(env))->GetRenderPartialTicks(env);
		const auto renderPos = renderManager->GetRenderPos(env);
		const auto localPlayerPos = thePlayer->GetPos(env);
		const auto playerPos = pl->GetPos(env);

		const auto playerLastTickPos = pl->GetLastTickPos(env);

		auto playerBoundingBox = AxisAlignedBB_t(
			static_cast<float>((playerPos.x - 0.4f) - renderPos.x + -playerPos.x + playerLastTickPos.x + (playerPos.x - playerLastTickPos.x) * renderPartialTicks),
			static_cast<float>((playerPos.y - 0.1f) - renderPos.y + -playerPos.y + playerLastTickPos.y + (playerPos.y - playerLastTickPos.y) * renderPartialTicks),
			static_cast<float>((playerPos.z - 0.4f) - renderPos.z + -playerPos.z + playerLastTickPos.z + (playerPos.z - playerLastTickPos.z) * renderPartialTicks),
			static_cast<float>((playerPos.x + 0.4f) - renderPos.x + -playerPos.x + playerLastTickPos.x + (playerPos.x - playerLastTickPos.x) * renderPartialTicks),
			static_cast<float>((playerPos.y + 1.72f) - renderPos.y + -playerPos.y + playerLastTickPos.y + (playerPos.y - playerLastTickPos.y) * renderPartialTicks),
			static_cast<float>((playerPos.z + 0.4f) - renderPos.z + -playerPos.z + playerLastTickPos.z + (playerPos.z - playerLastTickPos.z) * renderPartialTicks)
		);

		std::vector<Vec3> boxVertices
		{
			{playerBoundingBox.minX, playerBoundingBox.minY, playerBoundingBox.minZ},
			{playerBoundingBox.minX, playerBoundingBox.maxY, playerBoundingBox.minZ},
			{playerBoundingBox.maxX, playerBoundingBox.maxY, playerBoundingBox.minZ},
			{playerBoundingBox.maxX, playerBoundingBox.minY, playerBoundingBox.minZ},
			{playerBoundingBox.minX, playerBoundingBox.minY, playerBoundingBox.maxZ},
			{playerBoundingBox.minX, playerBoundingBox.maxY, playerBoundingBox.maxZ},
			{playerBoundingBox.maxX, playerBoundingBox.maxY, playerBoundingBox.maxZ},
			{playerBoundingBox.maxX, playerBoundingBox.minY, playerBoundingBox.maxZ},
		};

		tmpHitboxes.push_back(playerBoundingBox);
		tmpCoords.emplace_back(std::make_pair(Entity_t(pl->GetName(env, true), localPlayerPos.distance(playerPos), pl->GetHealth(env)), boxVertices));
	}

	hitboxToDraw = tmpHitboxes;
	coordsToDraw = tmpCoords;
}

void ESP::OnRender(JNIEnv* env)
{
	const auto espSettings = Communication::GetSettings();
	if (!espSettings->Visuals.ESP.m_Enabled || espSettings->Visuals.ESP.m_Mode == 0 || !espSettings->Visuals.ESP.m_Boxes)
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
	auto drawFilledBox = [](AxisAlignedBB_t boundingBox)
	{
		glBegin(7);
		glVertex3f(boundingBox.minX, boundingBox.minY, boundingBox.minZ);
		glVertex3f(boundingBox.minX, boundingBox.maxY, boundingBox.minZ);
		glVertex3f(boundingBox.minX, boundingBox.minY, boundingBox.minZ);
		glVertex3f(boundingBox.maxX, boundingBox.maxY, boundingBox.minZ);
		glVertex3f(boundingBox.maxX, boundingBox.minY, boundingBox.maxZ);
		glVertex3f(boundingBox.maxX, boundingBox.maxY, boundingBox.maxZ);
		glVertex3f(boundingBox.minX, boundingBox.minY, boundingBox.maxZ);
		glVertex3f(boundingBox.minX, boundingBox.maxY, boundingBox.maxZ);
		glEnd();
		glBegin(7);
		glVertex3f(boundingBox.maxX, boundingBox.maxY, boundingBox.minZ);
		glVertex3f(boundingBox.maxX, boundingBox.minY, boundingBox.minZ);
		glVertex3f(boundingBox.minX, boundingBox.maxY, boundingBox.minZ);
		glVertex3f(boundingBox.minX, boundingBox.minY, boundingBox.minZ);
		glVertex3f(boundingBox.minX, boundingBox.maxY, boundingBox.maxZ);
		glVertex3f(boundingBox.minX, boundingBox.minY, boundingBox.maxZ);
		glVertex3f(boundingBox.maxX, boundingBox.maxY, boundingBox.maxZ);
		glVertex3f(boundingBox.maxX, boundingBox.minY, boundingBox.maxZ);
		glEnd();
		glBegin(7);
		glVertex3f(boundingBox.minX, boundingBox.minY, boundingBox.minZ);
		glVertex3f(boundingBox.minX, boundingBox.maxY, boundingBox.minZ);
		glVertex3f(boundingBox.maxX, boundingBox.minY, boundingBox.minZ);
		glVertex3f(boundingBox.maxX, boundingBox.maxY, boundingBox.minZ);
		glEnd();
		glBegin(7);
		glVertex3f(boundingBox.minX, boundingBox.maxY, boundingBox.minZ);
		glVertex3f(boundingBox.maxX, boundingBox.maxY, boundingBox.minZ);
		glVertex3f(boundingBox.maxX, boundingBox.maxY, boundingBox.maxZ);
		glVertex3f(boundingBox.minX, boundingBox.maxY, boundingBox.maxZ);
		glVertex3f(boundingBox.minX, boundingBox.maxY, boundingBox.minZ);
		glVertex3f(boundingBox.minX, boundingBox.maxY, boundingBox.maxZ);
		glVertex3f(boundingBox.maxX, boundingBox.maxY, boundingBox.maxZ);
		glVertex3f(boundingBox.maxX, boundingBox.maxY, boundingBox.minZ);
		glEnd();
		glBegin(7);
		glVertex3f(boundingBox.minX, boundingBox.minY, boundingBox.minZ);
		glVertex3f(boundingBox.maxX, boundingBox.minY, boundingBox.minZ);
		glVertex3f(boundingBox.maxX, boundingBox.minY, boundingBox.maxZ);
		glVertex3f(boundingBox.minX, boundingBox.minY, boundingBox.maxZ);
		glVertex3f(boundingBox.minX, boundingBox.minY, boundingBox.minZ);
		glVertex3f(boundingBox.minX, boundingBox.minY, boundingBox.maxZ);
		glVertex3f(boundingBox.maxX, boundingBox.minY, boundingBox.maxZ);
		glVertex3f(boundingBox.maxX, boundingBox.minY, boundingBox.minZ);
		glEnd();
		glBegin(7);
		glVertex3f(boundingBox.minX, boundingBox.minY, boundingBox.minZ);
		glVertex3f(boundingBox.minX, boundingBox.maxY, boundingBox.minZ);
		glVertex3f(boundingBox.minX, boundingBox.minY, boundingBox.maxZ);
		glVertex3f(boundingBox.minX, boundingBox.maxY, boundingBox.maxZ);
		glVertex3f(boundingBox.maxX, boundingBox.minY, boundingBox.maxZ);
		glVertex3f(boundingBox.maxX, boundingBox.maxY, boundingBox.maxZ);
		glVertex3f(boundingBox.maxX, boundingBox.minY, boundingBox.minZ);
		glVertex3f(boundingBox.maxX, boundingBox.maxY, boundingBox.minZ);
		glEnd();
		glBegin(7);
		glVertex3f(boundingBox.minX, boundingBox.maxY, boundingBox.maxZ);
		glVertex3f(boundingBox.minX, boundingBox.minY, boundingBox.maxZ);
		glVertex3f(boundingBox.minX, boundingBox.maxY, boundingBox.minZ);
		glVertex3f(boundingBox.minX, boundingBox.minY, boundingBox.minZ);
		glVertex3f(boundingBox.maxX, boundingBox.maxY, boundingBox.minZ);
		glVertex3f(boundingBox.maxX, boundingBox.minY, boundingBox.minZ);
		glVertex3f(boundingBox.maxX, boundingBox.maxY, boundingBox.maxZ);
		glVertex3f(boundingBox.maxX, boundingBox.minY, boundingBox.maxZ);
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
		
		for (const auto& bb : hitboxToDraw)
		{
			if (espSettings->Visuals.ESP.m_Mode == 1) {
				glColor4f(espSettings->Visuals.ESP.m_OutlineColor[0], espSettings->Visuals.ESP.m_OutlineColor[1], espSettings->Visuals.ESP.m_OutlineColor[2], 1.f);
				drawOutlinedBox(bb);
			}
			else if (espSettings->Visuals.ESP.m_Mode == 2) {
				glColor4f(espSettings->Visuals.ESP.m_FilledColor[0], espSettings->Visuals.ESP.m_FilledColor[1], espSettings->Visuals.ESP.m_FilledColor[2], 0.15f);
				drawFilledBox(bb);

				glColor4f(espSettings->Visuals.ESP.m_OutlineColor[0], espSettings->Visuals.ESP.m_OutlineColor[1], espSettings->Visuals.ESP.m_OutlineColor[2], 1.f);
				drawOutlinedBox(bb);
			}
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

void ESP::OnImGuiRender(JNIEnv* env)
{
	const auto espSettings = Communication::GetSettings();
	if ((!espSettings->Visuals.ESP.m_Enabled || espSettings->Visuals.ESP.m_Mode > 0) && !espSettings->Visuals.Tracers.m_Enabled)
		return;

	const auto DrawFilledBox = [](const ImVec2& start, const ImVec2& end, const ImColor& color) {
		ImGui::GetWindowDrawList()->AddRectFilled(start, ImVec2(start.x + end.x, start.y + end.y), color);
	};

	const auto DrawBox = [](const ImVec2& start, const ImVec2& end, const ImU32& color)
	{
		ImGui::GetBackgroundDrawList()->AddRect(start, end, ImColor(0, 0, 0, 255), 0, 0, 4.f);
		ImGui::GetBackgroundDrawList()->AddRect(start, end, color, 0, 0, 1.f);
	};

	const auto DrawLine = [](const ImVec2& start, const ImVec2& end, const ImVec4& color, const float& thickness) {
		ImGui::GetWindowDrawList()->AddLine(start, end, ImGui::GetColorU32(color), thickness);
	};

	const auto DrawText = [](float fontSize, ImVec2 pos, const char* str, ImU32 outline)
	{
		ImGui::GetWindowDrawList()->AddText(ImGui::GetIO().Fonts->Fonts[0], fontSize, ImVec2(pos.x + 1, pos.y + 1), outline, str);
		ImGui::GetWindowDrawList()->AddText(ImGui::GetIO().Fonts->Fonts[0], fontSize, ImVec2(pos.x - 1, pos.y - 1), outline, str);
		ImGui::GetWindowDrawList()->AddText(ImGui::GetIO().Fonts->Fonts[0], fontSize, ImVec2(pos.x + 1, pos.y - 1), outline, str);
		ImGui::GetWindowDrawList()->AddText(ImGui::GetIO().Fonts->Fonts[0], fontSize, ImVec2(pos.x - 1, pos.y + 1), outline, str);
		ImGui::GetWindowDrawList()->AddText(ImGui::GetIO().Fonts->Fonts[0], fontSize, ImVec2(pos.x + 1, pos.y), outline, str);
		ImGui::GetWindowDrawList()->AddText(ImGui::GetIO().Fonts->Fonts[0], fontSize, ImVec2(pos.x - 1, pos.y), outline, str);
		ImGui::GetWindowDrawList()->AddText(ImGui::GetIO().Fonts->Fonts[0], fontSize, ImVec2(pos.x, pos.y - 1), outline, str);
		ImGui::GetWindowDrawList()->AddText(ImGui::GetIO().Fonts->Fonts[0], fontSize, ImVec2(pos.x, pos.y + 1), outline, str);
		ImGui::GetWindowDrawList()->AddText(ImGui::GetIO().Fonts->Fonts[0], fontSize, ImVec2(pos.x, pos.y), IM_COL32_WHITE, str);
	};

	if (Minecraft::GetTheWorld(env) && !Minecraft::GetCurrentScreen(env))
	{
		auto modelView = ActiveRenderInfo::GetModelView(env);
		auto projection = ActiveRenderInfo::GetProjection(env);
		for (const auto& et : coordsToDraw)
		{
			auto worldCoords = et.second;
			const auto health = et.first.health < 0 ? 0.f : min(20.f, et.first.health);
			auto dist = et.first.distToLocal;

			Vec4 screenBoundingBox = Vec4(FLT_MAX, FLT_MAX, -1.f, -1.f);
			if (worldCoords.size() != 8)
				continue;

			for (int i = 0; i < 8; i++)
			{
				Vec2 screenPos;
				if (!WorldToScreen(Vec3(worldCoords[i].x, worldCoords[i].y, worldCoords[i].z), screenPos, modelView, projection, static_cast<int>(ImGui::GetIO().DisplaySize.x), static_cast<int>(ImGui::GetIO().DisplaySize.y)))
					continue;

				screenBoundingBox.x = fminf(screenPos.x, screenBoundingBox.x);
				screenBoundingBox.y = fminf(screenPos.y, screenBoundingBox.y);
				screenBoundingBox.z = fmaxf(screenPos.x, screenBoundingBox.z);
				screenBoundingBox.w = fmaxf(screenPos.y, screenBoundingBox.w);
			}

			if (screenBoundingBox.x > 0 || screenBoundingBox.y > 0 || screenBoundingBox.z <= ImGui::GetIO().DisplaySize.x || screenBoundingBox.w <= ImGui::GetIO().DisplaySize.y)
			{
				if (espSettings->Visuals.ESP.m_Enabled && espSettings->Visuals.ESP.m_Mode == 0 && espSettings->Visuals.ESP.m_Boxes) {
					DrawBox(ImVec2(screenBoundingBox.x, screenBoundingBox.y), ImVec2(screenBoundingBox.z, screenBoundingBox.w), ImGui::GetColorU32(ImVec4(espSettings->Visuals.ESP.m_OutlineColor[0], espSettings->Visuals.ESP.m_OutlineColor[1], espSettings->Visuals.ESP.m_OutlineColor[2], 1.f)));
					DrawFilledBox(ImVec2(screenBoundingBox.x, screenBoundingBox.y), ImVec2(abs(screenBoundingBox.z - screenBoundingBox.x), abs(screenBoundingBox.w - screenBoundingBox.y)), ImVec4(espSettings->Visuals.ESP.m_FilledColor[0], espSettings->Visuals.ESP.m_FilledColor[1], espSettings->Visuals.ESP.m_FilledColor[2], 0.f));
				
					if (espSettings->Visuals.ESP.m_Healthbar) {
						DrawFilledBox({ screenBoundingBox.x - 6.f, screenBoundingBox.y }, { 4.f, abs(screenBoundingBox.w - screenBoundingBox.y) }, ImVec4(0.f, 0.f, 0.f, 200 / 255.f));
						DrawFilledBox({ screenBoundingBox.x - 5.f, screenBoundingBox.y + 1.f }, { 2.f, (abs(screenBoundingBox.w - screenBoundingBox.y) - 2.f) * (health / 20.f)}, ImVec4(0.f, 200.f / 255.f, 0.f, 1.f));
					}
				}

				if (espSettings->Visuals.ESP.m_Names)
				{
					const ImVec2 textSize = ImGui::CalcTextSize(et.first.name.data());

					const float width = screenBoundingBox.z - screenBoundingBox.x;
					const ImVec2 boxMin = ImVec2((screenBoundingBox.x + (width / 2)) - (textSize.x / 2) - (textSize.x * 0.1f), screenBoundingBox.y - 5 - (textSize.y / 2));
					const ImVec2 boxMax = ImVec2((screenBoundingBox.x + (width / 2)) + (textSize.x / 2) + (textSize.x * 0.1f), screenBoundingBox.y - 5 + (textSize.y / 2));
					ImGui::GetBackgroundDrawList()->AddRectFilled(
						boxMin,
						boxMax,
						ImColor(0, 0, 0, 150),
						2.5f
					);

					const float textX = ((screenBoundingBox.x + (width / 2)) - (textSize.x / 2));
					const float textY = (screenBoundingBox.y - 5 - (textSize.y / 2));
					ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), ImGui::GetFont()->FontSize, ImVec2(textX, textY), ImGui::GetColorU32(ImVec4(1.f, 1.f, 1.f, 1.f)), et.first.name.data());
				}
				
				if (espSettings->Visuals.Tracers.m_Enabled)
				{
					auto xPos = static_cast<float>(screenBoundingBox.x + (abs(screenBoundingBox.z - screenBoundingBox.x) / 2));
					auto yPos = static_cast<float>(screenBoundingBox.y + (abs(screenBoundingBox.w - screenBoundingBox.y) / 2));

					if (xPos > 0 && yPos > 0 && xPos <= ImGui::GetIO().DisplaySize.x && yPos <= ImGui::GetIO().DisplaySize.y)
					{
						DrawLine({ ImGui::GetIO().DisplaySize.x / 2,  ImGui::GetIO().DisplaySize.y / 2 }, { xPos , yPos }, ImVec4(espSettings->Visuals.Tracers.m_Color[0], espSettings->Visuals.Tracers.m_Color[1], espSettings->Visuals.Tracers.m_Color[2], 1.f), espSettings->Visuals.Tracers.thickness);
					}
				}

			}
		}
	}
}