#include <shared-internal.h>
#include <registers.h>
#include <instructions.h>

int c8_getDelay(c8_state_t *state, UWord word) {
	c8_register_t reg = (c8_register_t) ((word >> 8) & 0xF);
	UByte timer = (UByte) state->delayTimer;
	c8_writeRegister(state, reg, timer);
	return 0;
}

int c8_setDelayTimer(c8_state_t *state, UWord word) {
	c8_register_t reg = (c8_register_t) ((word >> 8) & 0xF);
	state->delayTimer = c8_readRegister(state, reg);
	return 0;
}

int c8_setSoundTimer(c8_state_t *state, UWord word) {
	c8_register_t reg = (c8_register_t) ((word >> 8) & 0xF);
	state->soundTimer = c8_readRegister(state, reg);
	return 0;
}
