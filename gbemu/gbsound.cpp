#include "gbsound.h"

int gbSound::writeByte(UINT16 addr, UINT8 byte) {
	if (addr >= 0xFF10 && addr < 0xFF27) {
		mem[addr - 0xFF10] = byte;
		return 0;
	}

	if (addr >= 0xFF30 && addr < 0xFF40) {
		mem[addr - 0xFF10] = byte;
		return 0;
	}

	return -1;
}

int gbSound::readByte(UINT16 addr) {
	if (addr >= 0xFF10 && addr < 0xFF27) {
		return mem[addr - 0xFF10];
	}

	if (addr >= 0xFF30 && addr < 0xFF40) {
		return mem[addr - 0xFF10];
	}

	return -1;
}