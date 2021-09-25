// Source of information from: https://gbdev.io/pandocs/CPU_Registers_and_Flags.html

#pragma once
#include <windows.h>

#define REGISTER(name, high, low) \
	union {					\
		struct {			\
			UINT8 low;		\
			UINT8 high;		\
		};					\
		UINT16 name;			\
	}

#define BIT(n) (1 << (n))

// Result is zero
#define ZERO_FLAG		BIT(7)
/*
* 8 bit addition > $FF
* 16 bit addition > $FFFF
* subtraction or comparison is < $0
* When a rotate/shift operation shifts out a high bit
*/
#define CARRY_FLAG		BIT(4)

// BCD Flags

// Previous instruction was a subtraction
#define SUBTRACT_FLAG	BIT(6)
//Carry for the lower 4 bits of result (CARRY_FLAG is for higher 8 bits)
#define HALF_CARRY_FLAG BIT(5)

#define ALL_FLAGS ZERO_FLAG | CARRY_FLAG | SUBTRACT_FLAG | HALF_CARRY_FLAG

// interrupt enable flags
#define VBLANK_INTR		BIT(0)
#define LCDSTAT_INTR	BIT(1)
#define TIMER_INTR		BIT(2)
#define SERIAL_INTR		BIT(3)
#define JOYPAD_INTR		BIT(4)

#define ALL_INTR_FLAGS VBLANK_INTR | LCDSTAT_INTR | TIMER_INTR | SERIAL_INTR | JOYPAD_INTR

struct gbCpu {
	// Accumulator & Flags
	struct {
		union {
			UINT8 F;
			struct {
				UINT8 _ : 4;
				UINT8 carry : 1;
				UINT8 halfCarry : 1;
				UINT8 subtract : 1;
				UINT8 zero : 1;
			};
		};
		UINT8 A;
	};

	REGISTER(BC, B, C);
	REGISTER(DE, D, E);
	REGISTER(HL, H, L);
	UINT16 SP;	// Stack Pointer
	UINT16 PC;	// Program Counter

	bool interruptEnable{ true };
	UINT8 interrupts{ ALL_INTR_FLAGS };
};