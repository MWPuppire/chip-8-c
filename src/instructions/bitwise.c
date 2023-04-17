#include <shared-internal.h>
#include <registers.h>
#include <instructions.h>

int c8_orRegister(c8_state_t *state, UWord word) {
	c8_register_t regX = (c8_register_t) ((word >> 8) & 0xF);
	c8_register_t regY = (c8_register_t) ((word >> 4) & 0xF);
	UByte valueX = c8_readRegister(state, regX);
	UByte valueY = c8_readRegister(state, regY);
	c8_writeRegister(state, regX, valueX | valueY);
	return 0;
}

int c8_cosmacOrRegister(c8_state_t *state, UWord word) {
	c8_register_t regX = (c8_register_t) ((word >> 8) & 0xF);
	c8_register_t regY = (c8_register_t) ((word >> 4) & 0xF);
	UByte valueX = c8_readRegister(state, regX);
	UByte valueY = c8_readRegister(state, regY);
	c8_writeRegister(state, regX, valueX | valueY);
	state->registers[0xF] = 0;
	return 0;
}

int c8_andRegister(c8_state_t *state, UWord word) {
	c8_register_t regX = (c8_register_t) ((word >> 8) & 0xF);
	c8_register_t regY = (c8_register_t) ((word >> 4) & 0xF);
	UByte valueX = c8_readRegister(state, regX);
	UByte valueY = c8_readRegister(state, regY);
	c8_writeRegister(state, regX, valueX & valueY);
	return 0;
}

int c8_cosmacAndRegister(c8_state_t *state, UWord word) {
	c8_register_t regX = (c8_register_t) ((word >> 8) & 0xF);
	c8_register_t regY = (c8_register_t) ((word >> 4) & 0xF);
	UByte valueX = c8_readRegister(state, regX);
	UByte valueY = c8_readRegister(state, regY);
	c8_writeRegister(state, regX, valueX & valueY);
	state->registers[0xF] = 0;
	return 0;
}

int c8_xorRegister(c8_state_t *state, UWord word) {
	c8_register_t regX = (c8_register_t) ((word >> 8) & 0xF);
	c8_register_t regY = (c8_register_t) ((word >> 4) & 0xF);
	UByte valueX = c8_readRegister(state, regX);
	UByte valueY = c8_readRegister(state, regY);
	c8_writeRegister(state, regX, valueX ^ valueY);
	return 0;
}

int c8_cosmacXorRegister(c8_state_t *state, UWord word) {
	c8_register_t regX = (c8_register_t) ((word >> 8) & 0xF);
	c8_register_t regY = (c8_register_t) ((word >> 4) & 0xF);
	UByte valueX = c8_readRegister(state, regX);
	UByte valueY = c8_readRegister(state, regY);
	c8_writeRegister(state, regX, valueX ^ valueY);
	state->registers[0xF] = 0;
	return 0;
}

int c8_shiftRegisterRight(c8_state_t *state, UWord word) {
	c8_register_t reg = (c8_register_t) ((word >> 8) & 0xF);
	c8_register_t inReg = (c8_register_t) ((word >> 4) & 0xF);
	UByte value = c8_readRegister(state, inReg);
	c8_writeRegister(state, reg, value >> 1);
	state->registers[0xF] = value & 1;
	return 0;
}

int c8_schipShiftRegisterRight(c8_state_t *state, UWord word) {
	c8_register_t reg = (c8_register_t) ((word >> 8) & 0xF);
	UByte value = c8_readRegister(state, reg);
	c8_writeRegister(state, reg, value >> 1);
	state->registers[0xF] = value & 1;
	return 0;
}

int c8_shiftRegisterLeft(c8_state_t *state, UWord word) {
	c8_register_t reg = (c8_register_t) ((word >> 8) & 0xF);
	c8_register_t inReg = (c8_register_t) ((word >> 4) & 0xF);
	UByte value = c8_readRegister(state, inReg);
	c8_writeRegister(state, reg, value << 1);
	state->registers[0xF] = (value >> 7) & 1;
	return 0;
}

int c8_schipShiftRegisterLeft(c8_state_t *state, UWord word) {
	c8_register_t reg = (c8_register_t) ((word >> 8) & 0xF);
	UByte value = c8_readRegister(state, reg);
	c8_writeRegister(state, reg, value << 1);
	state->registers[0xF] = (value >> 7) & 1;
	return 0;
}
