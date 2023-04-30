#pragma once

class Authentification
{
public:
	static std::string GetHwid();

public:
	Authentification(std::string hwid, std::string token)
		: m_Hwid(hwid), m_Token(token), m_Status(0), m_UID(0), m_Username("")
	{
	}

	bool Login(bool& outStatus);

private:
	int				m_Status;
	int				m_UID;
	std::string		m_Hwid;
	std::string		m_Token;
	std::string		m_Username;
};