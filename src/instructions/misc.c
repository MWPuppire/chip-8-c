#include <shared.h>
#include <memory.h>
#include <registers.h>
#include <instructions.h>

int bcd(struct emuState *state, UWord word) {
	UByte reg = (UByte) ((word & 0xF00) >> 8);
	UByte val = readRegister(state, reg);
	writeMemoryByte(state, state->registers.I + 0, val / 100);
	writeMemoryByte(state, state->registers.I + 1, (val / 10) % 10);
	writeMemoryByte(state, state->registers.I + 2, val % 10);
	return 0;
}

int randRegister(struct emuState *state, UWord word) {
	UByte reg = (UByte) ((word & 0xF00) >> 8);
	UByte val = word & 0xFF;
	// Xorshift LFSR
	UWord lfsr = state->randomState;
	lfsr ^= lfsr >>  7;
	lfsr ^= lfsr <<  9;
	lfsr ^= lfsr >> 13;
	state->randomState = lfsr;
	writeRegister(state, reg, lfsr & val);
	return 0;
}

int nop(struct emuState *UNUSED(state), UWord UNUSED(word)) {
	return 0;
}
