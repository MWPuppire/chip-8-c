#include <shared-internal.h>
#include <registers.h>
#include <memory.h>
#include <instructions.h>

static inline void c8_skipInstruction(c8_state_t *state) {
#ifdef XO_CHIP
	// handle the double-word instruction `0xF000` correctly
	if (c8_readMemoryWord(state, state->registers.pc) == 0xF000) {
		state->registers.pc += 4;
	} else {
		state->registers.pc += 2;
	}
#else
	state->registers.pc += 2;
#endif
}

int c8_ifEquals(c8_state_t *state, UWord word) {
	UByte value = (UByte) (word & 0xFF);
	c8_register_t reg = (c8_register_t) ((word >> 8) & 0xF);
	UByte regValue = c8_readRegister(state, reg);
	if (value == regValue)
		c8_skipInstruction(state);
	return 0;
}

int c8_ifNotEquals(c8_state_t *state, UWord word) {
	UByte value = (UByte) (word & 0xFF);
	c8_register_t reg = (c8_register_t) ((word >> 8) & 0xF);
	UByte regValue = c8_readRegister(state, reg);
	if (value != regValue)
		c8_skipInstruction(state);
	return 0;
}

int c8_ifEqualsReg(c8_state_t *state, UWord word) {
	c8_register_t regX = (c8_register_t) ((word >> 8) & 0xF);
	c8_register_t regY = (c8_register_t) ((word >> 4) & 0xF);
	UByte regXValue = c8_readRegister(state, regX);
	UByte regYValue = c8_readRegister(state, regY);
	if (regXValue == regYValue)
		c8_skipInstruction(state);
	return 0;
}

int c8_ifNotEqualsReg(c8_state_t *state, UWord word) {
	c8_register_t regX = (c8_register_t) ((word >> 8) & 0xF);
	c8_register_t regY = (c8_register_t) ((word >> 4) & 0xF);
	UByte regXValue = c8_readRegister(state, regX);
	UByte regYValue = c8_readRegister(state, regY);
	if (regXValue != regYValue)
		c8_skipInstruction(state);
	return 0;
}

int c8_keyEquals(c8_state_t *state, UWord word) {
	c8_register_t reg = (c8_register_t) ((word >> 8) & 0xF);
	int key = (int) c8_readRegister(state, reg);
	if (state->input[key])
		c8_skipInstruction(state);
	return 0;
}

int c8_keyNotEquals(c8_state_t *state, UWord word) {
	c8_register_t reg = (c8_register_t) ((word >> 8) & 0xF);
	int key = (int) c8_readRegister(state, reg);
	if (!state->input[key])
		c8_skipInstruction(state);
	return 0;
}
