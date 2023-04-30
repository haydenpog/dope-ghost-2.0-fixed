#pragma once
#include <vector>

class Settings {
public:
	bool m_Destruct{ false };

	struct
	{
		struct
		{
			bool m_Enabled{ false };

			bool m_Boxes{ false };

			int m_Bind{ NULL };
			int m_Mode{ 0 }; // 0 -> 2d, 1->3d outlined, 2->3d filled

			bool m_Healthbar{ false };
			bool m_Names{ false };
			bool m_Chams{ false };

			float m_FilledColor[3]{ 0.f, 0.f, 0.f };
			float m_OutlineColor[3]{ 89.f / 255.f, 161.f / 255.f, 236.f / 255.f };
		}ESP;

		struct
		{
			bool m_Enabled{ false };

			int m_Bind{ NULL };

			float m_Color[3]{ 89.f / 255.f, 161.f / 255.f, 236.f / 255.f };
			float thickness{ 1.f };
		}Tracers;

		struct
		{
			bool m_Enabled{ false };

			int m_Bind{ NULL };

			bool m_ColorBar{ false };
			bool m_Rainbow{ false };
			bool m_Background{ false };

			float m_RainbowSaturation{ 0.5f };
			float m_Color[3]{ 89.f / 255.f, 161.f / 255.f, 236.f / 255.f };
		}ArrayList;

		struct
		{
			bool m_Enabled{ false };
			int m_Bind{ NULL };

			int m_Radius{ 25 };
			int m_Delay{ 2500 };

			int blockList[255]{ 0x0 };
			/* very messy allocation but std::vector cause issue with shared mem
				00000000 00000000 00000000 00000000
				(char)value = block id
				(char)value >> 8 = B
				(char)value >> 16 = G
				(char)value >> 24 = R
			*/
		}BlockESP;
	}Visuals;

	struct
	{

		struct
		{
			bool m_Enabled{ false };
			int m_Bind{ NULL };

			double m_Horizontal{ 80.0 };
			double m_Vertical{ 100.0 };
			int m_Chance{ 100 };
			int m_Delay{ 2 };

			bool m_AirOnly{ false };
			bool m_Moving{ false };
			bool m_WeaponOnly{ false };
		}Velocity;

		struct
		{
			bool m_Enabled{ false };
			int m_Bind{ NULL };

			float m_Distance{ 3.6f };
			float m_Hitbox{ 0.0f };

			bool m_OnlyWeapon{ false };
			bool m_OnGround{ false };
			bool m_LiquidCheck{ false };
			bool m_ComboMode{ false };
		}Reach;

		struct
		{
			bool m_Enabled{ false };
			int m_Bind{ NULL };

			float m_Distance{ 6.f };

			int m_Fov{ 70 };
			float m_Speed{ 2.5f };

			bool m_OnlyWeapon{ true };
			bool m_ThroughWall{ false };
			bool m_ClickingOnly{ false };
			bool m_PitchRand{ false };

			struct
			{
				int m_Mode{ 0 };
			}Mode;
		}AimAssist;

		struct
		{
			bool m_Enabled{ false };
			int m_Bind{ NULL };

			int m_Average{ 14 };

			bool m_BreakBlock{ true };
			bool m_OnlyWeapon{ true };
			bool m_InInventory{ false };
			bool m_SmartBlockHit{ false };
		}LeftClicker;

		struct
		{
			bool m_Enabled{ false };
			int m_Bind{ NULL };

			int m_Average{ 14 };

			bool m_OnlyBlock{ true };
		}RightClicker;

	}Combat;

	struct
	{

		struct
		{
			bool m_Enabled{ false };
			int m_Bind{ NULL };

			bool m_ShowTimer{ false };
			float m_TimerLimit{ 7.5f };

			bool m_ShowPath{ false };

			float m_MarkerColor[3]{ 1.f, 1.f, 1.f };
		}Blink;

		struct
		{
			bool m_Enabled{ false };
			int m_Bind{ NULL };

			int m_SwitchDelay{ 50 }; // min: 25
			int m_ThrowDelay{ 50 }; // min: 50

			struct
			{
				int m_Mode{ 0 }; // 0 = one pot, 1 = double pot, 2 = smart
			}Mode;
		}ThrowPot;

		struct
		{
			bool m_Enabled{ false };
			int m_Bind{ NULL };

			int m_SwitchDelay{ 50 }; // min: 25
			int m_ThrowDelay{ 50 }; // min: 50
		}ThrowPearl;

		struct
		{
			bool m_Enabled{ false };
			int m_Bind{ NULL };

			bool m_OnlyRightClick{ false };
		}BridgeAssist;

	}Utilities;

	struct
	{

		struct
		{
			bool m_Enabled{ false };
			int m_Bind{ NULL };

			float m_Power{ 1.0f };
			bool m_LiquidCheck{ false };
		}Bhop;

		struct
		{
			bool m_Enabled{ false };
			int m_Bind{ NULL };

			float m_Speed{ 1.f };
			bool m_Moving{ false };
			bool m_OnlyWeapon{ false };
		}TimerSpeed;

	}Movement;
};

class Globals
{
public:
	HWND m_Hwnd;
	DWORD m_Pid;

	struct
	{
		std::vector<unsigned long long> m_Heartbeats = { GetTickCount64(), GetTickCount64(), GetTickCount64() };
		char m_Flag1{ 0x0 };
		char m_Flag2{ 0x0 };
		char m_Flag3{ 0x0 };
	}Flags;

	struct
	{
		std::string m_Token;
		std::string dllOne;
		std::string dllTwo;
		int dllSize;

		std::string userName;
		int m_UserID{ 0 };
		int m_Expiration{ 0 };
		int m_TokenCreation{ 0 };
	}User;

	struct
	{
		bool m_Rainbow{ false };
		float m_AccentColor[3]{ 59.f / 255.f, 120.f / 255.f, 254.f / 255.f };
	}Gui;
};