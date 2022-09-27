#include <shared.h>
#include <memory.h>
#include <rom.h>

c8_status_t c8_loadROM(c8_state_t *state, const unsigned char *rom, long size) {
	if (size > 0xE00)
		return C8_INVALID_FILE;

	for (long i = 0; i < size; i++)
		c8_writeMemoryByte(state, i + 0x200, rom[i]);
	for (long i = size; i < 0xE00; i++)
		c8_writeMemoryByte(state, i + 0x200, 0);
	return C8_OK;
}
