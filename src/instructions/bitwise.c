#include <shared.h>
#include <registers.h>
#include <instructions.h>

int orRegister(struct emuState *state, UWord word) {
	UByte regX = ((word & 0xF00) >> 8);
	UByte regY = ((word & 0x0F0) >> 8);
	UByte valueX = readRegister(state, regX);
	UByte valueY = readRegister(state, regY);
	writeRegister(state, regX, valueX | valueY);
	return 0;
}

int andRegister(struct emuState *state, UWord word) {
	UByte regX = ((word & 0xF00) >> 8);
	UByte regY = ((word & 0x0F0) >> 8);
	UByte valueX = readRegister(state, regX);
	UByte valueY = readRegister(state, regY);
	writeRegister(state, regX, valueX & valueY);
	return 0;
}

int xorRegister(struct emuState *state, UWord word) {
	UByte regX = ((word & 0xF00) >> 8);
	UByte regY = ((word & 0x0F0) >> 8);
	UByte valueX = readRegister(state, regX);
	UByte valueY = readRegister(state, regY);
	writeRegister(state, regX, valueX ^ valueY);
	return 0;
}

int shiftRegisterRight(struct emuState *state, UWord word) {
	UByte reg = ((word & 0xF00) >> 8);
	UByte value = readRegister(state, reg);
	writeRegister(state, reg, value >> 1);
	state->registers.vF = value & 1;
	return 0;
}

int shiftRegisterLeft(struct emuState *state, UWord word) {
	UByte reg = ((word & 0xF00) >> 8);
	UByte value = readRegister(state, reg);
	writeRegister(state, reg, value << 1);
	state->registers.vF = value >> 7;
	return 0;
}
