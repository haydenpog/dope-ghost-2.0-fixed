#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <iostream>
#include <thread>

#include "vendors/Singleton.h"

typedef struct {
	LPVOID lpVirtualAddress;
	DWORD dwSizeOfRawData;
} SECTIONINFO, * PSECTIONINFO;

typedef struct {
	DWORD64 dwRealHash;
	SECTIONINFO SectionInfo;
} HASHSET, * PHASHSET;
