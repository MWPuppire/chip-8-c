#include <stdlib.h>

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
	writeRegister(state, reg, rand() & val);
	return 0;
}
