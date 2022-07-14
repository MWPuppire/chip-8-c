#pragma once

#include "shared.h"

#define REGISTER_COUNT 18

enum registerName {
	REG_0, REG_1, REG_2, REG_3, REG_4, REG_5, REG_6, REG_7, REG_8,
	REG_9, REG_A, REG_B, REG_C, REG_D, REG_E, REG_F, REG_I, REG_PC
};
extern const char *REGISTER_NAMES[REGISTER_COUNT];

int registerByName(const char *name, UByte *reg);
void resetRegisters(struct emuState *state);
UByte readRegister(struct emuState *state, UByte reg);
void writeRegister(struct emuState *state, UByte reg, UByte value);
