#include "gbtimer.h"
#include "gb.h"

int gbTimer::writeByte(UINT16 addr, UINT8 val) {
	switch (addr) {
	case 0xFF04: div = 0; break;
	case 0xFF05: tima = val; break;
	case 0xFF06: tma = val; break;
	case 0xFF07: tac = val; break;
	default: return -1;
	}
	
	return 0;
}

int gbTimer::readByte(UINT16 addr) {
	switch (addr) {
	case 0xFF04: return div >> 8;
	case 0xFF05: return tima;
	case 0xFF06: return tma;
	case 0xFF07: return tac;
	default: return -1;
	}
}

void gbTimer::step() {
	int overflow_bit = freqToBit[tac & 0x3];
	int overflow_mask = 1 << overflow_bit;

	// Detect falling edge to increment tima
	int before = div & overflow_mask;
	div += 4;
	int after = div & overflow_mask;

	if (overflow) {
		overflow = false;
		g_gb->cpu->interruptFlags |= TIMER_INTR;
		tima = tma;
	}

	if ((tac & 0x4) == 0 || (before - after != overflow_mask)) {
		return;
	}

	tima++;
	if (tima == 0) {
		overflow = true;
	}
}