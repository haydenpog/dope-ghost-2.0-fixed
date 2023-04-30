#include "pch.h"
#include "Helper/Communication.h"

#include "Cheat/Settings.h"
#include "Cheat/InjectionHelper.h"
#include "Cheat/Security/Helper.h"

#include "Interface/UserInterface.h"

#include <fstream>
#include <filesystem>

bool EnableTokenPriviliges(HANDLE processhandle, const char* permchar)
{
	HANDLE tokenhandle;
	LUID permissionidentifier;
	TOKEN_PRIVILEGES tokenpriv;
	if (OpenProcessToken(processhandle, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &tokenhandle))
	{
		if (LookupPrivilegeValue(NULL, permchar, &permissionidentifier))
		{
			tokenpriv.PrivilegeCount = 1;
			tokenpriv.Privileges[0].Luid = permissionidentifier;
			tokenpriv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (AdjustTokenPrivileges(tokenhandle, false, &tokenpriv, sizeof(tokenpriv), NULL, NULL)) { return true; }
			else { return false; }
		}
		else { return false; }
	}
	else { return false; }
	CloseHandle(tokenhandle);
}

int main()
{
	srand((unsigned int)time_t(NULL));
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	EnableTokenPriviliges(GetCurrentProcess(), SE_SECURITY_NAME);
	EnableTokenPriviliges(GetCurrentProcess(), SE_DEBUG_NAME);


	auto comm = new Communication();
	if (!comm->IsLoaded()) {
		FatalAppExitW(-1, L"An error occured while initializing communication.");
	}



	std::thread([&comm] {
		while (!Instance<Settings>::Get()->m_Destruct) {
			Sleep(1);
			if(comm->GetBuffer())
				memcpy(comm->GetBuffer(), Instance<Settings>::Get(), sizeof(Settings));
		}
	}).detach();


	std::thread([&comm] {
		const auto userInterface = new UserInterface(850.f, 595.f);
		userInterface->Display();
		delete userInterface;
	}).detach();

	Instance<Globals>::Get()->User.m_Token = "1e4b2ad3fe2bb590";


	while (!Instance<Settings>::Get()->m_Destruct) {
		Sleep(1);
	}

	Sleep(3500);
	Instance<InjectionHelper>::Get()->Unload();
	delete comm;
	return 0;
}