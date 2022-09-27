#include <shared.h>
#include <memory.h>
#include <font.h>

#include <stdlib.h>

UByte c8_readMemoryByte(c8_state_t *state, UWord position) {
	return state->memory[position];
}
SByte c8_readMemorySByte(c8_state_t *state, UWord position) {
	return (SByte) state->memory[position];
}

UWord c8_readMemoryWord(c8_state_t *state, UWord position) {
	UWord out = state->memory[position + 1];
	out |= (state->memory[position] << 8);
	return out;
}
SWord c8_readMemorySWord(c8_state_t *state, UWord position) {
	UWord out = state->memory[position + 1];
	out |= (state->memory[position] << 8);
	return (SWord) out;
}

void c8_writeMemoryByte(c8_state_t *state, UWord position, UByte value) {
	state->memory[position] = value & 0xFF;
}

void c8_writeMemoryWord(c8_state_t *state, UWord position, UWord value) {
	state->memory[position] = value & 0xFF;
	state->memory[position + 1] = (value >> 8) & 0xFF;
}

void c8_clearMemory(c8_state_t *state) {
	memset(state->memory, 0, sizeof(UByte) * 0x1000);
	c8_initializeFont(state);
}
