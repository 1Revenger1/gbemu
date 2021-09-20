#pragma once
#include "Windows.h"

#define NO_MBC			0x00

#define MBC1			0x01
#define MBC1_RAM		0x02
#define MBC1_RAM_BAT	0x03

#define MBC2			0x05
#define MBC2_BAT		0x06

#define NO_MBC_RAM		0x08
#define NO_MBC_RAM_BATT	0x09

#define MMM01			0x0B
#define MMM01_RAM		0x0C
#define MMM01_RAM_BATT	0x0D

#define MBC3_TIMER_BATT		0x0F
#define MBC3_TIMER_BATT_RAM 0x10
#define MBC3				0x11
#define MBC3_RAM			0x12
#define MBC3_RAM_BATT		0x13

#define MBC5					0x19
#define MBC5_RAM				0x1A
#define MBC5_RAM_BATT			0x1B
#define MBC5_RUMBLE				0x1C
#define MBC5_RUMBLE_RAM			0x1D
#define MBC5_RUMBLE_RAM_BATT	0x1E

#define MBC6			0x20

#define MBC7_SENSOR_RUMBLE_RAM_BATT	0x22

#define HuC3			0xFE
#define HuC1_RAM_BATT	0xFF

//CHAR readMBC1(Gb* gb);