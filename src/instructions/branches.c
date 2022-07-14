#include <shared.h>
#include <registers.h>
#include <instructions.h>

int ifEquals(struct emuState *state, UWord word) {
	UByte value = (UByte) (word & 0xFF);
	UByte reg = (UByte) ((word & 0xF00) >> 8);
	UByte regValue = readRegister(state, reg);
	if (value == regValue)
		state->registers.pc += 2;
	return 0;
}

int ifNotEquals(struct emuState *state, UWord word) {
	UByte value = (UByte) (word & 0xFF);
	UByte reg = (UByte) ((word & 0xF00) >> 8);
	UByte regValue = readRegister(state, reg);
	if (value != regValue)
		state->registers.pc += 2;
	return 0;
}

int ifEqualsReg(struct emuState *state, UWord word) {
	UByte regX = (UByte) ((word & 0xF00) >> 8);
	UByte regY = (UByte) ((word & 0x0F0) >> 4);
	UByte regXValue = readRegister(state, regX);
	UByte regYValue = readRegister(state, regY);
	if (regXValue == regYValue)
		state->registers.pc += 2;
	return 0;
}

int ifNotEqualsReg(struct emuState *state, UWord word) {
	UByte regX = (UByte) ((word & 0xF00) >> 8);
	UByte regY = (UByte) ((word & 0x0F0) >> 4);
	UByte regXValue = readRegister(state, regX);
	UByte regYValue = readRegister(state, regY);
	if (regXValue != regYValue)
		state->registers.pc += 2;
	return 0;
}
