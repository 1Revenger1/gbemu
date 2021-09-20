#include "gb.h"
#include "windows.h"

CHAR readByte(Gb* gb, SHORT addr)
{
	if (addr < 0x4000)
	{
		return gb->rom.romBank0[addr];
	}
	else if (addr < 0x8000)
	{
		return gb->rom.romBankN[addr - 0x4000];
	}
}