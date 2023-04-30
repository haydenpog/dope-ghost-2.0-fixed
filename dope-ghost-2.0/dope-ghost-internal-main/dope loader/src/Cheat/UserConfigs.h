#pragma once
#include <vector>
#include "../../vendors/Singleton.h"

class UserConfigs
{
public:
	UserConfigs(int UID)
		: m_UID(UID), m_CID(NULL)
	{
	}

	UserConfigs(int UID, int CID) /*we prepare for public configs*/
		: m_UID(UID), m_CID(CID)
	{
	}

	void Create(std::string data);
	void Update(std::string data);
	void Delete();
	void Load(std::vector<std::string>& out);
	void Reload(std::vector<std::string>& out);

	static std::string CreateData(std::string name);

private:
	int m_UID;
	int m_CID;
};