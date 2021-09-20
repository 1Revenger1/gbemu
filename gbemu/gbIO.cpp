#include "gb.h"
#include <tchar.h>
#include <Windows.h>

static UINT8* getPtr(Gb* gb, UINT16 addr)
{
	if (addr < 0x4000)
	{
		return &gb->rom.romBank0[addr];
	}
	
	if (addr < 0x8000)
	{
		return &gb->rom.romBankN[addr - 0x4000];
	}

	if (addr < 0xA000)
	{
		return &gb->gpu.vram[addr - 0x8000];
	}

	// TODO: https://gbdev.io/pandocs/MBCs.html
	if (addr < 0xC000)
	{
		return &gb->rom.ram[addr - 0xA000];
	}

	if (addr <= 0xFDFF)
	{
		addr &= 0x1FFF;
		return addr < 0x1000 ?
			&gb->ram.ramBank0[addr] :
			&gb->ram.ramBankn[addr - 0x1000];
	}

	if (addr == 0xFFFF)
	{
		return &gb->cpu.interrupts;
	}

	debugPrint("Invalid read at %x\n", addr);
	exit(0x21);
	return NULL;
}

void pushStackByte(Gb* gb, UINT8 val) {

}

void pushStackShort(Gb* gb, UINT16 val) {
	UINT16 stackPtr = gb->cpu.SP - 0xFF80;
	gbRam &ram = gb->ram;

	ram.hram[stackPtr--] = val >> 8;
	ram.hram[stackPtr--] = val & 0xff;
	gb->cpu.SP = stackPtr + 0xFF80;
}

UINT8 popStackByte(Gb* gb) {
	return 0;
}

UINT16 popStackShort(Gb* gb) {
	UINT16 stackPtr = gb->cpu.SP - 0xFF80;
	UINT16 val;
	gbRam &ram = gb->ram;

	val  = ram.hram[stackPtr++] & 0xFF;
	val += ram.hram[stackPtr++] << 8;
	gb->cpu.SP = stackPtr + 0xFF80;
	return val;
}

UINT8 readByte(Gb* gb, UINT16 addr) {
	UINT8* loc = getPtr(gb, addr);
	return *loc;
}

UINT16 readShort(Gb* gb, UINT16 addr) {
	UINT8* loc = getPtr(gb, addr);
	return (*(loc + 1) << 8) + *(loc);
}

void writeByte(Gb* gb, UINT16 addr, UINT8 val)
{
	debugPrint("writeByte: %x = %x\n", addr, val);
}