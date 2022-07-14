#include <shared.h>
#include <registers.h>
#include <instructions.h>

int getDelay(struct emuState *state, UWord word) {
	UByte reg = (UByte) ((word & 0xF00) >> 8);
	UByte timer = (UByte) state->delayTimer;
	writeRegister(state, reg, timer);
	return 0;
}

int setDelayTimer(struct emuState *state, UWord word) {
	UByte reg = (UByte) ((word & 0xF00) >> 8);
	state->delayTimer = readRegister(state, reg);
	return 0;
}

int setSoundTimer(struct emuState *state, UWord word) {
	UByte reg = (UByte) ((word & 0xF00) >> 8);
	state->soundTimer = readRegister(state, reg);
	return 0;
}
