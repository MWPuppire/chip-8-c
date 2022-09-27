#include <shared.h>
#include <registers.h>
#include <instructions.h>

int c8_ifEquals(c8_state_t *state, UWord word) {
	UByte value = (UByte) (word & 0xFF);
	c8_register_t reg = (c8_register_t) ((word >> 8) & 0xF);
	UByte regValue = c8_readRegister(state, reg);
	if (value == regValue)
		state->registers.pc += 2;
	return 0;
}

int c8_ifNotEquals(c8_state_t *state, UWord word) {
	UByte value = (UByte) (word & 0xFF);
	c8_register_t reg = (c8_register_t) ((word >> 8) & 0xF);
	UByte regValue = c8_readRegister(state, reg);
	if (value != regValue)
		state->registers.pc += 2;
	return 0;
}

int c8_ifEqualsReg(c8_state_t *state, UWord word) {
	c8_register_t regX = (c8_register_t) ((word >> 8) & 0xF);
	c8_register_t regY = (c8_register_t) ((word >> 4) & 0xF);
	UByte regXValue = c8_readRegister(state, regX);
	UByte regYValue = c8_readRegister(state, regY);
	if (regXValue == regYValue)
		state->registers.pc += 2;
	return 0;
}

int c8_ifNotEqualsReg(c8_state_t *state, UWord word) {
	c8_register_t regX = (c8_register_t) ((word >> 8) & 0xF);
	c8_register_t regY = (c8_register_t) ((word >> 4) & 0xF);
	UByte regXValue = c8_readRegister(state, regX);
	UByte regYValue = c8_readRegister(state, regY);
	if (regXValue != regYValue)
		state->registers.pc += 2;
	return 0;
}
