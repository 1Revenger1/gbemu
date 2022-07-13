#pragma once
#include <Windows.h>
#include "gbspace.h"

class gbTimer : public gbSpace {
	UINT16 div = 0;  // Always counts up
	UINT8 tima = 0;
	UINT8 tma = 0;  // Initial tima value
	UINT8 tac = 0;  // Control

	const int freqToBit[4] { 9, 3, 5, 7 };
	bool overflow = false;

public:
	virtual int writeByte(UINT16 addr, UINT8 val) override;
	virtual int readByte(UINT16 addr) override;
	void step();
};