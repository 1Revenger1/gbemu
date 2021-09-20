// Source of information from: https://gbdev.io/pandocs/CPU_Registers_and_Flags.html

#pragma once
#include <windows.h>

#define REGISTER(name, high, low) \
	union {					\
		struct {			\
			CHAR (low);		\
			CHAR (high);	\
		};					\
		SHORT name;			\
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

struct gbCpu {
	REGISTER(AF, A, F);	// Accumulator & Flags
	REGISTER(BC, B, C);
	REGISTER(DE, D, E);
	REGISTER(HL, H, L);
	SHORT SP;	// Stack Pointer
	SHORT PC;	// Program Counter
};