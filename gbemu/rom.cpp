#include "gbrom.h"

// No MBC

Rom::Rom(UINT8* rom, size_t romSize) : rom(rom), romSize(romSize) {
	hdr = reinterpret_cast<CartHeader*>(rom + 0x100);
	if (hdr->romSize < 0x9) {
		maxRomBanks = (int)std::pow(2, hdr->romSize + 1);
	}

	switch (hdr->ramSize) {
	case 0x0: externalRam = false; break;
	case 0x2: maxRamBanks = 1; break;
	case 0x3: maxRamBanks = 4; break;
	case 0x4: maxRamBanks = 16; break;
	case 0x5: maxRamBanks = 8; break;
	}

	ram = new UINT8[maxRamBanks * ERAM_SIZE];
}

Rom::~Rom() {
	delete rom;
	delete ram;
}

int Rom::readByte(UINT16 addr) {
	if (addr < 0x8000) {
		return rom[addr & 0x7FFF];
	}

	if (externalRam && addr >= 0xA000 && addr < 0xC000) {
		return ram[addr - 0xA000];
	}

	return -1;
}

int Rom::writeByte(UINT16 addr, UINT8 byte) {
	if (addr < 0x8000) {
		// While we do nothing here, it's still within the ROM range
		// and thus still consumes the byte
		return 0;
	}

	if (externalRam && addr >= 0xA000 && addr < 0xC000) {
		ram[addr - 0xA000] = byte;
		return 0;
	}

	return -1;
}

const std::string Rom::getType() {
	if (!externalBatt && !externalRam) return "ROM Only";
	if (externalBatt) return "ROM + RAM + BATT";
	return "ROM + RAM";
}

const CartHeader* Rom::getHeader() {
	return hdr;
}

bool checksum(UINT8* rom) {
	UINT8 x = 0;
	for (int i = 0x134; i < 0x14D; i++) {
		x += ~rom[i];
	}

	return x == rom[0x014D];
}

Rom* createRom(UINT8* rom, size_t romSize) {
	if (romSize < 0x200) {
		std::string s("ROM too small");
		displayPopup(s, nullptr);
		return nullptr;
	}

	if (!checksum(rom)) {
		std::string s("Invalid ROM checksum");
		displayPopup(s, nullptr);
		return nullptr;
	}

	CartHeader* hdr = reinterpret_cast<CartHeader*>(rom + 0x100);
	Rom* romObj = nullptr;
	switch (hdr->cartType) {
	case ROM_ONLY:
		romObj = new Rom(rom, romSize);
		break;
	case MBC1:
	case MBC1_RAM:
	case MBC1_RAM_BATT:
		romObj = new MBC1Rom(rom, romSize);
		break;
	case MBC3:
	case MBC3_RAM:
	case MBC3_RAM_BATT:
	case MBC3_TIMER_BATT:
	case MBC3_TIMER_BATT_RAM:
		romObj = new MBC3Rom(rom, romSize);
		break;
	default: 
		std::string s("Invalid ROM type");
		displayPopup(s, nullptr);
		return nullptr;
	}

	return romObj;
}