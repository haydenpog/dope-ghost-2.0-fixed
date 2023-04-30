#pragma once
#include <vector>
#include "../../../vendors/Singleton.h"

class Security
{
private:
	DWORD64 m_OrgTextSection;
public:
	Security() = default;
	~Security() = default;

	bool HasHooks();
	bool IsBeingDebugged();
	void AntiAttach();
	bool IsOnVM();

	DWORD64 HashSection(LPVOID lpSectionAddress, DWORD dwSizeOfRawData);
	std::vector<HASHSET> GetModulesSectionHash();
};