#include <stdlib.h>

#include <shared-internal.h>
#include <opcodes.h>
#include <registers.h>
#include <screen.h>
#include <cpu.h>
#include <input.h>
#include <memory.h>

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

int c8_cpuStep(c8_state_t *state) {
	if (state->exited)
		return -1;
	UWord opcode = c8_readMemoryWord(state, state->registers.pc);
	struct c8_instruction inst = { 0 };
	c8_instructionLookup(&inst, opcode);
	if (inst.execute == NULL)
		return -1;
	int cycles = inst.cycles;
	state->registers.pc += 2;
	int extraCycles = inst.execute(state, opcode);
	return cycles + extraCycles;
}

c8_status_t c8_emulate(c8_state_t *state, double dt) {
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
	if (state->awaitingKey != -1)
		return C8_AWAITING_KEY;
	state->cycleDiff += dt * C8_CLOCK_SPEED;

	while (state->cycleDiff > 0) {
		int cyclesTaken = c8_cpuStep(state);
		if (cyclesTaken < 0)
			return C8_UNKNOWN_OP;
		state->cycleDiff -= cyclesTaken;
	}
	return C8_OK;
}

c8_status_t c8_emulateUntil(c8_state_t *state, double dt, int *breakpoints, int n) {
	if (state->exited)
		return C8_EXITED;

	state->timerDiff += dt * C8_TIMER_SPEED;
	UByte timerDiff = (UByte) state->timerDiff;
	state->delayTimer -= timerDiff > state->delayTimer
		? state->delayTimer : timerDiff;
	state->soundTimer -= timerDiff > state->soundTimer
		? state->soundTimer : timerDiff;
	state->timerDiff -= timerDiff;
	if (state->awaitingKey != -1)
		return C8_AWAITING_KEY;
	state->cycleDiff += dt * C8_CLOCK_SPEED;

	while (state->cycleDiff > 0) {
		int cyclesTaken = c8_cpuStep(state);
		if (cyclesTaken < 0)
			return C8_UNKNOWN_OP;
		state->cycleDiff -= cyclesTaken;
		for (int i = 0; i < n; i++) {
			if (state->registers.pc == breakpoints[i])
				return C8_BREAK;
		}
	}
	return C8_OK;
}

bool c8_shouldBeep(c8_state_t *state) {
	return !state->exited && state->soundTimer > 0;
}
