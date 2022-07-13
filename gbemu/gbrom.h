#pragma once
#include "gb.h"
#include "gbspace.h"
#include <string>
#include <cmath>

#define KB_SHIFT 4
#define MB_SHIFT 8

#define ROM_BANK_SIZE	0x4000
#define ERAM_SIZE		0x2000

struct CartHeader {
	UINT8 entryPoint[4];	// 0x100-0x103
	UINT8 logo[48];			// 0x104-0x133
	union {
		char title[16];
		struct {
			char title[11];
			char manufacturer[4];
			UINT8 mode;
		} cgb;
	};
	UINT16 licensee;
	UINT8 superGameboy;
	UINT8 cartType;
	UINT8 romSize;
	UINT8 ramSize;
	UINT8 destCode;
	UINT8 oldLicensee;
	UINT8 version;
	UINT8 headerChecksum;
	UINT16 cartChecksum;
};

// Memory Bank Controller
enum MBCType {
	// No MBC
	ROM_ONLY = 0x00,
	// MBC1
	MBC1 = 0x01,
	MBC1_RAM = 0x02,
	MBC1_RAM_BATT = 0x03,
	// MBC2
	MBC2 = 0x05,
	MBC2_BAT = 0x06,
	// No MBC + addons
	ROM_RAM = 0x08,
	ROM_RAM_BATT = 0x09,
	// MMM - Multi game carts
	MMM01 = 0x0B,
	MMM01_RAM = 0x0C,
	MMM01_RAM_BATT = 0x0D,
	// MBC3
	MBC3_TIMER_BATT = 0x0F,
	MBC3_TIMER_BATT_RAM = 0x10,
	MBC3 = 0x11,
	MBC3_RAM = 0x12,
	MBC3_RAM_BATT = 0x13,
	// MBC5
	MBC5 = 0x19,
	MBC5_RAM = 0x1A,
	MBC5_RAM_BATT = 0x1B,
	MBC5_RUMBLE = 0x1C,
	MBC5_RUMBLE_RAM = 0x1D,
	MBC5_RUMBLE_RAM_BATT = 0x1E,
	// MBC6
	MBC6 = 0x20,
	// MBC7
	MBC7_SENSOR_RUMBLE_RAM_BATT = 0x22,
};

#define HuC3			0xFE
#define HuC1_RAM_BATT	0xFF

class Rom : public gbSpace {
protected:
	const CartHeader* hdr = nullptr;
	UINT8* rom = nullptr;

	DWORD romSize;
	int maxRomBanks = 2;

	bool externalBatt = false;
	bool externalRam = true;

	// Battery backed data
	bool ramEnable = false;
	UINT8* ram = nullptr;
	int maxRamBanks = 1;

public:
	Rom(UINT8* rom, DWORD romSize);
	~Rom();

	virtual int writeByte(UINT16 addr, UINT8 byte) override;
	virtual int readByte(UINT16 addr) override;
	virtual const std::string getType();
	const CartHeader* getHeader();

};

class MBC1Rom : public Rom {
	typedef Rom super;
private:
	enum MBC1Mode {
		ROMBanking,
		RAMBanking
	};

	MBC1Mode mode = ROMBanking;

	int romBank = 1;
	int ramBank = 0;
public:
	MBC1Rom(UINT8* rom, DWORD romSize);

	virtual int writeByte(UINT16 addr, UINT8 byte) override;
	virtual int readByte(UINT16 addr) override;

	const std::string getType();
};

class MBC3Rom : public Rom {
	typedef Rom super;
private:
	int romBank = 1;
	int ramBank = 0;
	bool externalTimer = false;

public:
	MBC3Rom(UINT8* rom, DWORD romSize);

	virtual int readByte(UINT16 addr) override;
	virtual int writeByte(UINT16 addr, UINT8 byte) override;

	const std::string getType();
};

Rom* createRom(UINT8* rom, DWORD romSize);