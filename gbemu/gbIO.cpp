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

	return NULL;
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