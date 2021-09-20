#pragma once
#include <windows.h>
#include "gbcpu.h"

#define MAX_SIZE 0xFFFF

// interrupt enable flags
#define VBLANK_INTR		BIT(0)
#define LCDSTAT_INTR	BIT(1)
#define TIMER_INTR		BIT(2)
#define SERIAL_INTR		BIT(3)
#define JOYPAD_INTR		BIT(4)

#define KiloByte		1024
#define ROM_BANK_SIZE	16 * KiloByte
#define VRAM_SIZE		8 * KiloByte
#define ERAM_SIZE		8 * KiloByte
#define WRAM_BANKS_SIZE 4 * KiloByte

#define SPRITE_ATTR_SIZE 0xFE9F - 0xFE00

#define ROM_BANK_TO_PTR(buffer, n) ((char *) buffer + ((n) * ROM_BANK_SIZE))

struct gbRom {
	CHAR* romBank0;
	CHAR* romBankN;
	int bank{ 0 };

	LPVOID romBuffer;
	HANDLE hRom;
	DWORD  romSize;
};

struct gbGpu {
	CHAR vram[VRAM_SIZE];
	int bank{ 0 };	// Two banks on Gameboy Color

	CHAR spriteAttr[SPRITE_ATTR_SIZE];
};

struct gbRam {
	CHAR eram[ERAM_SIZE];
	int eramBank{ 0 };

	CHAR ramBank0[WRAM_BANKS_SIZE];
	CHAR ramBankn[WRAM_BANKS_SIZE];
	int wramBank{ 0 };
};

struct Gb {
	gbRom rom;
	gbGpu gpu;
	gbRam ram;

	gbCpu cpu;
};


void startupRom(Gb* gb);
CHAR readByte(Gb* gb, SHORT addr);
