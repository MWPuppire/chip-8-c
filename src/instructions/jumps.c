#include <shared-internal.h>
#include <instructions.h>
#include <registers.h>

int c8_gotoInst(c8_state_t *state, UWord word) {
	UWord dest = word & 0xFFF;
	state->regPC = dest;
	return 0;
}

int c8_callInst(c8_state_t *state, UWord word) {
	UWord pos = word & 0xFFF;
	if (state->callStackPos < C8_STACK_SIZE)
		state->callStack[state->callStackPos++] = state->regPC;
	state->regPC = pos;
	return 0;
}

int c8_returnInst(c8_state_t *state, UWord UNUSED(word)) {
	if (state->callStackPos != 0)
		state->regPC = state->callStack[--state->callStackPos];
	return 0;
}

int c8_jumpV0(c8_state_t *state, UWord word) {
	UWord value = word & 0xFFF;
#ifdef SCHIP
	c8_register_t reg = (c8_register_t) ((word >> 8) & 0xF);
	UWord offset = c8_readRegister(state, reg);
#else
	UWord offset = state->registers[0x0];
#endif
	state->regPC = value + offset;
	return 0;
}

int c8_jumpOffset(c8_state_t *state, UWord word) {
	c8_register_t reg = (c8_register_t) ((word >> 8) & 0xF);
	UWord value = word & 0xFFF;
	UWord offset = c8_readRegister(state, reg);
	state->regPC = value + offset;
	return 0;
}
