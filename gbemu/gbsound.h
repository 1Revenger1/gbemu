#pragma once
#include "gbspace.h"

class gbSound : public gbSpace {
	UINT8 mem[0xFF40 - 0xFF10];

public:
	virtual int writeByte(UINT16 addr, UINT8 byte) override;
	virtual int readByte(UINT16 addr) override;
};