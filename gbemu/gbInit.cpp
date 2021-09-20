#include "gb.h"
#include "stdio.h"

void startupRom(Gb* gb)
{
	gbCpu* cpu = &gb->cpu;

	cpu->SP = 0xFFFE;
	memset(gb->gpu.vram, 0, VRAM_SIZE);

	char str[13 + 0x10];
	sprintf_s(str, sizeof(str), "ROM Name: %s\n", &gb->rom.romBank0[0x134]);
	OutputDebugStringA(str);


}