#include "gb.h"
#include "stdio.h"

void startupRom(Gb* gb)
{
	gbCpu* cpu = &gb->cpu;

	cpu->SP = (UINT16) 0xFFFE;
	cpu->PC = 0x100;
	memset(gb->gpu.vram, 0, VRAM_SIZE);

	debugPrint("RomName: %s\nRomType: %x\n", 
		&gb->rom.romBank0[0x134], 
		gb->rom.romBank0[0x147]);

	execute(gb);
}