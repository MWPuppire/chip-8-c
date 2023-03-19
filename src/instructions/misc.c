#include <shared-internal.h>
#include <memory.h>
#include <registers.h>
#include <instructions.h>

int c8_bcd(c8_state_t *state, UWord word) {
	c8_register_t reg = (c8_register_t) ((word >> 8) & 0xF);
	UByte val = c8_readRegister(state, reg);
	c8_writeMemoryByte(state, state->registers.I + 0, val / 100);
	c8_writeMemoryByte(state, state->registers.I + 1, (val / 10) % 10);
	c8_writeMemoryByte(state, state->registers.I + 2, val % 10);
	return 0;
}

int c8_randRegister(c8_state_t *state, UWord word) {
	c8_register_t reg = (c8_register_t) ((word >> 8) & 0xF);
	UByte val = word & 0xFF;
	// Xorshift LFSR
	UWord lfsr = state->randomState;
	lfsr ^= lfsr >>  7;
	lfsr ^= lfsr <<  9;
	lfsr ^= lfsr >> 13;
	state->randomState = lfsr;
	c8_writeRegister(state, reg, lfsr & val);
	return 0;
}

int c8_nop(c8_state_t *UNUSED(state), UWord UNUSED(word)) {
	return 0;
}

int c8_getKey(c8_state_t *state, UWord word) {
	int reg = (int) ((word >> 8) & 0xF);
	state->awaitingKey = reg;
	return 0;
}

#if defined(SCHIP) || defined(XO_CHIP)
int c8_exitInst(c8_state_t *state, UWord UNUSED(word)) {
	state->exited = true;
	return 0;
}
#endif
