#include <shared.h>
#include <memory.h>
#include <rom.h>

int loadROM(struct emuState *state, const char *rom, long size) {
	if (size > 0xE00)
		return 1;

	for (long i = 0; i < size; i++)
		writeMemoryByte(state, i + 0x200, rom[i]);
	for (long i = size; i < 0xE00; i++)
		writeMemoryByte(state, i + 0x200, 0);
	return 0;
}
