#pragma once

class Communication
{
public:

	Communication();

	~Communication() {
		if (m_Mapping)
			CloseHandle(m_Mapping);

		UnmapViewOfFile(m_Buffer);
	}

	bool IsLoaded() const {
		return m_Loaded;
	}

	LPVOID GetBuffer() const {
		return m_Buffer;
	}

private:
	int m_Size;
	HANDLE m_Mapping;
	LPVOID m_Buffer;
	bool m_Loaded;

};