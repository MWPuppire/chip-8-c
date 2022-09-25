#include <shared.h>
#include <registers.h>
#include <memory.h>
#include <font.h>
#include <instructions.h>

int setRegister(struct emuState *state, UWord word) {
	UByte value = (UByte) (word & 0xFF);
	UByte reg = (UByte) ((word & 0xF00) >> 8);
	writeRegister(state, reg, value);
	return 0;
}

int setIRegister(struct emuState *state, UWord word) {
	UWord value = word & 0xFFF;
	state->registers.I = value;
	return 0;
}

int regDump(struct emuState *state, UWord word) {
	UWord addr = state->registers.I;
	int max = ((word >> 8) & 0xF) + 1;
	for (int i = 0; i < max; i++) {
		UByte reg = readRegister(state, i);
		writeMemoryByte(state, addr++, reg);
	}
	return 0;
}

int regLoad(struct emuState *state, UWord word) {
	UWord addr = state->registers.I;
	int max = ((word >> 8) & 0xF) + 1;
	for (int i = 0; i < max; i++) {
		UByte byte = readMemoryByte(state, addr++);
		writeRegister(state, i, byte);
	}
	return 0;
}

int moveRegister(struct emuState *state, UWord word) {
	UByte regX = (UByte) ((word & 0xF00) >> 8);
	UByte regY = (UByte) ((word & 0x0F0) >> 4);
	UByte value = readRegister(state, regY);
	writeRegister(state, regX, value);
	return 0;
}

int spriteAddrI(struct emuState *state, UWord word) {
	UByte reg = (UByte) ((word & 0xF00) >> 8);
	UByte value = readRegister(state, reg) & 15;
	state->registers.I = spriteAddr[value];
	return 0;
}
