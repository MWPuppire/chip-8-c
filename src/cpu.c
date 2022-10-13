#include <shared.h>
#include <opcodes.h>
#include <memory.h>
#include <rom.h>
#include <registers.h>
#include <screen.h>
#include <cpu.h>
#include <input.h>

#include <stdlib.h>

void c8_seedRandom(c8_state_t *state, UWord seed) {
	state->randomState = seed;
}

void c8_cpuBoot(c8_state_t *state) {
	c8_clearMemory(state);
	c8_clearScreen(state);
	c8_clearInput(state);
	c8_resetRegisters(state);
	c8_seedRandom(state, (UWord) random());
	state->awaitingKey = -1;
}

int c8_cpuStep(c8_state_t *state) {
	UWord opcode = c8_readMemoryWord(state, state->registers.pc);
	struct c8_instruction inst = { 0 };
	c8_instructionLookup(&inst, opcode);
	if (inst.execute == NULL) {
		return -1;
	}
	int cycles = inst.cycles;
	state->registers.pc += 2;
	int extraCycles = inst.execute(state, opcode);
	return cycles + extraCycles;
}

c8_status_t c8_emulate(c8_state_t *state, double dt) {
	// Timers still tick while awaiting key input,
	// but don't execute any more instructions.
	state->timerDiff += dt * C8_TIMER_SPEED;
	UByte timerDiff = (UByte) state->timerDiff;
	state->delayTimer -= timerDiff > state->delayTimer
		? state->delayTimer : timerDiff;
	state->soundTimer -= timerDiff > state->soundTimer
		? state->soundTimer : timerDiff;
	state->timerDiff -= timerDiff;
	if (state->awaitingKey != -1) {
		return C8_AWAITING_KEY;
	}
	state->cycleDiff += dt * C8_CLOCK_SPEED;

	while (state->cycleDiff > 0) {
		int cyclesTaken = c8_cpuStep(state);
		if (cyclesTaken < 0)
			return C8_UNKNOWN_OP;
		state->cycleDiff -= cyclesTaken;
	}
	return C8_OK;
}

c8_status_t c8_emulateUntil(c8_state_t *state, double dt, int breakpoint) {
	state->timerDiff += dt * C8_TIMER_SPEED;
	UByte timerDiff = (UByte) state->timerDiff;
	state->delayTimer -= timerDiff > state->delayTimer
		? state->delayTimer : timerDiff;
	state->soundTimer -= timerDiff > state->soundTimer
		? state->soundTimer : timerDiff;
	state->timerDiff -= timerDiff;
	if (state->awaitingKey != -1) {
		return C8_AWAITING_KEY;
	}
	state->cycleDiff += dt * C8_CLOCK_SPEED;

	while (state->cycleDiff > 0) {
		int cyclesTaken = c8_cpuStep(state);
		if (cyclesTaken < 0)
			return C8_UNKNOWN_OP;
		state->cycleDiff -= cyclesTaken;
		if (state->registers.pc == breakpoint)
			return C8_BREAK;
	}
	return C8_OK;
}

bool c8_shouldBeep(c8_state_t *state) {
	return state->soundTimer > 0;
}
