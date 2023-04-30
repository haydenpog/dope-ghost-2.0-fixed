#include "pch.h"
#include "InjectionHelper.h"

#include <string.h>

bool InjectionHelper::Inject(DWORD pid, void *buf, int size)
{
	if (!NT_SUCCESS(this->thisProc.Attach(pid, PROCESS_ALL_ACCESS)))
		return false;

	if (size < 0x1000)
		return false;

	auto image = this->thisProc.mmap().MapImage(size, buf, false, blackbone::NoTLS | blackbone::WipeHeader | blackbone::ForceRemap);
	return true;
}

void InjectionHelper::Unload()
{
	if (NT_SUCCESS(this->thisProc.mmap().UnmapAllModules())) {
		this->thisProc.mmap().Cleanup();
	}
}
