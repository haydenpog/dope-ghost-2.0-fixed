#include "pch.h"
#include "Communication.h"

#include "../Cheat/Settings.h"

Communication::Communication()
	: m_Size(NULL), m_Mapping(NULL), m_Buffer(NULL), m_Loaded(false)
{
	m_Size = sizeof(Settings);
	m_Mapping = CreateFileMappingW(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		m_Size,
		L"win32");

	if (!m_Mapping)
		return;

	m_Buffer = MapViewOfFile(m_Mapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	m_Loaded = true;
}
