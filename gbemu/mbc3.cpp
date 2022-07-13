#include "gbrom.h"

MBC3Rom::MBC3Rom(UINT8* rom, DWORD romSize) : Rom(rom, romSize) {
	switch (hdr->cartType) {
	case MBC3_TIMER_BATT:
	case MBC3_TIMER_BATT_RAM:
		externalTimer = true;
		[[fallthrough]];
	case MBC3_RAM_BATT:
		externalBatt = true;
		break;
	}
}

int MBC3Rom::readByte(UINT16 addr) {
	// Lower ROM Bank
	if (addr < 0x4000) {
		return rom[addr];
	} 
	
	// Upper ROM bank (Switchable)
	if (addr < 0x8000) {
		addr -= ROM_BANK_SIZE;
		int bank = romBank % maxRomBanks;
		return rom[addr + (bank * ROM_BANK_SIZE)];
	} 
	
	// External RAM
	if (addr >= 0xA000 && addr <= 0xC000) {
		addr -= 0xA000;

		if (!externalRam || !ramEnable) {
			return 0xFF;
		}

		int bank = ramBank % maxRamBanks;
		return ram[addr + (bank * ERAM_SIZE)];
	}

	return -1;
}

int MBC3Rom::writeByte(UINT16 addr, UINT8 byte) {
	// External Ram Enable
	if (addr < 0x2000) {
		ramEnable = externalRam && (byte & 0xF) == 0xA;
		return 0;
	}
	
	// ROM Bank Select
	if (addr < 0x4000) {
		romBank = byte & 0x7F;
		if (romBank == 0) romBank = 1;
		return 0;
	}
	
	// RAM/RTC Bank Select
	if (addr < 0x6000) {
		ramBank = byte & 0x7;
		return 0;
	}
	
	// Real time clock latching
	if (addr < 0x8000) {
		debugPrint("Clock Latch!\n");
		return 0;
	}

	// External RAM
	if (addr >= 0xA000 && addr <= 0xC000) {
		if (ramEnable) {
			ram[addr - 0xA000] = byte;
		}

		// Always consume even if RAM is not enabled
		return 0;
	}

	return -1;
}

const std::string MBC3Rom::getType() {
	std::string ret = "MBC1";
	if (externalRam) ret += " + RAM";
	if (externalBatt) ret += " + BATT";
	return ret;
}