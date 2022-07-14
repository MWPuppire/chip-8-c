#include <shared.h>
#include <memory.h>
#include <font.h>

#include <stdlib.h>

UByte readMemoryByte(struct emuState *state, UWord position) {
	return state->memory[position];
}
SByte readMemorySByte(struct emuState *state, UWord position) {
	return (SByte) state->memory[position];
}

UWord readMemoryWord(struct emuState *state, UWord position) {
	UWord out = state->memory[position + 1];
	out |= (state->memory[position] << 8);
	return out;
}
SWord readMemorySWord(struct emuState *state, UWord position) {
	UWord out = state->memory[position + 1];
	out |= (state->memory[position] << 8);
	return (SWord) out;
}

void writeMemoryByte(struct emuState *state, UWord position, UByte value) {
	state->memory[position] = value & 0xFF;
}

void writeMemoryWord(struct emuState *state, UWord position, UWord value) {
	state->memory[position] = value & 0xFF;
	state->memory[position + 1] = (value >> 8) & 0xFF;
}

void clearMemory(struct emuState *state) {
	memset(state->memory, 0, 0x1000);
	initializeFont(state);
}
