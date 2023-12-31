#include <string.h>

#include <shared-internal.h>
#include <save-state.h>

struct c8_saved_state {
	c8_emu_mode_t mode;
	int callStackPos;
	int awaitingKey;
	UWord regI;
	UWord regPC;
	UWord randomState;
	UWord callStack[C8_STACK_SIZE];
	UByte registers[16];
	UByte delayTimer;
	UByte soundTimer;
	UByte memory[C8_ADDRESSABLE_MEM];
	UByte screen[C8_SCREEN_MEM];
	UByte registerPersistent[16];
	bool hires;
};

int c8_saveState(const c8_state_t *state, c8_saved_state_t *out) {
	if (state->exited) {
		return 1;
	}
	out->mode = state->mode;
	out->callStackPos = state->callStackPos;
	out->awaitingKey = state->awaitingKey;
	out->regI = state->regI;
	out->regPC = state->regPC;
	out->randomState = state->randomState;
	memcpy(out->callStack, state->callStack, C8_STACK_SIZE * sizeof(UWord));
	memcpy(out->registers, state->registers, 16 * sizeof(UByte));
	out->delayTimer = state->delayTimer;
	out->soundTimer = state->soundTimer;
	memcpy(out->memory, state->memory, C8_ADDRESSABLE_MEM * sizeof(UByte));
	memcpy(out->screen, state->screen, C8_SCREEN_MEM * sizeof(UByte));
	memcpy(out->registerPersistent, state->registerPersistent, 16 * sizeof(UByte));
	out->hires = state->hires;
	return 0;
}

int c8_loadState(c8_state_t *state, const c8_saved_state_t *save) {
	if (state->mode != save->mode) {
		return 1;
	}
	state->exited = false;
	state->cycleDiff = 0.0;
	state->timerDiff = 0.0;
	state->vblankDiff = 0.0;
	state->vblankWait = false;
	state->callStackPos = save->callStackPos;
	state->awaitingKey = save->awaitingKey;
	state->regI = save->regI;
	state->regPC = save->regPC;
	state->randomState = save->randomState;
	memcpy(state->callStack, save->callStack, C8_STACK_SIZE * sizeof(UWord));
	memcpy(state->registers, save->registers, 16 * sizeof(UByte));
	state->delayTimer = save->delayTimer;
	state->soundTimer = save->soundTimer;
	memcpy(state->memory, save->memory, C8_ADDRESSABLE_MEM * sizeof(UByte));
	memcpy(state->screen, save->screen, C8_SCREEN_MEM * sizeof(UByte));
	memcpy(state->registerPersistent, save->registerPersistent, 16 * sizeof(UByte));
	state->hires = save->hires;
	return 0;
}
