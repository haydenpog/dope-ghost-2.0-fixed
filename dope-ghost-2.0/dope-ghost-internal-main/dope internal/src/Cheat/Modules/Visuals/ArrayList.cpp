#include "pch.h"
#include "ArrayList.h"

#include "../Settings.h"
#include "../../../Helper/Communication.h"

#include "../../../Helper/Utils.h"

#include "../../../../vendors/imgui/imgui.h"

static std::once_flag texturesInitialized;
static std::vector<std::pair<const char*, bool*>> modulesArray;
void ArrayList::OnImGuiRender(JNIEnv* env)
{
	std::call_once(texturesInitialized, []() {
		modulesArray.emplace_back(std::make_pair("Player ESP", &Communication::GetSettings()->Visuals.ESP.m_Enabled));
		modulesArray.emplace_back(std::make_pair("Tracers", &Communication::GetSettings()->Visuals.Tracers.m_Enabled));
		modulesArray.emplace_back(std::make_pair("Velocity", &Communication::GetSettings()->Combat.Velocity.m_Enabled));
		modulesArray.emplace_back(std::make_pair("Reach", &Communication::GetSettings()->Combat.Reach.m_Enabled));
		modulesArray.emplace_back(std::make_pair("Blink", &Communication::GetSettings()->Utilities.Blink.m_Enabled));
		modulesArray.emplace_back(std::make_pair("Aim Assist", &Communication::GetSettings()->Combat.AimAssist.m_Enabled));
		modulesArray.emplace_back(std::make_pair("Left Clicker", &Communication::GetSettings()->Combat.LeftClicker.m_Enabled));
		modulesArray.emplace_back(std::make_pair("Right Clicker", &Communication::GetSettings()->Combat.RightClicker.m_Enabled));
		modulesArray.emplace_back(std::make_pair("Throw Pearl", &Communication::GetSettings()->Utilities.ThrowPearl.m_Enabled));
		modulesArray.emplace_back(std::make_pair("Throw Pots", &Communication::GetSettings()->Utilities.ThrowPot.m_Enabled));
		modulesArray.emplace_back(std::make_pair("Bhop", &Communication::GetSettings()->Movement.Bhop.m_Enabled));
		modulesArray.emplace_back(std::make_pair("Timer", &Communication::GetSettings()->Movement.TimerSpeed.m_Enabled));
		modulesArray.emplace_back(std::make_pair("Bridge Assist", &Communication::GetSettings()->Utilities.BridgeAssist.m_Enabled));
		modulesArray.emplace_back(std::make_pair("Block ESP", &Communication::GetSettings()->Visuals.BlockESP.m_Enabled));

		std::ranges::sort(modulesArray.begin(), modulesArray.end(), [](const std::pair<const char*, bool*>& first, const std::pair<const char*, bool*>& second)
		{
			return ImGui::CalcTextSize(first.first).x > ImGui::CalcTextSize(second.first).x;
		});
	});

	const auto arrayListSettings = Communication::GetSettings()->Visuals.ArrayList;
	if (!arrayListSettings.m_Enabled)
		return;

	ImGui::SetNextWindowPos(ImVec2{ +1, -1 });
	{
		auto Rainbow = [](int offset, float saturation)
		{
			const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

			const auto state = ceil((millis.count() + offset) / 20);
			const auto rainBow = FromHSB((fmod(state, 360) / 360.0f), saturation, 0.8f);
			return ImColor((int)rainBow.x, (int)rainBow.y, (int)rainBow.z);
		};

		int idx = 0;
		auto DrawModule = [&Rainbow, &idx, &arrayListSettings](const char* str, bool rainbow = false)
		{
			ImVec2 txtSize = ImGui::CalcTextSize(str);
			const auto col = arrayListSettings.m_Rainbow ? ImVec4(Rainbow(idx * 200, arrayListSettings.m_RainbowSaturation)) : ImVec4(arrayListSettings.m_Color[0], arrayListSettings.m_Color[1], arrayListSettings.m_Color[2], 1.f);

			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(ImColor(19, 19, 19, arrayListSettings.m_Background ? 150 : 0)));
			ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - (txtSize.x + 10.f), -1 + (idx * txtSize.y)), ImGuiCond_Always);
			ImGui::BeginChild(std::to_string(idx).data(), { txtSize.x + (arrayListSettings.m_ColorBar ? 25.f : 22.f), txtSize.y }, false);
			{
				const ImVec2 cursor = ImGui::GetCursorPos();
				ImGui::SetCursorPos({ cursor.x + (arrayListSettings.m_ColorBar ? 5.f : 8.f), cursor.y - 1.f });

				ImGui::PushStyleColor(ImGuiCol_Text, col);

				ImGui::TextUnformatted(str);

				ImGui::PopStyleColor(1);
				ImGui::SetCursorPos(cursor);
			}
			ImGui::EndChild();
			ImGui::PopStyleColor(1);

			if (arrayListSettings.m_ColorBar) {
				ImGui::PushStyleColor(ImGuiCol_ChildBg, col);
				ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 3.f, -1 + (idx * txtSize.y)), ImGuiCond_Always);
				ImGui::BeginChild(std::string("##" + std::to_string(idx) + "r").data(), { 3.f, txtSize.y }, false);
				ImGui::EndChild();
				ImGui::PopStyleColor(1);
			}
			idx++;
		};

		for (const auto& mod: modulesArray)
		{
			if (*mod.second)
				DrawModule(mod.first);
		}
	}
}