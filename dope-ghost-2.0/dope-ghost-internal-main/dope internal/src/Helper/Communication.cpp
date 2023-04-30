#include "pch.h"
#include "Communication.h"

Settings* Communication::GetSettings()
{
	if (!m_Loaded)
	{
		m_Mapping = OpenFileMapping(
			FILE_MAP_ALL_ACCESS,
			FALSE,
			"win32");

		m_Buffer = MapViewOfFile(m_Mapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		m_Loaded = true;
	}

	return (Settings*)m_Buffer;
}

void Communication::Unload() {
	UnmapViewOfFile(m_Buffer);
}

bool Communication::m_Loaded;
HANDLE Communication::m_Mapping;
LPVOID Communication::m_Buffer;