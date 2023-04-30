#include "pch.h"
#include "Authentification.h"

#include "../Helper.h"

#include "../../Settings.h"
#include "../../../Helper/Request.h"

#include "../../../../vendors/aes.h"
#include "../../../../vendors/json.h"
#include "../../../../vendors/caesar.h"

#include <shellapi.h>
#include <Lmcons.h>
#include <sstream>
#include <fstream>
#include <regex>
#include <shlobj.h>
#include <filesystem>


#pragma optimize("", off)

std::string Authentification::GetHwid()
{

	DWORD VolumeSerialNumber = 0;
	GetVolumeInformation("c:\\", NULL, NULL, &VolumeSerialNumber, NULL, NULL, NULL, NULL);

	std::ostringstream hwid;
	hwid << std::hex << std::uppercase << VolumeSerialNumber;

	return hwid.str();

}

bool Authentification::Login(bool& outStatus)
{
	if (!m_Token.compare("") || outStatus == true)
	{
		exit(EXIT_SUCCESS);
	}


	ULONGLONG timer = GetTickCount64();
	srand((unsigned)time(NULL));

	auto key = new uint8_t[32];
	for (unsigned i = 0; i < 32; i++)
		*(uint8_t*)(key + i) = rand() % 0xff + 1;

	auto iv = new uint8_t[16];
	for (unsigned i = 0; i < 16; i++)
		*(uint8_t*)(iv + i) = rand() % 0xff + 1;

	auto xorKey = new uint8_t[16];
	for (unsigned i = 0; i < 16; i++)
		*(uint8_t*)(xorKey + i) = rand() % 0xff + 1;

	AES_ctx context;

	AES_init_ctx_iv(&context, key, iv);


	auto GetUsername = []() {
		TCHAR name[UNLEN + 1]; DWORD size = UNLEN + 1;
		auto ret = GetUserName((CHAR*)name, &size);

		return ret ? std::string(name) : std::string("Unknown");
	};

	auto GetUserDirectory = []()
	{
		PWSTR userFolderPath; SHGetKnownFolderPath(FOLDERID_Profile, 0, NULL, &userFolderPath);
		auto wstr = std::wstring(userFolderPath);
		std::wstring userDir(wstr.begin(), wstr.end());

		return userDir;
	};

	auto GetMinecraftUsernames = [&GetUserDirectory]() {
		std::vector<std::wstring> gameDir = { std::wstring(_wgetenv(L"APPDATA")) + L"\\.minecraft" + L"\\launcher_accounts.json", GetUserDirectory() + L"\\.lunarclient\\settings\\game\\accounts.json" };

		auto accounts = std::string("");
		for (const auto& dir : gameDir) {
			std::ifstream fin(dir.data());
			if (fin.fail()) {
				return "";
			}

			std::string rawData((std::istreambuf_iterator<char>(fin)),
				(std::istreambuf_iterator<char>()));

			auto j = nlohmann::json::parse(rawData.data());
			auto jsonAccounts = j["accounts"];

			for (auto it = jsonAccounts.begin(); it != jsonAccounts.end(); ++it)
			{
				if (!it.value()["minecraftProfile"]["name"].is_null())
				{
					if (accounts.find(it.value()["minecraftProfile"]["name"].get<std::string>()) != std::string::npos)
						continue;

					if (accounts.size() != 0)
						accounts += ", ";

					accounts += it.value()["minecraftProfile"]["name"].get<std::string>();
				}
			}
		}

		return (const char*)accounts.data();
	};

	auto hasTracingFile = std::filesystem::exists(std::filesystem::temp_directory_path().wstring() + L"\\_CL_45644114475.tmp");
	if (hasTracingFile) {
		std::ofstream{ std::string(std::filesystem::temp_directory_path().string() + "\\c2a7a660-ca80-41d9-bb09-c133f11ee957-af23d.tmp").data() };
	}

	char* szQuery = new char[520];
	sprintf(szQuery, "token=%s&hwid=%s&ver=%s&clientTime=%i&isVerified=%i&pcName=%s&minecraftAccounts=%s", m_Token.data(), m_Hwid.data(), "1.0.1", time(NULL), hasTracingFile, GetUsername().data(), GetMinecraftUsernames());
	/**/

	uint8_t* query = (uint8_t*)szQuery;
	size_t querySize = strlen(szQuery);
	if (querySize % 16 != 0)
	{
		size_t oldSize = querySize;
		unsigned toFill = (16 - querySize % 16);
		querySize = oldSize + toFill;
		query = new uint8_t[querySize];
		memcpy(query, szQuery, oldSize);
		memset(query + oldSize, 0, toFill);
	}



	AES_CBC_encrypt_buffer(&context, query, (uint32_t)querySize);

	for (unsigned i = 0; i < querySize; i++)
		query[i] ^= xorKey[i % 16];

	size_t requestBodySize = 32 + 16 + 16 + querySize;
	uint8_t* requestBody = new uint8_t[requestBodySize];
	memcpy(requestBody, key, 32);
	memcpy(requestBody + 32, iv, 16);
	memcpy(requestBody + 32 + 16, xorKey, 16);
	memcpy(requestBody + 32 + 16 + 16, query, querySize);

	if (GetTickCount64() - timer > 10000) {
		outStatus = false;
		return true;
	}


	auto req = new Request("http://193.26.14.16/ghost_authv2/");
	std::string readBuffer = req->Perform((char*)requestBody, requestBodySize);
	delete req;

	delete[] szQuery;
	delete[] requestBody;


	/**/
	for (unsigned i = 0; i < readBuffer.size(); i++)
		readBuffer[i] ^= xorKey[i % 16];

		AES_init_ctx_iv(&context, key, iv);
	AES_CBC_decrypt_buffer(&context, (uint8_t*)readBuffer.data(), (uint32_t)readBuffer.size());

	timer = GetTickCount64();

	/**/
	char* buffer = readBuffer.data();
	auto curTime = *(int*)(buffer + 4);

	if (GetTickCount64() - timer > 10000) {
		ShellExecuteA(0, "runas", "cmd.exe", "cmd /c \"echo An error occured [0x1].\" & color c & pause", NULL, SW_SHOW);
		_exit(EXIT_SUCCESS);
	}
	else {
		if (time(NULL) - curTime > (7 * 86400) || abs(curTime - time(NULL)) > 25000) { // if it ever get cracked, depending on the way they did it,  it'll probably flag this after  7days
			// delayed crash
			Instance<Globals>::Get()->Flags.m_Flag2 = 0x2;
		}

		auto licenseExpiration = *(int*)(buffer + 12);
		if (licenseExpiration - time(NULL) < 0 && licenseExpiration != 0)
		{
			_exit(EXIT_SUCCESS); // expired sub
		}

		auto tokenCreation = *(int*)(buffer + 8);
		if (time(NULL) - tokenCreation > (7 * 86400)) { //if  it ever get cracked, depending on the way they did it,  it'll probably flag this after 7days
			Instance<Globals>::Get()->Flags.m_Flag2 = 0x2;
			// delayed crash
		}

		m_Status = *(int*)(buffer);
		if (m_Status == 0xFFF)
		{
			auto ret = (m_Status == 0xFFF) && (time(NULL) != 0) && (licenseExpiration - time(NULL) > 0) && readBuffer.size() != 0;
			if (!ret) {
				readBuffer.clear();
			}

			/**/
			auto usernameLen = *(int*)(buffer + 20);

			int dllOneSize = *(int*)(buffer + 24 + usernameLen);
			std::string dllOneBuffer(buffer + 24 + usernameLen + 4, dllOneSize);

			int dllTwoSize = *(int*)(buffer + 24 + usernameLen + 4 + dllOneSize);
			std::string dllTwoBuffer(buffer + 24 + usernameLen + 4 + dllOneSize + 4, dllOneSize);
			Instance<Globals>::Get()->User.dllOne = dllOneBuffer;
			Instance<Globals>::Get()->User.dllTwo = dllTwoBuffer;
			Instance<Globals>::Get()->User.dllSize = dllOneSize + dllTwoSize;

			Instance<Globals>::Get()->User.m_Expiration = licenseExpiration;
			Instance<Globals>::Get()->User.m_TokenCreation = tokenCreation;

			std::string caesarUsername(buffer + 24, usernameLen);
			this->m_UID = *(int*)(buffer + 16);
			this->m_Username = Decipher(caesarUsername, 6);

			Instance<Globals>::Get()->User.userName = m_Username;
			Instance<Globals>::Get()->User.m_UserID = m_UID;

			outStatus = true;
			return false;
		}
		else {
			// switch  statement  fuck with themida prot for some reason
			if (m_Status == 0xFFF3 || m_Status == 0xFFF4) {
				ShellExecuteA(0, "runas", "cmd.exe", "cmd /c \"echo The current build is outdated. Please redownload\" & color c & pause", NULL, SW_SHOW);
			}
			else if (m_Status == 0xFFF1) {
				ShellExecuteA(0, "runas", "cmd.exe", "cmd /c \"echo Your access to dope is gone.\" & color c & pause", NULL, SW_SHOW);
			}
			else if (m_Status == 0xFFF2) {
				{
					HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, m_Hwid.size() + 1); memcpy(GlobalLock(hMem), m_Hwid.data(), m_Hwid.size() + 1);
					GlobalUnlock(hMem);
					OpenClipboard(0);
					EmptyClipboard();
					SetClipboardData(CF_TEXT, hMem);
					CloseClipboard();
				}

				ShellExecuteA(0, "runas", "cmd.exe", "cmd /c \"echo Your HWID has been copied to your clipboard, please update it.\" & color c & pause", NULL, SW_SHOW);
			}
			else if (m_Status == 0xFFF5) {
				ShellExecuteA(0, "runas", "cmd.exe", "cmd /c \"Invalid login. Please re-download the loader\" & color c & pause", NULL, SW_SHOW);
			}
			else if (m_Status == 0x0) {
				ShellExecuteA(0, "runas", "cmd.exe", "cmd /c \"The current loader version is outdated. Please redownload\" & color c & pause", NULL, SW_SHOW);
			}
		}

		outStatus = false;
	}
	_exit(EXIT_SUCCESS);
	return true;
}