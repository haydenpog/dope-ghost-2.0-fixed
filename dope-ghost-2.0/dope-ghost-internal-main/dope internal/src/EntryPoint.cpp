#include "pch.h"
#include "Cheat/Hack.h"
#include "Cheat/Modules/Module.h"

#include "Cheat/Modules/Settings.h"

#include "Helper/Communication.h"
#include "Helper/HookFunction.h"

DWORD __stdcall Main(HMODULE hModule)
{
#if DOPE_DEBUG
	AllocConsole();
	FILE* fStream;
	freopen_s(&fStream, "conin$", "r", stdin);
	freopen_s(&fStream, "conout$", "w", stdout);
	freopen_s(&fStream, "conout$", "w", stderr);
#endif

    g_Instance = std::make_unique<Hack>();
    g_Instance->Attach();
    g_Instance->InitializeGame();
    
	if (g_Instance->GetInitializationState() == ERR) {
		MessageBoxA(NULL, "An error occured while initializing dope.", "", MB_OK | MB_ICONERROR);
#if DOPE_DEBUG
		ShowWindow(GetConsoleWindow(), SW_HIDE);
		FreeConsole();
		FreeLibraryAndExitThread(hModule, 0);
		return 0;
#endif
	}
    else if (g_Instance->GetInitializationState() == INCOMPLETE)
    {
		MessageBoxA(NULL, "The dope initializing process ended with the INCOMPLETE status, we highly recommend restarting your game to prevent any issues.", "", MB_OK | MB_ICONINFORMATION);
    }

	const auto code = g_Instance->CleanupJVMTI();
	if (code == ERR) {
		MessageBoxA(NULL, "The dope cleaning process ended with the ERR status, you may not be safe in screenshare.\nIf this issue still happen please report it and make sure to give the game version and client you are using.", "", MB_OK | MB_ICONINFORMATION);
	}

	Modules::InitializeModules();
	HookFunction::RunAll();

	while (!Communication::GetSettings()->m_Destruct)
	{
		Sleep(25);
	}

	Sleep(1000);

    g_Instance->Detach();

#if DOPE_DEBUG
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
#endif
	return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
		if (const auto handle = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)Main, hModule, 0, nullptr); handle != NULL)
			CloseHandle(handle);
    }

    return TRUE;
}