#include <shared.h>
#include <call.h>

void callRoutine(struct emuState *state, UWord pos) {
	if (state->callStackPos < STACK_SIZE) {
		state->callStack[state->callStackPos++] = state->registers.pc;
	}
	state->registers.pc = pos;
}

void returnRoutine(struct emuState *state) {
	if (state->callStackPos == 0)
		return;
	state->registers.pc = state->callStack[--state->callStackPos];
}
