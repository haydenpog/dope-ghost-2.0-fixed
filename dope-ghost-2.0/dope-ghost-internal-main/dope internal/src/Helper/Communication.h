#pragma once

class Settings;
class Communication
{
public:
	static Settings* GetSettings();
	static void Unload();

private:
	static bool m_Loaded;
	static HANDLE m_Mapping;
	static LPVOID m_Buffer;
};