#include "pch.h"
#include "UserConfigs.h"
#include "Settings.h"
#include <iostream>
#include <fstream>

#include "../../vendors/json.h"

std::string UserConfigs::CreateData(std::string name)
{
	nlohmann::json j;

#pragma region json_struct
	j["name"] = name.data();
	j["id"] = "-1";

	/**/
	j["struct"]["combat"]["velocity"]["enabled"] = Instance<Settings>::Get()->Combat.Velocity.m_Enabled;
	j["struct"]["combat"]["velocity"]["bind"] = Instance<Settings>::Get()->Combat.Velocity.m_Bind;
	j["struct"]["combat"]["velocity"]["horizontal"] = Instance<Settings>::Get()->Combat.Velocity.m_Horizontal;
	j["struct"]["combat"]["velocity"]["vertical"] = Instance<Settings>::Get()->Combat.Velocity.m_Vertical;
	j["struct"]["combat"]["velocity"]["chance"] = Instance<Settings>::Get()->Combat.Velocity.m_Chance;
	j["struct"]["combat"]["velocity"]["delay"] = Instance<Settings>::Get()->Combat.Velocity.m_Delay;
	j["struct"]["combat"]["velocity"]["onlyweapon"] = Instance<Settings>::Get()->Combat.Velocity.m_WeaponOnly;
	j["struct"]["combat"]["velocity"]["aironly"] = Instance<Settings>::Get()->Combat.Velocity.m_AirOnly;
	j["struct"]["combat"]["velocity"]["moving"] = Instance<Settings>::Get()->Combat.Velocity.m_Moving;

	j["struct"]["combat"]["leftclicker"]["enabled"] = Instance<Settings>::Get()->Combat.LeftClicker.m_Enabled;
	j["struct"]["combat"]["leftclicker"]["bind"] = Instance<Settings>::Get()->Combat.LeftClicker.m_Bind;
	j["struct"]["combat"]["leftclicker"]["avg"] = Instance<Settings>::Get()->Combat.LeftClicker.m_Average;
	j["struct"]["combat"]["leftclicker"]["breakBlock"] = Instance<Settings>::Get()->Combat.LeftClicker.m_BreakBlock;
	j["struct"]["combat"]["leftclicker"]["onlyWeapon"] = Instance<Settings>::Get()->Combat.LeftClicker.m_OnlyWeapon;
	j["struct"]["combat"]["leftclicker"]["inInventory"] = Instance<Settings>::Get()->Combat.LeftClicker.m_InInventory;
	j["struct"]["combat"]["leftclicker"]["smartblockhit"] = Instance<Settings>::Get()->Combat.LeftClicker.m_SmartBlockHit;

	j["struct"]["combat"]["aimassist"]["enabled"] = Instance<Settings>::Get()->Combat.AimAssist.m_Enabled;
	j["struct"]["combat"]["aimassist"]["bind"] = Instance<Settings>::Get()->Combat.AimAssist.m_Bind;
	j["struct"]["combat"]["aimassist"]["distance"] = Instance<Settings>::Get()->Combat.AimAssist.m_Distance;
	j["struct"]["combat"]["aimassist"]["fov"] = Instance<Settings>::Get()->Combat.AimAssist.m_Fov;
	j["struct"]["combat"]["aimassist"]["speed"] = Instance<Settings>::Get()->Combat.AimAssist.m_Speed;
	j["struct"]["combat"]["aimassist"]["onlyweapon"] = Instance<Settings>::Get()->Combat.AimAssist.m_OnlyWeapon;
	j["struct"]["combat"]["aimassist"]["throughWall"] = Instance<Settings>::Get()->Combat.AimAssist.m_ThroughWall;
	j["struct"]["combat"]["aimassist"]["onlyclicking"] = Instance<Settings>::Get()->Combat.AimAssist.m_ClickingOnly;
	j["struct"]["combat"]["aimassist"]["mode"]["mode_selected"] = Instance<Settings>::Get()->Combat.AimAssist.Mode.m_Mode;

	j["struct"]["combat"]["reach"]["enabled"] = Instance<Settings>::Get()->Combat.Reach.m_Enabled;
	j["struct"]["combat"]["reach"]["bind"] = Instance<Settings>::Get()->Combat.Reach.m_Bind;
	j["struct"]["combat"]["reach"]["value"] = Instance<Settings>::Get()->Combat.Reach.m_Distance;
	j["struct"]["combat"]["reach"]["hitbox"] = Instance<Settings>::Get()->Combat.Reach.m_Hitbox;
	j["struct"]["combat"]["reach"]["onlyweapon"] = Instance<Settings>::Get()->Combat.Reach.m_OnlyWeapon;
	j["struct"]["combat"]["reach"]["onground"] = Instance<Settings>::Get()->Combat.Reach.m_OnGround;
	j["struct"]["combat"]["reach"]["liquidcheck"] = Instance<Settings>::Get()->Combat.Reach.m_LiquidCheck;
	j["struct"]["combat"]["reach"]["combomode"] = Instance<Settings>::Get()->Combat.Reach.m_ComboMode;

	/**/
	j["struct"]["movement"]["timer"]["enabled"] = Instance<Settings>::Get()->Movement.TimerSpeed.m_Enabled;
	j["struct"]["movement"]["timer"]["bind"] = Instance<Settings>::Get()->Movement.TimerSpeed.m_Bind;
	j["struct"]["movement"]["timer"]["value"] = Instance<Settings>::Get()->Movement.TimerSpeed.m_Speed;
	j["struct"]["movement"]["timer"]["moving"] = Instance<Settings>::Get()->Movement.TimerSpeed.m_Moving;
	j["struct"]["movement"]["timer"]["onlyweapon"] = Instance<Settings>::Get()->Movement.TimerSpeed.m_OnlyWeapon;

	j["struct"]["movement"]["bhop"]["enabled"] = Instance<Settings>::Get()->Movement.Bhop.m_Enabled;
	j["struct"]["movement"]["bhop"]["bind"] = Instance<Settings>::Get()->Movement.Bhop.m_Bind;
	j["struct"]["movement"]["bhop"]["power"] = Instance<Settings>::Get()->Movement.Bhop.m_Power;
	j["struct"]["movement"]["bhop"]["liquidCheck"] = Instance<Settings>::Get()->Movement.Bhop.m_LiquidCheck;

	/**/
	j["struct"]["utilities"]["throwpot"]["enabled"] = Instance<Settings>::Get()->Utilities.ThrowPot.m_Enabled;
	j["struct"]["utilities"]["throwpot"]["bind"] = Instance<Settings>::Get()->Utilities.ThrowPot.m_Bind;
	j["struct"]["utilities"]["throwpot"]["switchDelay"] = Instance<Settings>::Get()->Utilities.ThrowPot.m_SwitchDelay;
	j["struct"]["utilities"]["throwpot"]["throwDelay"] = Instance<Settings>::Get()->Utilities.ThrowPot.m_ThrowDelay;
	j["struct"]["utilities"]["throwpot"]["mode"]["mode_selected"] = Instance<Settings>::Get()->Utilities.ThrowPot.Mode.m_Mode;

	j["struct"]["utilities"]["throwpearl"]["enabled"] = Instance<Settings>::Get()->Utilities.ThrowPearl.m_Enabled;
	j["struct"]["utilities"]["throwpearl"]["bind"] = Instance<Settings>::Get()->Utilities.ThrowPearl.m_Bind;
	j["struct"]["utilities"]["throwpearl"]["switchDelay"] = Instance<Settings>::Get()->Utilities.ThrowPearl.m_SwitchDelay;
	j["struct"]["utilities"]["throwpearl"]["throwDelay"] = Instance<Settings>::Get()->Utilities.ThrowPearl.m_ThrowDelay;

	j["struct"]["utilities"]["blink"]["enabled"] = Instance<Settings>::Get()->Utilities.Blink.m_Enabled;
	j["struct"]["utilities"]["blink"]["bind"] = Instance<Settings>::Get()->Utilities.Blink.m_Bind;
	j["struct"]["utilities"]["blink"]["showtimer"] = Instance<Settings>::Get()->Utilities.Blink.m_ShowTimer;
	j["struct"]["utilities"]["blink"]["timerlimit"] = Instance<Settings>::Get()->Utilities.Blink.m_TimerLimit;
	j["struct"]["utilities"]["blink"]["showpath"] = Instance<Settings>::Get()->Utilities.Blink.m_ShowPath;
	j["struct"]["utilities"]["blink"]["markercolor"] = Instance<Settings>::Get()->Utilities.Blink.m_MarkerColor;

	j["struct"]["utilities"]["bridgeassist"]["enabled"] = Instance<Settings>::Get()->Utilities.BridgeAssist.m_Enabled;
	j["struct"]["utilities"]["bridgeassist"]["bind"] = Instance<Settings>::Get()->Utilities.BridgeAssist.m_Bind;
	j["struct"]["utilities"]["bridgeassist"]["switchDelay"] = Instance<Settings>::Get()->Utilities.BridgeAssist.m_OnlyRightClick;

	/**/
	j["struct"]["visuals"]["esp"]["enabled"] = Instance<Settings>::Get()->Visuals.ESP.m_Enabled;
	j["struct"]["visuals"]["esp"]["bind"] = Instance<Settings>::Get()->Visuals.ESP.m_Bind;
	j["struct"]["visuals"]["esp"]["boxes"] = Instance<Settings>::Get()->Visuals.ESP.m_Boxes;
	j["struct"]["visuals"]["esp"]["mode"] = Instance<Settings>::Get()->Visuals.ESP.m_Mode;
	j["struct"]["visuals"]["esp"]["healthbar"] = Instance<Settings>::Get()->Visuals.ESP.m_Healthbar;
	j["struct"]["visuals"]["esp"]["names"] = Instance<Settings>::Get()->Visuals.ESP.m_Names;
	j["struct"]["visuals"]["esp"]["chams"] = Instance<Settings>::Get()->Visuals.ESP.m_Chams;
	j["struct"]["visuals"]["esp"]["filledcolor"] = Instance<Settings>::Get()->Visuals.ESP.m_FilledColor;
	j["struct"]["visuals"]["esp"]["outlinecolor"] = Instance<Settings>::Get()->Visuals.ESP.m_OutlineColor;

	j["struct"]["visuals"]["tracers"]["enabled"] = Instance<Settings>::Get()->Visuals.Tracers.m_Enabled;
	j["struct"]["visuals"]["tracers"]["bind"] = Instance<Settings>::Get()->Visuals.Tracers.m_Bind;
	j["struct"]["visuals"]["tracers"]["color"] = Instance<Settings>::Get()->Visuals.Tracers.m_Color;
	j["struct"]["visuals"]["tracers"]["thickness"] = Instance<Settings>::Get()->Visuals.Tracers.thickness;

	j["struct"]["visuals"]["arraylist"]["enabled"] = Instance<Settings>::Get()->Visuals.ArrayList.m_Enabled;
	j["struct"]["visuals"]["arraylist"]["bind"] = Instance<Settings>::Get()->Visuals.ArrayList.m_Bind;
	j["struct"]["visuals"]["arraylist"]["colorbar"] = Instance<Settings>::Get()->Visuals.ArrayList.m_ColorBar;
	j["struct"]["visuals"]["arraylist"]["rainbow"] = Instance<Settings>::Get()->Visuals.ArrayList.m_Rainbow;
	j["struct"]["visuals"]["arraylist"]["background"] = Instance<Settings>::Get()->Visuals.ArrayList.m_Background;
	j["struct"]["visuals"]["arraylist"]["rainbowsaturation"] = Instance<Settings>::Get()->Visuals.ArrayList.m_RainbowSaturation;
	j["struct"]["visuals"]["arraylist"]["color"] = Instance<Settings>::Get()->Visuals.ArrayList.m_Color;

	j["struct"]["visuals"]["blockesp"]["enabled"] = Instance<Settings>::Get()->Visuals.BlockESP.m_Enabled;
	j["struct"]["visuals"]["blockesp"]["bind"] = Instance<Settings>::Get()->Visuals.BlockESP.m_Bind;
	j["struct"]["visuals"]["blockesp"]["radius"] = Instance<Settings>::Get()->Visuals.BlockESP.m_Radius;
	j["struct"]["visuals"]["blockesp"]["delay"] = Instance<Settings>::Get()->Visuals.BlockESP.m_Delay;
	j["struct"]["visuals"]["blockesp"]["blocklist"] = Instance<Settings>::Get()->Visuals.BlockESP.blockList;

#pragma endregion
	return j.dump();
}

