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

const char *C8_MODE_NAMES[3] = {
	"COSMAC", "Super CHIP-8", "XO-CHIP"
};

c8_state_t *c8_newState(c8_emu_mode_t mode) {
	if (mode > 2 || mode < 0)
		return NULL;
	c8_state_t *state = (c8_state_t *) malloc(sizeof(c8_state_t));
	if (state == NULL)
		return state;
	state->mode = mode;
	c8_cpuBoot(state);
	return state;
}

void c8_seedRandom(c8_state_t *state, UWord seed) {
	state->randomState = seed;
}

void c8_cpuBoot(c8_state_t *state) {
	state->exited = false;
	state->hires = false;
	state->vblankWait = false;
	state->awaitingKey = -1;
	state->cycleDiff = 0.0;
	state->timerDiff = 0.0;
	state->vblankDiff = 0.0;
	state->callStackPos = 0;
	state->delayTimer = 0;
	state->soundTimer = 0;
	c8_clearMemory(state);
	c8_clearScreen(state);
	c8_clearInput(state);
	c8_resetRegisters(state);
	c8_seedRandom(state, (UWord) random());
}

c8_status_t c8_cpuStep(c8_state_t *state, int *outCycles) {
	if (state->exited)
		return C8_EXITED;
	if (state->awaitingKey != -1)
		return C8_AWAITING_KEY;

	UWord opcode = c8_readMemoryWord(state, state->regPC);
	struct c8_instruction inst = { 0 };
	c8_status_t status = c8_instructionLookup(state->mode, &inst, opcode);
	if (status != C8_OK)
		return status;
	int cycles = inst.cycles;
	state->regPC += 2;
	int extraCycles = inst.execute(state, opcode);

	if (outCycles != NULL)
		*outCycles = cycles + extraCycles;
	if (state->regPC >= C8_ADDRESSABLE_MEM)
		state->regPC %= C8_ADDRESSABLE_MEM;
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
	if (state->mode == C8_CHIP_8) {
		state->vblankDiff += dt * C8_VBLANK_SPEED;
		while (state->vblankDiff > 1.0) {
			state->vblankWait = false;
			state->vblankDiff -= 1.0;
		}
		if (state->vblankWait)
			return C8_OK;
	}
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
		if (state->vblankWait)
			break;
	}
	if (outCycles != NULL)
		*outCycles = totalCycles;
	return C8_OK;
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
	if (state->mode == C8_CHIP_8) {
		state->vblankDiff += dt * C8_VBLANK_SPEED;
		while (state->vblankDiff > 1) {
			state->vblankWait = false;
			state->vblankDiff -= 1.0;
		}
		if (state->vblankWait)
			return C8_OK;
	}
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
			if (state->regPC == breakpoints[i]) {
				outCycles != NULL && (*outCycles = totalCycles);
				return C8_BREAK;
			}
		}
		if (state->vblankWait)
			break;
	}
	if (outCycles != NULL)
		*outCycles = totalCycles;
	return C8_OK;
}

bool c8_shouldBeep(const c8_state_t *state) {
	return !state->exited && state->soundTimer > 0;
}

UWord c8_delayTimer(const c8_state_t *state) {
	return state->delayTimer;
}
UWord c8_soundTimer(const c8_state_t *state) {
	return state->soundTimer;
}

size_t c8_callStack(const c8_state_t *state, UWord *frames, size_t frameSize) {
	size_t totalFrames = (size_t) state->callStackPos;
	size_t framesWritten = 0;
	for (size_t i = totalFrames - 1; i >= 0; i--) {
		if (framesWritten == frameSize)
			break;
		frames[framesWritten++] = state->callStack[i];
	}
	return totalFrames;
}

const char *c8_disassemble(const c8_state_t *state, UWord pos) {
	UWord word = c8_readMemoryWord(state, pos);
	struct c8_instruction inst = { 0 };
	c8_instructionLookup(state->mode, &inst, word);
	return inst.disassembly;
}

c8_emu_mode_t c8_mode(const c8_state_t *state) {
	return state->mode;
}
