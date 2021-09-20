#pragma once
#include <windows.h>
#include <stdio.h>
#include "gbcpu.h"

#define MAX_SIZE 0xFFFF


#define KiloByte		1024
#define ROM_BANK_SIZE	16 * KiloByte
#define VRAM_SIZE		8 * KiloByte
#define ERAM_SIZE		8 * KiloByte
#define WRAM_BANKS_SIZE 4 * KiloByte

#define SPRITE_ATTR_SIZE 0xFE9F - 0xFE00
#define HIGH_RAM_SIZE 0xFFFE - 0xFF80

#define ROM_BANK_TO_PTR(buffer, n) ((UINT8 *) buffer + ((n) * ROM_BANK_SIZE))

struct gbRom {
	UINT8* romBank0;
	UINT8* romBankN;
	int bank{ 0 };

	// Battery backed data
	UINT8 ram[ERAM_SIZE];
	int ramBank{ 0 };

	LPVOID romBuffer;
	HANDLE hRom;
	DWORD  romSize;
};

struct gbGpu {
	UINT8 vram[VRAM_SIZE];
	// int bank{ 0 };	// Two banks on Gameboy Color

	UINT8 spriteAttr[SPRITE_ATTR_SIZE];
};

struct gbRam {
	UINT8 ramBank0[WRAM_BANKS_SIZE];
	UINT8 ramBankn[WRAM_BANKS_SIZE];
	// int wramBank{ 0 };

	UINT8 hram[HIGH_RAM_SIZE];
};

struct Gb {
	gbRom rom;
	gbGpu gpu;
	gbRam ram;

	gbCpu cpu;
};


void startupRom(Gb* gb);
void execute(Gb* gb);
UINT8 readByte(Gb* gb, UINT16 addr);
void pushStackShort(Gb* gb, UINT16 val);
UINT16 readStackShort(Gb* gb);
UINT16 readShort(Gb* gb, UINT16 addr);
void writeByte(Gb* gb, UINT16 addr, UINT8 val);

inline void debugPrint(const char *fmt...) {
	char str[1024];
	va_list args;
	va_start(args, fmt);
	vsprintf_s(str, sizeof(str), fmt, args);
	va_end(args);
	OutputDebugStringA(str);
}