void UserConfigs::Create(std::string data)
{
	char* szQuery = new char[data.size() + 126];
	sprintf(szQuery, "action=create&creator_id=%s&data=%s", std::to_string(m_UID).data(), data.data());

	std::ofstream outfile("output.txt");

	if (outfile.is_open()) {
		outfile << szQuery;
		outfile.close();
		std::cout << "Output file created successfully." << std::endl;
	}

	delete[] szQuery;
}

void UserConfigs::Update(std::string data)
{
	char* szQuery = new char[data.size() + 126];
	sprintf(szQuery, "action=update&creator_id=%s&id=%s&data=%s", std::to_string(m_UID).data(), std::to_string(m_CID).data(), data.data());

	std::ofstream outfile("output.txt");

	if (outfile.is_open()) {
		outfile << szQuery;
		outfile.close();
		std::cout << "Output file created successfully." << std::endl;
	}
	delete[] szQuery;
}

void UserConfigs::Delete()
{
}

void UserConfigs::Load(std::vector<std::string>& out)
{
#pragma region json_struct
	std::string askedConfig;
	for (const auto& config : out)
	{
		const nlohmann::json j = nlohmann::json::parse(config);
		if (j["id"] == std::to_string(m_CID))
		{
			askedConfig = config;
		}
	}

	nlohmann::json j = nlohmann::json::parse(askedConfig);

	/**/
	if (!j["struct"]["combat"]["velocity"]["enabled"].is_null())
		Instance<Settings>::Get()->Combat.Velocity.m_Enabled = j["struct"]["combat"]["velocity"]["enabled"].get<bool>();

	if (!j["struct"]["combat"]["velocity"]["bind"].is_null())
		Instance<Settings>::Get()->Combat.Velocity.m_Bind = j["struct"]["combat"]["velocity"]["bind"].get<int>();

	if (!j["struct"]["combat"]["velocity"]["horizontal"].is_null())
		Instance<Settings>::Get()->Combat.Velocity.m_Horizontal = j["struct"]["combat"]["velocity"]["horizontal"].get<double>();

	if (!j["struct"]["combat"]["velocity"]["vertical"].is_null())
		Instance<Settings>::Get()->Combat.Velocity.m_Vertical = j["struct"]["combat"]["velocity"]["vertical"].get<double>();

	if (!j["struct"]["combat"]["velocity"]["chance"].is_null())
		Instance<Settings>::Get()->Combat.Velocity.m_Chance = j["struct"]["combat"]["velocity"]["chance"].get<int>();

	if (!j["struct"]["combat"]["velocity"]["delay"].is_null())
		Instance<Settings>::Get()->Combat.Velocity.m_Delay = j["struct"]["combat"]["velocity"]["delay"].get<int>();

	if (!j["struct"]["combat"]["velocity"]["onlyweapon"].is_null())
		Instance<Settings>::Get()->Combat.Velocity.m_WeaponOnly = j["struct"]["combat"]["velocity"]["onlyweapon"].get<bool>();

	if (!j["struct"]["combat"]["velocity"]["moving"].is_null())
		Instance<Settings>::Get()->Combat.Velocity.m_Moving = j["struct"]["combat"]["velocity"]["moving"].get<bool>();

	/**/
	if (!j["struct"]["combat"]["leftclicker"]["enabled"].is_null())
		Instance<Settings>::Get()->Combat.LeftClicker.m_Enabled = j["struct"]["combat"]["leftclicker"]["enabled"].get<bool>();

	if (!j["struct"]["combat"]["leftclicker"]["bind"].is_null())
		Instance<Settings>::Get()->Combat.LeftClicker.m_Bind = j["struct"]["combat"]["leftclicker"]["bind"].get<int>();

	if (!j["struct"]["combat"]["leftclicker"]["avg"].is_null())
		Instance<Settings>::Get()->Combat.LeftClicker.m_Average = j["struct"]["combat"]["leftclicker"]["avg"].get<int>();

	if (!j["struct"]["combat"]["leftclicker"]["breakBlock"].is_null())
		Instance<Settings>::Get()->Combat.LeftClicker.m_BreakBlock = j["struct"]["combat"]["leftclicker"]["breakBlock"].get<bool>();

	if (!j["struct"]["combat"]["leftclicker"]["onlyWeapon"].is_null())
		Instance<Settings>::Get()->Combat.LeftClicker.m_OnlyWeapon = j["struct"]["combat"]["leftclicker"]["onlyWeapon"].get<bool>();

	if (!j["struct"]["combat"]["leftclicker"]["inInventory"].is_null())
		Instance<Settings>::Get()->Combat.LeftClicker.m_InInventory = j["struct"]["combat"]["leftclicker"]["inInventory"].get<bool>();

	if (!j["struct"]["combat"]["leftclicker"]["smartblockhit"].is_null())
		Instance<Settings>::Get()->Combat.LeftClicker.m_SmartBlockHit = j["struct"]["combat"]["leftclicker"]["smartblockhit"].get<bool>();

	if (!j["struct"]["combat"]["aimassist"]["enabled"].is_null())
		Instance<Settings>::Get()->Combat.AimAssist.m_Enabled = j["struct"]["combat"]["aimassist"]["enabled"].get<bool>();

	if (!j["struct"]["combat"]["aimassist"]["bind"].is_null())
		Instance<Settings>::Get()->Combat.AimAssist.m_Bind = j["struct"]["combat"]["aimassist"]["bind"].get<int>();

	/**/
	if (!j["struct"]["combat"]["aimassist"]["distance"].is_null())
		Instance<Settings>::Get()->Combat.AimAssist.m_Distance = j["struct"]["combat"]["aimassist"]["distance"].get<float>();

	if (!j["struct"]["combat"]["aimassist"]["fov"].is_null())
		Instance<Settings>::Get()->Combat.AimAssist.m_Fov = j["struct"]["combat"]["aimassist"]["fov"].get<int>();

	if (!j["struct"]["combat"]["aimassist"]["speed"].is_null())
		Instance<Settings>::Get()->Combat.AimAssist.m_Speed = j["struct"]["combat"]["aimassist"]["speed"].get<int>();

	if (!j["struct"]["combat"]["aimassist"]["onlyweapon"].is_null())
		Instance<Settings>::Get()->Combat.AimAssist.m_OnlyWeapon = j["struct"]["combat"]["aimassist"]["onlyweapon"].get<bool>();

	if (!j["struct"]["combat"]["aimassist"]["throughWall"].is_null())
		Instance<Settings>::Get()->Combat.AimAssist.m_ThroughWall = j["struct"]["combat"]["aimassist"]["throughWall"].get<bool>();

	if (!j["struct"]["combat"]["aimassist"]["onlyclicking"].is_null())
		Instance<Settings>::Get()->Combat.AimAssist.m_ClickingOnly = j["struct"]["combat"]["aimassist"]["onlyclicking"].get<bool>();

	if (!j["struct"]["combat"]["aimassist"]["mode"]["mode_selected"].is_null())
		Instance<Settings>::Get()->Combat.AimAssist.Mode.m_Mode = j["struct"]["combat"]["aimassist"]["mode"]["mode_selected"].get<int>();

	/**/
	if (!j["struct"]["combat"]["reach"]["enabled"].is_null())
		Instance<Settings>::Get()->Combat.Reach.m_Enabled = j["struct"]["combat"]["reach"]["enabled"].get<bool>();

	if (!j["struct"]["combat"]["reach"]["bind"].is_null())
		Instance<Settings>::Get()->Combat.Reach.m_Bind = j["struct"]["combat"]["reach"]["bind"].get<int>();

	if (!j["struct"]["combat"]["reach"]["value"].is_null())
		Instance<Settings>::Get()->Combat.Reach.m_Distance = j["struct"]["combat"]["reach"]["value"].get<double>();

	if (!j["struct"]["combat"]["reach"]["hitbox"].is_null())
		Instance<Settings>::Get()->Combat.Reach.m_Hitbox = j["struct"]["combat"]["reach"]["hitbox"].get<float>();

	if (!j["struct"]["combat"]["reach"]["onlyweapon"].is_null())
		Instance<Settings>::Get()->Combat.Reach.m_OnlyWeapon = j["struct"]["combat"]["reach"]["onlyweapon"].get<bool>();

	if (!j["struct"]["combat"]["reach"]["onground"].is_null())
		Instance<Settings>::Get()->Combat.Reach.m_OnGround = j["struct"]["combat"]["reach"]["onground"].get<bool>();

	if (!j["struct"]["combat"]["reach"]["liquidcheck"].is_null())
		Instance<Settings>::Get()->Combat.Reach.m_LiquidCheck = j["struct"]["combat"]["reach"]["liquidcheck"].get<bool>();

	if (!j["struct"]["combat"]["reach"]["combomode"].is_null())
		Instance<Settings>::Get()->Combat.Reach.m_ComboMode = j["struct"]["combat"]["reach"]["combomode"].get<bool>();

	/**/
	if (!j["struct"]["utilities"]["blink"]["enabled"].is_null())
		Instance<Settings>::Get()->Utilities.Blink.m_Enabled = j["struct"]["utilities"]["blink"]["enabled"].get<bool>();

	if (!j["struct"]["utilities"]["blink"]["bind"].is_null())
		Instance<Settings>::Get()->Utilities.Blink.m_Bind = j["struct"]["utilities"]["blink"]["bind"].get<int>();

	if (!j["struct"]["utilities"]["blink"]["showtimer"].is_null())
		Instance<Settings>::Get()->Utilities.Blink.m_ShowTimer = j["struct"]["utilities"]["blink"]["showtimer"].get<bool>();

	if (!j["struct"]["utilities"]["blink"]["timerlimit"].is_null())
		Instance<Settings>::Get()->Utilities.Blink.m_TimerLimit = j["struct"]["utilities"]["blink"]["timerlimit"].get<int>();

	if (!j["struct"]["utilities"]["blink"]["showpath"].is_null())
		Instance<Settings>::Get()->Utilities.Blink.m_ShowPath = j["struct"]["utilities"]["blink"]["showpath"].get<bool>();

	if (!j["struct"]["utilities"]["blink"]["markercolor"].is_null())
	{
		const auto k = j["struct"]["utilities"]["blink"]["markercolor"];
		for (size_t i = 0; i < k.size(); i++)
			Instance<Settings>::Get()->Utilities.Blink.m_MarkerColor[i] = k[i].get<float>();
	}

	/**/
	if (!j["struct"]["utilities"]["bridgeassist"]["enabled"].is_null())
		Instance<Settings>::Get()->Utilities.BridgeAssist.m_Enabled = j["struct"]["utilities"]["bridgeassist"]["enabled"].get<bool>();

	if (!j["struct"]["utilities"]["bridgeassist"]["bind"].is_null())
		Instance<Settings>::Get()->Utilities.BridgeAssist.m_Bind = j["struct"]["utilities"]["bridgeassist"]["bind"].get<int>();

	if (!j["struct"]["utilities"]["bridgeassist"]["switchDelay"].is_null())
		Instance<Settings>::Get()->Utilities.BridgeAssist.m_OnlyRightClick = j["struct"]["utilities"]["bridgeassist"]["switchDelay"].get<int>();

	/**/
	if (!j["struct"]["misc"]["throwpot"]["enabled"].is_null())
		Instance<Settings>::Get()->Utilities.ThrowPot.m_Enabled = j["struct"]["misc"]["throwpot"]["enabled"].get<bool>();

	if (!j["struct"]["misc"]["throwpot"]["bind"].is_null())
		Instance<Settings>::Get()->Utilities.ThrowPot.m_Bind = j["struct"]["misc"]["throwpot"]["bind"].get<int>();

	if (!j["struct"]["misc"]["throwpot"]["switchDelay"].is_null())
		Instance<Settings>::Get()->Utilities.ThrowPot.m_SwitchDelay = j["struct"]["misc"]["throwpot"]["switchDelay"].get<int>();

	if (!j["struct"]["misc"]["throwpot"]["throwDelay"].is_null())
		Instance<Settings>::Get()->Utilities.ThrowPot.m_ThrowDelay = j["struct"]["misc"]["throwpot"]["throwDelay"].get<int>();

	if (!j["struct"]["misc"]["throwpot"]["mode"]["mode_selected"].is_null())
		Instance<Settings>::Get()->Utilities.ThrowPot.Mode.m_Mode = j["struct"]["misc"]["throwpot"]["mode"]["mode_selected"].get<int>();


	if (!j["struct"]["misc"]["throwpearl"]["enabled"].is_null())
		Instance<Settings>::Get()->Utilities.ThrowPearl.m_Enabled = j["struct"]["misc"]["throwpearl"]["enabled"].get<bool>();

	if (!j["struct"]["misc"]["throwpearl"]["bind"].is_null())
		Instance<Settings>::Get()->Utilities.ThrowPearl.m_Bind = j["struct"]["misc"]["throwpearl"]["bind"].get<int>();

	if (!j["struct"]["misc"]["throwpearl"]["switchDelay"].is_null())
		Instance<Settings>::Get()->Utilities.ThrowPearl.m_SwitchDelay = j["struct"]["misc"]["throwpearl"]["switchDelay"].get<int>();

	if (!j["struct"]["misc"]["throwpearl"]["throwDelay"].is_null())
		Instance<Settings>::Get()->Utilities.ThrowPearl.m_ThrowDelay = j["struct"]["misc"]["throwpearl"]["throwDelay"].get<int>();

	/**/
	if (!j["struct"]["movement"]["timer"]["enabled"].is_null())
		Instance<Settings>::Get()->Movement.TimerSpeed.m_Enabled = j["struct"]["movement"]["timer"]["enabled"].get<bool>();

	if (!j["struct"]["movement"]["timer"]["bind"].is_null())
		Instance<Settings>::Get()->Movement.TimerSpeed.m_Bind = j["struct"]["movement"]["timer"]["bind"].get<int>();

	if (!j["struct"]["movement"]["timer"]["value"].is_null())
		Instance<Settings>::Get()->Movement.TimerSpeed.m_Speed = j["struct"]["movement"]["timer"]["value"].get<float>();

	if (!j["struct"]["movement"]["timer"]["moving"].is_null())
		Instance<Settings>::Get()->Movement.TimerSpeed.m_Moving = j["struct"]["movement"]["timer"]["moving"].get<bool>();

	if (!j["struct"]["movement"]["timer"]["onlyweapon"].is_null())
		Instance<Settings>::Get()->Movement.TimerSpeed.m_OnlyWeapon = j["struct"]["movement"]["timer"]["onlyweapon"].get<bool>();

	/**/
	if (!j["struct"]["movement"]["bhop"]["enabled"].is_null())
		Instance<Settings>::Get()->Movement.Bhop.m_Enabled = j["struct"]["movement"]["bhop"]["enabled"].get<bool>();

	if (!j["struct"]["movement"]["bhop"]["bind"].is_null())
		Instance<Settings>::Get()->Movement.Bhop.m_Bind = j["struct"]["movement"]["bhop"]["bind"].get<int>();

	if (!j["struct"]["movement"]["bhop"]["power"].is_null())
		Instance<Settings>::Get()->Movement.Bhop.m_Power = j["struct"]["movement"]["bhop"]["power"].get<float>();

	if (!j["struct"]["movement"]["bhop"]["liquidCheck"].is_null())
		Instance<Settings>::Get()->Movement.Bhop.m_LiquidCheck = j["struct"]["movement"]["bhop"]["liquidCheck"].get<bool>();

	/**/
	if (j["struct"]["visuals"]["esp"]["enabled"].is_null())
		Instance<Settings>::Get()->Visuals.ESP.m_Enabled = j["struct"]["visuals"]["esp"]["enabled"].get<bool>();

	if (!j["struct"]["visuals"]["esp"]["bind"].is_null())
		Instance<Settings>::Get()->Visuals.ESP.m_Bind = j["struct"]["visuals"]["esp"]["bind"].get<int>();

	if (!j["struct"]["visuals"]["esp"]["boxes"].is_null())
		Instance<Settings>::Get()->Visuals.ESP.m_Boxes = j["struct"]["visuals"]["esp"]["boxes"].get<bool>();

	if (!j["struct"]["visuals"]["esp"]["mode"].is_null())
		Instance<Settings>::Get()->Visuals.ESP.m_Mode = j["struct"]["visuals"]["esp"]["mode"].get<int>();

	if (!j["struct"]["visuals"]["esp"]["healthbar"].is_null())
		Instance<Settings>::Get()->Visuals.ESP.m_Healthbar = j["struct"]["visuals"]["esp"]["healthbar"].get<bool>();

	if (!j["struct"]["visuals"]["esp"]["names"].is_null())
		Instance<Settings>::Get()->Visuals.ESP.m_Names = j["struct"]["visuals"]["esp"]["names"].get<bool>();

	if (!j["struct"]["visuals"]["esp"]["chams"].is_null())
		Instance<Settings>::Get()->Visuals.ESP.m_Chams = j["struct"]["visuals"]["esp"]["chams"].get<bool>();

	if (!j["struct"]["visuals"]["esp"]["filledcolor"].is_null())
	{
		const auto k = j["struct"]["visuals"]["esp"]["filledcolor"];
		for (size_t i = 0; i < k.size(); i++)
			Instance<Settings>::Get()->Visuals.ESP.m_FilledColor[i] = k[i].get<float>();
	}

	if (!j["struct"]["visuals"]["esp"]["outlinecolor"].is_null())
	{
		const auto k = j["struct"]["visuals"]["esp"]["outlinecolor"];
		for (size_t i = 0; i < k.size(); i++)
			Instance<Settings>::Get()->Visuals.ESP.m_OutlineColor[i] = k[i].get<float>();
	}

	/**/
	if (!j["struct"]["visuals"]["tracers"]["enabled"].is_null())
		Instance<Settings>::Get()->Visuals.Tracers.m_Enabled = j["struct"]["visuals"]["tracers"]["enabled"].get<bool>();

	if (!j["struct"]["visuals"]["tracers"]["bind"].is_null())
		Instance<Settings>::Get()->Visuals.Tracers.m_Bind = j["struct"]["visuals"]["tracers"]["bind"].get<int>();

	if (!j["struct"]["visuals"]["tracers"]["color"].is_null())
	{
		const auto k = j["struct"]["visuals"]["tracers"]["color"];
		for (size_t i = 0; i < k.size(); i++)
			Instance<Settings>::Get()->Visuals.Tracers.m_Color[i] = k[i].get<float>();
	}

	if (!j["struct"]["visuals"]["tracers"]["thickness"].is_null())
		Instance<Settings>::Get()->Visuals.Tracers.thickness = j["struct"]["visuals"]["tracers"]["thickness"].get<float>();

	/**/
	if (!j["struct"]["visuals"]["arraylist"]["enabled"].is_null())
		Instance<Settings>::Get()->Visuals.ArrayList.m_Enabled = j["struct"]["visuals"]["arraylist"]["enabled"].get<bool>();

	if (!j["struct"]["visuals"]["arraylist"]["bind"].is_null())
		Instance<Settings>::Get()->Visuals.ArrayList.m_Bind = j["struct"]["visuals"]["arraylist"]["bind"].get<int>();

	if (!j["struct"]["visuals"]["arraylist"]["colorbar"].is_null())
		Instance<Settings>::Get()->Visuals.ArrayList.m_ColorBar = j["struct"]["visuals"]["arraylist"]["colorbar"].get<bool>();
	
	if (!j["struct"]["visuals"]["arraylist"]["rainbow"].is_null())
		Instance<Settings>::Get()->Visuals.ArrayList.m_Rainbow = j["struct"]["visuals"]["arraylist"]["rainbow"].get<bool>();

	if (!j["struct"]["visuals"]["arraylist"]["background"].is_null())
		Instance<Settings>::Get()->Visuals.ArrayList.m_Background = j["struct"]["visuals"]["arraylist"]["background"].get<bool>();

	if (!j["struct"]["visuals"]["arraylist"]["rainbowsaturation"].is_null())
		Instance<Settings>::Get()->Visuals.ArrayList.m_RainbowSaturation = j["struct"]["visuals"]["arraylist"]["rainbowsaturation"].get<float>();

	if (!j["struct"]["visuals"]["arraylist"]["color"].is_null())
	{
		const auto k = j["struct"]["visuals"]["arraylist"]["color"];
		for (size_t i = 0; i < k.size(); i++)
			Instance<Settings>::Get()->Visuals.ArrayList.m_Color[i] = k[i].get<float>();
	}

	/**/
	if (!j["struct"]["visuals"]["blockesp"]["enabled"].is_null())
		Instance<Settings>::Get()->Visuals.BlockESP.m_Enabled = j["struct"]["visuals"]["blockesp"]["enabled"].get<bool>();

	if (!j["struct"]["visuals"]["blockesp"]["bind"].is_null())
		Instance<Settings>::Get()->Visuals.BlockESP.m_Bind = j["struct"]["visuals"]["blockesp"]["bind"].get<int>();
	
	if (!j["struct"]["visuals"]["blockesp"]["radius"].is_null())
		Instance<Settings>::Get()->Visuals.BlockESP.m_Radius = j["struct"]["visuals"]["blockesp"]["radius"].get<int>();

	if (!j["struct"]["visuals"]["blockesp"]["delay"].is_null())
		Instance<Settings>::Get()->Visuals.BlockESP.m_Delay = j["struct"]["visuals"]["blockesp"]["delay"].get<int>();

	if (!j["struct"]["visuals"]["blockesp"]["blocklist"].is_null())
	{
		const auto k = j["struct"]["visuals"]["blockesp"]["blocklist"];
		for (size_t i = 0; i < k.size(); i++)
			Instance<Settings>::Get()->Visuals.BlockESP.blockList[i] = k[i].get<int>();
	}

	/**/

#pragma endregion
}

void UserConfigs::Reload(std::vector<std::string>& out)
{
	char* szQuery = new char[126];
	sprintf(szQuery, "action=get&creator_id=%s", std::to_string(m_UID).data());

	
	delete[] szQuery;
}