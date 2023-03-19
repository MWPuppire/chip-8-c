#include <shared-internal.h>
#include <registers.h>
#include <instructions.h>

int c8_pluseqImmediate(c8_state_t *state, UWord word) {
	UByte value = (UByte) (word & 0xFF);
	c8_register_t reg = (c8_register_t) ((word >> 8) & 0xF);
	UByte initial = c8_readRegister(state, reg);
	c8_writeRegister(state, reg, initial + value);
	return 0;
}

int c8_pluseqRegister(c8_state_t *state, UWord word) {
	c8_register_t regX = (c8_register_t) ((word >> 8) & 0xF);
	c8_register_t regY = (c8_register_t) ((word >> 4) & 0xF);
	UByte valueX = c8_readRegister(state, regX);
	UByte valueY = c8_readRegister(state, regY);
	c8_writeRegister(state, regX, valueX + valueY);
	if (((valueX + valueY) & 0xFF) < valueX)
		state->registers.vF = 1;
	else
		state->registers.vF = 0;
	return 0;
}

int c8_minuseqRegister(c8_state_t *state, UWord word) {
	c8_register_t regX = (c8_register_t) ((word >> 8) & 0xF);
	c8_register_t regY = (c8_register_t) ((word >> 4) & 0xF);
	UByte valueX = c8_readRegister(state, regX);
	UByte valueY = c8_readRegister(state, regY);
	c8_writeRegister(state, regX, valueX - valueY);
	if (((valueX - valueY) & 0xFF) < valueX)
		state->registers.vF = 1;
	else
		state->registers.vF = 0;
	return 0;
}

int c8_subtractionRegister(c8_state_t *state, UWord word) {
	c8_register_t regX = (c8_register_t) ((word >> 8) & 0xF);
	c8_register_t regY = (c8_register_t) ((word >> 4) & 0xF);
	UByte valueX = c8_readRegister(state, regX);
	UByte valueY = c8_readRegister(state, regY);
	c8_writeRegister(state, regX, valueY - valueX);
	if (valueY > valueX)
		state->registers.vF = 1;
	else
		state->registers.vF = 0;
	return 0;
}

int c8_addToI(c8_state_t *state, UWord word) {
	c8_register_t reg = (c8_register_t) ((word >> 8) & 0xF);
	UByte value = c8_readRegister(state, reg);
	state->registers.I += value;
	return 0;
}
