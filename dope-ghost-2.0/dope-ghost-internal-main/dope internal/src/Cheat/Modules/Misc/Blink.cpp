#include "pch.h"
#include "Blink.h"

#include "../Settings.h"
#include "../../../Helper/Communication.h"

#include "../../../Game/Classes/Minecraft.h"
#include "../../../Game/Classes/Player.h"
#include "../../../Game/Classes/World.h"
#include "../../../Game/Classes/ActiveRenderInfo.h"
#include "../../../Game/Classes/RenderManager.h"
#include "../../../Game/Classes/Timer.h"

#include "../../../Helper/Utils.h"

#include "../../../../vendors/imgui/imgui.h"

#include <gl/GL.h>

static std::vector<Vec3D> pathCoords;
static std::vector<std::pair<Vec3D, Vec3D>> tmpCoords;
static ULONGLONG timer;
static bool wasActive;
void Blink::Run(JNIEnv* env)
{
	const auto blinkSettings = Communication::GetSettings()->Utilities.Blink;
	if (!blinkSettings.m_Enabled || !blinkSettings.m_ShowPath) {
		Sleep(5);
		return;
	}

	const bool isBindPress = GetAsyncKeyState(blinkSettings.m_Bind) & 0x8000;

	if (!isBindPress || (timer != NULL && GetTickCount64() - timer > blinkSettings.m_TimerLimit * 1000))
	{
		tmpCoords.clear();
		pathCoords.clear();
		timer = NULL;
	}

	if (!isBindPress)
		wasActive = false;

	if (timer == 0 && !wasActive && isBindPress) {
		timer = GetTickCount64();
		wasActive = true;
	}

	if (timer == NULL)
		return;

	if (Minecraft::GetTheWorld(env) && !Minecraft::GetCurrentScreen(env))
	{
		const auto renderManager = (RenderManager*)Minecraft::GetRenderManager(env);
		const auto thePlayer = (Player*)Minecraft::GetThePlayer(env);
		auto pos = thePlayer->GetPos(env);
		auto playerLastTickPos = thePlayer->GetLastTickPos(env);

		if (thePlayer->GetPreviousPos(env).distance(pos) > 0.1f)
		{
			tmpCoords.push_back(std::make_pair(playerLastTickPos, pos));

			std::vector<Vec3D> tmpInterpolatedCoords;
			for (const auto& wlrdCo : tmpCoords) {
				const auto wlrdCoords = wlrdCo.second;
				const auto playerLastTickPos = wlrdCo.first;

				const auto renderPos = renderManager->GetRenderPos(env);
				const auto renderPartialTicks = ((Timer*)Minecraft::GetTimer(env))->GetRenderPartialTicks(env);
				auto newPos = Vec3D{
					wlrdCoords.x - renderPos.x + -wlrdCoords.x + playerLastTickPos.x + (wlrdCoords.x - playerLastTickPos.x) * renderPartialTicks,
					wlrdCoords.y - renderPos.y + -wlrdCoords.y + playerLastTickPos.y + (wlrdCoords.y - playerLastTickPos.y) * renderPartialTicks,
					wlrdCoords.z - renderPos.z + -wlrdCoords.z + playerLastTickPos.z + (wlrdCoords.z - playerLastTickPos.z) * renderPartialTicks,
				};

				tmpInterpolatedCoords.push_back(newPos);
			}
			pathCoords = tmpInterpolatedCoords;
		}
	}
}

void Blink::OnRender(JNIEnv* env)
{
	const auto blinkSettings = Communication::GetSettings()->Utilities.Blink;
	if (!blinkSettings.m_Enabled || !blinkSettings.m_ShowPath || GetTickCount64() - timer > blinkSettings.m_TimerLimit * 1000)
		return;

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

	/**/
	glBegin(GL_LINE_STRIP);
	for (const auto& pos : pathCoords) {
		glColor3f(blinkSettings.m_MarkerColor[0], blinkSettings.m_MarkerColor[1], blinkSettings.m_MarkerColor[2]);
		glVertex3d(pos.x, pos.y, pos.z);
	}
	glEnd();
	/**/

	glDisable(GL_BLEND);
	glDepthMask(true);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_LINE_SMOOTH);
	glPopMatrix();

	glPopMatrix();
}

void Blink::OnImGuiRender(JNIEnv* env)
{
	const auto blinkSettings = Communication::GetSettings()->Utilities.Blink;
	if (!blinkSettings.m_Enabled || !blinkSettings.m_ShowTimer || timer == NULL || GetTickCount64() - timer > blinkSettings.m_TimerLimit * 1000)
		return;

	ImGui::SetNextWindowPos(ImVec2{ +1, -1 });
	{
		ImGui::BeginChild("##b", ImVec2(ImGui::GetIO().DisplaySize.x, 150.f), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
		{
			static ULONGLONG oldStage{ timer };
			static float fraction{ 0.0f };
			static float time{ 0.0f };
			static float newFraction{ timer / (blinkSettings.m_TimerLimit * 1000.f) };
			static bool changing{ false };

			const auto curStage = GetTickCount64() - timer;

			if (oldStage != curStage)
			{
				oldStage = curStage;
				time = 0.f;
				newFraction = curStage / (blinkSettings.m_TimerLimit * 1000.f);
				changing = true;
			}

			if (changing && time < 1.f)
			{
				time += 0.005f;
				fraction += (newFraction - fraction) * /* easeOutExpo */ (1.f - powf(1.f - time, 3.f));
			}
			else if (time >= 1.f)
			{
				changing = false;
				time = 0.f;
			}

			ImGui::SetCursorPos({ ImGui::GetIO().DisplaySize.x / 2 - (500.f / 2), 25.f });
			ImGui::ProgressBar(fraction, { 500.f, 25.f }, "");
		}
		ImGui::EndChild();

	}
}

void Blink::OnReceiveData()
{
	const auto blinkSettings = Communication::GetSettings()->Utilities.Blink;
	if (!blinkSettings.m_Enabled)
		return;

	while (GetAsyncKeyState(blinkSettings.m_Bind) & 0x8000 && (GetTickCount64() - timer < blinkSettings.m_TimerLimit * 1000)) {
		Sleep(1);
	}
}