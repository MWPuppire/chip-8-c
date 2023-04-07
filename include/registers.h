#pragma once

#include "shared.h"

#define C8_REGISTER_COUNT 16

typedef enum {
	C8_REG_0, C8_REG_1, C8_REG_2, C8_REG_3, C8_REG_4, C8_REG_5, C8_REG_6,
	C8_REG_7, C8_REG_8, C8_REG_9, C8_REG_A, C8_REG_B, C8_REG_C, C8_REG_D,
	C8_REG_E, C8_REG_F
} c8_register_t;
extern const char *REGISTER_NAMES[C8_REGISTER_COUNT];

int c8_registerByName(const char *name, c8_register_t *reg);
void c8_resetRegisters(c8_state_t *state);
UByte c8_readRegister(c8_state_t *state, c8_register_t reg);
void c8_writeRegister(c8_state_t *state, c8_register_t reg, UByte value);
UWord c8_readAddressRegister(c8_state_t *state);
void c8_writeAddressRegister(c8_state_t *state, UWord value);
UWord c8_readPC(c8_state_t *state);
void c8_writePC(c8_state_t *state, UWord value);
