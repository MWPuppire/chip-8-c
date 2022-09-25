#include <shared.h>
#include <registers.h>
#include <instructions.h>

int pluseqImmediate(struct emuState *state, UWord word) {
	UByte value = (UByte) (word & 0xFF);
	UByte reg = (UByte) ((word & 0xF00) >> 8);
	UByte initial = readRegister(state, reg);
	writeRegister(state, reg, initial + value);
	return 0;
}

int pluseqRegister(struct emuState *state, UWord word) {
	UByte regX = (UByte) ((word & 0xF00) >> 8);
	UByte regY = (UByte) ((word & 0x0F0) >> 4);
	UByte valueX = readRegister(state, regX);
	UByte valueY = readRegister(state, regY);
	writeRegister(state, regX, valueX + valueY);
	if (((valueX + valueY) & 0xFF) < valueX)
		state->registers.vF = 1;
	else
		state->registers.vF = 0;
	return 0;
}

int minuseqRegister(struct emuState *state, UWord word) {
	UByte regX = (UByte) ((word & 0xF00) >> 8);
	UByte regY = (UByte) ((word & 0x0F0) >> 4);
	UByte valueX = readRegister(state, regX);
	UByte valueY = readRegister(state, regY);
	writeRegister(state, regX, valueX - valueY);
	if (((valueX - valueY) & 0xFF) < valueX)
		state->registers.vF = 1;
	else
		state->registers.vF = 0;
	return 0;
}

int subtractionRegister(struct emuState *state, UWord word) {
	UByte regX = (UByte) ((word & 0xF00) >> 8);
	UByte regY = (UByte) ((word & 0x0F0) >> 4);
	UByte valueX = readRegister(state, regX);
	UByte valueY = readRegister(state, regY);
	writeRegister(state, regX, valueY - valueX);
	if (valueY > valueX)
		state->registers.vF = 1;
	else
		state->registers.vF = 0;
	return 0;
}

int addToI(struct emuState *state, UWord word) {
	UByte reg = (UByte) ((word & 0xF00) >> 8);
	UByte value = readRegister(state, reg);
	state->registers.I += value;
	if (state->registers.I > 0xFFF)
		state->registers.vF = 1;
	else
		state->registers.vF = 0;
	return 0;
}
