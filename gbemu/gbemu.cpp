#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#include "gbcpu.h"
#include "gb.h"

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	Gb* gb = new Gb();
	HANDLE hRom = CreateFileA(
		"Pokemon Blue.gb",
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (hRom == INVALID_HANDLE_VALUE) {
		OutputDebugString(_T("Failed to open rom\n"));
		return -1;
	}
	else {
		DWORD fileSize = GetFileSize(
			hRom,
			NULL
		);

		debugPrint("Found rom of size %x\n", fileSize);

		UINT8* romBuffer = new UINT8[fileSize];
		DWORD bytesRead;

		if (!ReadFile(
			hRom,
			romBuffer,
			fileSize,
			&bytesRead,
			NULL
		))
		{
			OutputDebugString(_T("Failed to read rom\n"));
			return -1;
		}

		gbRom* rom = &gb->rom;

		rom->hRom = hRom;
		rom->romSize = fileSize;
		rom->romBuffer = romBuffer;

		rom->romBank0 = ROM_BANK_TO_PTR(romBuffer, 0);
		rom->romBankN = ROM_BANK_TO_PTR(romBuffer, 1);

		startupRom(gb);
	}

	return 0;
}