#include <shared.h>
#include <call.h>
#include <instructions.h>
#include <registers.h>

int c8_gotoInst(c8_state_t *state, UWord word) {
	UWord dest = word & 0xFFF;
	state->registers.pc = dest;
	return 0;
}

int c8_callInst(c8_state_t *state, UWord word) {
	UWord pos = word & 0xFFF;
	c8_callRoutine(state, pos);
	return 0;
}

int c8_returnInst(c8_state_t *state, UWord UNUSED(word)) {
	c8_returnRoutine(state);
	return 0;
}

int c8_jumpV0(c8_state_t *state, UWord word) {
	UWord value = word & 0xFFF;
#ifdef SCHIP
	c8_register_t reg = (c8_register_t) ((word >> 8) & 0xF);
	UWord offset = c8_readRegister(state, reg);
#else
	UWord offset = state->registers.v0;
#endif
	state->registers.pc = value + offset;
	return 0;
}

int c8_jumpOffset(c8_state_t *state, UWord word) {
	c8_register_t reg = (c8_register_t) ((word >> 8) & 0xF);
	UWord value = word & 0xFFF;
	UWord offset = c8_readRegister(state, reg);
	state->registers.pc = value + offset;
	return 0;
}
