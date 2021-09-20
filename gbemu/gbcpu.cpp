#include "gb.h"


static bool run = true;
static bool interrupts = true;

static void load(Gb* gb, char* a, char* b)
{
	
}

static void loadAddr(Gb* gb, char* a, char* b)
{

}

static void enableInterrupts(bool newVal)
{
	interrupts = newVal;
}

void instr(Gb* gb)
{
	gbCpu &cpu = gb->cpu;
	BYTE instr = readByte(gb, cpu.PC);



}

void execute(Gb* gb)
{
	while (true) {
		instr(gb);
	}
}