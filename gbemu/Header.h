#pragma once

#include "gb.h"
#include "gbcpu.h"

void nop() {}

// Load value pointed to by HL
void aHL_read() {
	g_cpu.low = readByte(g_cpu.HL);
}