#include <stdlib.h>

#include <shared-internal.h>
#include <opcodes.h>
#include <registers.h>
#include <screen.h>
#include <cpu.h>
#include <input.h>
#include <memory.h>

const int C8_VERSION_MAJOR = 0;
const int C8_VERSION_MINOR = 1;
const int C8_VERSION_PATCH = 0;
const char *C8_VERSION_STRING = "0.1.0";
#ifdef SCHIP
const enum c8_comp_mode C8_COMPILED_MODE = C8_SUPER_CHIP;
#elif defined(XO_CHIP)
const enum c8_comp_mode C8_COMPILED_MODE = C8_XO_CHIP;
#else
const enum c8_comp_mode C8_COMPILED_MODE = C8_CHIP_8;
#endif
const int C8_SCREEN_WIDTH  = SCREEN_WIDTH;
const int C8_SCREEN_HEIGHT = SCREEN_HEIGHT;

#ifdef COSMAC
const double C8_VBLANK_SPEED = 60.0;
#endif

c8_state_t *c8_newState(void) {
	c8_state_t *state = (c8_state_t *) malloc(sizeof(c8_state_t));
	if (state == NULL)
		return state;
	c8_cpuBoot(state);
	return state;
}

void c8_seedRandom(c8_state_t *state, UWord seed) {
	state->randomState = seed;
}

void c8_cpuBoot(c8_state_t *state) {
	state->exited = false;
	c8_clearMemory(state);
	c8_clearScreen(state);
	c8_clearInput(state);
	c8_resetRegisters(state);
	c8_seedRandom(state, (UWord) random());
	state->awaitingKey = -1;
}

c8_status_t c8_cpuStep(c8_state_t *state, int *outCycles) {
	if (state->exited)
		return C8_EXITED;
	if (state->awaitingKey != -1)
		return C8_AWAITING_KEY;
	UWord opcode = c8_readMemoryWord(state, state->registers.pc);
	struct c8_instruction inst = { 0 };
	c8_instructionLookup(&inst, opcode);
	if (inst.execute == NULL)
		return C8_UNKNOWN_OP;
	int cycles = inst.cycles;
	state->registers.pc += 2;
	int extraCycles = inst.execute(state, opcode);
	if (outCycles != NULL)
		*outCycles = cycles + extraCycles;
	return C8_OK;
}

c8_status_t c8_emulate(c8_state_t *state, double dt, int *outCycles) {
	if (state->exited)
		return C8_EXITED;

	// Timers still tick while awaiting key input,
	// but don't execute any more instructions.
	state->timerDiff += dt * C8_TIMER_SPEED;
	UByte timerDiff = (UByte) state->timerDiff;
	state->delayTimer -= timerDiff > state->delayTimer
		? state->delayTimer : timerDiff;
	state->soundTimer -= timerDiff > state->soundTimer
		? state->soundTimer : timerDiff;
	state->timerDiff -= timerDiff;
#ifdef COSMAC
	state->vblankDiff += dt * C8_VBLANK_SPEED;
	while (state->vblankDiff > 1) {
		state->vblankWait = false;
		state->vblankDiff -= 1.0;
	}
	if (state->vblankWait)
		return C8_OK;
#endif
	if (state->awaitingKey != -1)
		return C8_AWAITING_KEY;
	state->cycleDiff += dt * C8_CLOCK_SPEED;

	int totalCycles = 0;
	while (state->cycleDiff > 0) {
		int cyclesTaken;
		c8_status_t status = c8_cpuStep(state, &cyclesTaken);
		if (status != C8_OK) {
			outCycles != NULL && (*outCycles = totalCycles);
			return status;
		}
		state->cycleDiff -= cyclesTaken;
		totalCycles += cyclesTaken;
#ifdef COSMAC
		if (state->vblankWait)
			break;
#endif
		if (state->awaitingKey != -1)
			break;
	}
	if (outCycles != NULL)
		*outCycles = totalCycles;
	return state->awaitingKey == -1 ? C8_OK : C8_AWAITING_KEY;
}

c8_status_t c8_emulateUntil(c8_state_t *state, double dt, int *outCycles, int *breakpoints, int n) {
	if (state->exited)
		return C8_EXITED;

	state->timerDiff += dt * C8_TIMER_SPEED;
	UByte timerDiff = (UByte) state->timerDiff;
	state->delayTimer -= timerDiff > state->delayTimer
		? state->delayTimer : timerDiff;
	state->soundTimer -= timerDiff > state->soundTimer
		? state->soundTimer : timerDiff;
	state->timerDiff -= timerDiff;
#ifdef COSMAC
	state->vblankDiff += dt * C8_VBLANK_SPEED;
	while (state->vblankDiff > 1) {
		state->vblankWait = false;
		state->vblankDiff -= 1.0;
	}
	if (state->vblankWait)
		return C8_OK;
#endif
	if (state->awaitingKey != -1)
		return C8_AWAITING_KEY;
	state->cycleDiff += dt * C8_CLOCK_SPEED;

	int totalCycles = 0;
	while (state->cycleDiff > 0) {
		int cyclesTaken;
		c8_status_t status = c8_cpuStep(state, &cyclesTaken);
		if (status != C8_OK) {
			outCycles != NULL && (*outCycles = totalCycles);
			return status;
		}
		state->cycleDiff -= cyclesTaken;
		totalCycles += cyclesTaken;
		for (int i = 0; i < n; i++) {
			if (state->registers.pc == breakpoints[i]) {
				outCycles != NULL && (*outCycles = totalCycles);
				return C8_BREAK;
			}
		}
#ifdef COSMAC
		if (state->vblankWait)
			break;
#endif
		if (state->awaitingKey != -1)
			break;
	}
	if (outCycles != NULL)
		*outCycles = totalCycles;
	return state->awaitingKey == -1 ? C8_OK : C8_AWAITING_KEY;
}

bool c8_shouldBeep(c8_state_t *state) {
	return !state->exited && state->soundTimer > 0;
}

UWord c8_delayTimer(c8_state_t *state) {
	return state->delayTimer;
}
UWord c8_soundTimer(c8_state_t *state) {
	return state->soundTimer;
}

size_t c8_callStack(c8_state_t *state, UWord *frames, size_t frameSize) {
	size_t totalFrames = (size_t) state->callStackPos;
	size_t framesWritten = 0;
	for (size_t i = totalFrames - 1; i >= 0; i--) {
		if (framesWritten == frameSize)
			break;
		frames[framesWritten++] = state->callStack[i];
	}
	return totalFrames;
}
