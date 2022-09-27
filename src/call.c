#include <shared.h>
#include <call.h>

void c8_callRoutine(c8_state_t *state, UWord pos) {
	if (state->callStackPos < C8_STACK_SIZE) {
		state->callStack[state->callStackPos++] = state->registers.pc;
	}
	state->registers.pc = pos;
}

void c8_returnRoutine(c8_state_t *state) {
	if (state->callStackPos == 0)
		return;
	state->registers.pc = state->callStack[--state->callStackPos];
}
