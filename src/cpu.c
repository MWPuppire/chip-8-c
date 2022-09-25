#include <shared.h>
#include <opcodes.h>
#include <memory.h>
#include <rom.h>
#include <registers.h>
#include <screen.h>
#include <cpu.h>
#include <delta.h>
#include <input.h>

#include <stdio.h>
#include <stdlib.h>

void seedRandom(struct emuState *state, UWord seed) {
	state->randomState = seed;
}

void cpuBoot(struct emuState *state) {
	clearMemory(state);
	clearScreen(state);
	clearInput(state);
	resetRegisters(state);
	long double time = deltatime(NULL);
	seedRandom(state, (UWord) time);
	state->awaitingKey = -1;
}

int cpuStep(struct emuState *state) {
	UWord opcode = readMemoryWord(state, state->registers.pc);
	struct instruction inst = { 0 };
	instructionLookup(&inst, opcode);
	if (inst.execute == NULL) {
		unknownOpcode(state, inst);
		return -1;
	}
	int cycles = inst.cycles;
	state->registers.pc += 2;
	int extraCycles = inst.execute(state, opcode);
	return cycles + extraCycles;
}

cpu_status_t emulate(struct emuState *state, long double dt) {
	if (state->awaitingKey != -1) {
		return CPU_AWAITING_KEY;
	}
	state->cycleDiff += dt * CLOCK_SPEED;
	long double timerDiff = dt * TIMER_SPEED;
	if (state->delayTimer > 0) {
		UByte diff = timerDiff > state->delayTimer
			? state->delayTimer : timerDiff;
		state->delayTimer -= diff;
	}
	if (state->soundTimer > 0) {
		UByte diff = timerDiff > state->delayTimer
			? state->delayTimer : timerDiff;
		state->soundTimer -= diff;
	}

	while (state->cycleDiff > 0) {
		int cyclesTaken = cpuStep(state);
		if (cyclesTaken < 0)
			return CPU_UNKNOWN_OP;
		state->cycleDiff -= cyclesTaken;
	}
	return CPU_OK;
}

cpu_status_t emulateUntil(struct emuState *state, long double dt, int breakpoint) {
	if (state->awaitingKey != -1) {
		return CPU_AWAITING_KEY;
	}
	state->cycleDiff += dt * CLOCK_SPEED;
	long double timerDiff = dt * TIMER_SPEED;
	if (state->delayTimer > 0) {
		UByte diff = timerDiff > state->delayTimer
			? state->delayTimer : timerDiff;
		state->delayTimer -= diff;
	}
	if (state->soundTimer > 0) {
		UByte diff = timerDiff > state->soundTimer
			? state->soundTimer : timerDiff;
		state->soundTimer -= diff;
	}

	while (state->cycleDiff > 0) {
		int cyclesTaken = cpuStep(state);
		if (cyclesTaken < 0)
			return CPU_UNKNOWN_OP;
		state->cycleDiff -= cyclesTaken;
		if (state->registers.pc == breakpoint)
			return CPU_BREAK;
	}
	return CPU_OK;
}

bool shouldBeep(struct emuState *state) {
	return state->soundTimer > 0;
}
