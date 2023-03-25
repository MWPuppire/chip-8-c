#include <shared-internal.h>
#include <registers.h>
#include <instructions.h>

int c8_orRegister(c8_state_t *state, UWord word) {
	c8_register_t regX = (c8_register_t) ((word >> 8) & 0xF);
	c8_register_t regY = (c8_register_t) ((word >> 4) & 0xF);
	UByte valueX = c8_readRegister(state, regX);
	UByte valueY = c8_readRegister(state, regY);
	c8_writeRegister(state, regX, valueX | valueY);
#ifdef COSMAC
	state->registers.vF = 0;
#endif
	return 0;
}

int c8_andRegister(c8_state_t *state, UWord word) {
	c8_register_t regX = (c8_register_t) ((word >> 8) & 0xF);
	c8_register_t regY = (c8_register_t) ((word >> 4) & 0xF);
	UByte valueX = c8_readRegister(state, regX);
	UByte valueY = c8_readRegister(state, regY);
	c8_writeRegister(state, regX, valueX & valueY);
#ifdef COSMAC
	state->registers.vF = 0;
#endif
	return 0;
}

int c8_xorRegister(c8_state_t *state, UWord word) {
	c8_register_t regX = (c8_register_t) ((word >> 8) & 0xF);
	c8_register_t regY = (c8_register_t) ((word >> 4) & 0xF);
	UByte valueX = c8_readRegister(state, regX);
	UByte valueY = c8_readRegister(state, regY);
	c8_writeRegister(state, regX, valueX ^ valueY);
#ifdef COSMAC
	state->registers.vF = 0;
#endif
	return 0;
}

int c8_shiftRegisterRight(c8_state_t *state, UWord word) {
	c8_register_t reg = (c8_register_t) ((word >> 8) & 0xF);
#ifdef SCHIP
	UByte value = c8_readRegister(state, reg);
#else
	c8_register_t inReg = (c8_register_t) ((word >> 4) & 0xF);
	UByte value = c8_readRegister(state, inReg);
#endif
	c8_writeRegister(state, reg, value >> 1);
	state->registers.vF = value & 1;
	return 0;
}

int c8_shiftRegisterLeft(c8_state_t *state, UWord word) {
	c8_register_t reg = (c8_register_t) ((word >> 8) & 0xF);
#ifdef SCHIP
	UByte value = c8_readRegister(state, reg);
#else
	c8_register_t inReg = (c8_register_t) ((word >> 4) & 0xF);
	UByte value = c8_readRegister(state, inReg);
#endif
	c8_writeRegister(state, reg, value << 1);
	state->registers.vF = (value >> 7) & 1;
	return 0;
}
