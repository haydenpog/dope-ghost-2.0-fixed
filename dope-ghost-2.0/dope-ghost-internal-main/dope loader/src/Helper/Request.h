#pragma once

class Request
{
public:
	Request(const char* link)
		: m_Link(link)
	{
	}

	std::string Perform(char* requestBody, size_t requestBodySize);

private:
	const char* m_Link;
};