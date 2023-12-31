#include <string.h>

#include <shared-internal.h>
#include <memory.h>

const UByte C8_FONT_SET[80] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

UByte c8_readMemoryByte(const c8_state_t *state, UWord position) {
	return state->memory[position];
}
SByte c8_readMemorySByte(const c8_state_t *state, UWord position) {
	return (SByte) state->memory[position];
}

UWord c8_readMemoryWord(const c8_state_t *state, UWord position) {
	UWord out = state->memory[position + 1];
	out |= (state->memory[position] << 8);
	return out;
}
SWord c8_readMemorySWord(const c8_state_t *state, UWord position) {
	UWord out = state->memory[position + 1];
	out |= (state->memory[position] << 8);
	return (SWord) out;
}

void c8_writeMemoryByte(c8_state_t *state, UWord position, UByte value) {
	state->memory[position] = value & 0xFF;
}

void c8_writeMemoryWord(c8_state_t *state, UWord position, UWord value) {
	state->memory[position + 1] = value & 0xFF;
	state->memory[position] = (value >> 8) & 0xFF;
}

void c8_clearMemory(c8_state_t *state) {
	memset(state->memory, 0, sizeof(UByte) * 0x1000);
	memcpy(state->memory + 0x50, C8_FONT_SET, sizeof(UByte) * 80);
}

c8_status_t c8_loadROM(c8_state_t *state, const unsigned char *rom, size_t size) {
	if (size > 0xE00)
		return C8_INVALID_FILE;

	for (size_t i = 0; i < size; i++)
		c8_writeMemoryByte(state, i + 0x200, rom[i]);
	for (size_t i = size; i < 0xE00; i++)
		c8_writeMemoryByte(state, i + 0x200, 0);
	return C8_OK;
}

void c8_dumpMemory(const c8_state_t *state, unsigned char *buf, size_t size) {
	if (buf == NULL || size == 0)
		return;
	memcpy(buf, state->memory, size);
}

void c8_loadMemory(c8_state_t *state, const unsigned char *buf, size_t size) {
	if (buf == NULL || size == 0)
		return;
	memcpy(state->memory, buf, size);
}
