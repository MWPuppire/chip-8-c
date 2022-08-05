#include <shared.h>
#include <opcodes.h>
#include <memory.h>
#include <rom.h>
#include <registers.h>
#include <screen.h>
#include <cpu.h>
#include <sound.h>
#include <delta.h>

#include <stdio.h>
#include <stdlib.h>

void seedRandom(struct emuState *state, UWord seed) {
	state->randomState = seed;
}

void cpuBoot(struct emuState *state) {
	clearMemory(state);
	clearScreen(state);
	resetRegisters(state);
	long double time = deltatime(NULL);
	seedRandom(state, (UWord) time);
}

int cpuStep(struct emuState *state) {
	UWord opcode = readMemoryWord(state, state->registers.pc);
	struct instruction inst;
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

int emulate(struct emuState *state, long double dt) {
	state->cycleDiff += dt * CLOCK_SPEED;
	long double timerDiff = dt * TIMER_SPEED;
	if (state->delayTimer > 0) {
		UByte diff = timerDiff > state->delayTimer
			? state->delayTimer : timerDiff;
		state->delayTimer -= diff;
	}
	if (state->soundTimer > 0) {
		emitBeep(state);
		UByte diff = timerDiff > state->delayTimer
			? state->delayTimer : timerDiff;
		state->soundTimer -= diff;
	}

	while (state->cycleDiff > 0) {
		int cyclesTaken = cpuStep(state);
		if (cyclesTaken < 0)
			return 1;
		state->cycleDiff -= cyclesTaken;
	}
	return 0;
}

int emulateUntil(struct emuState *state, long double dt, int breakpoint) {
	state->cycleDiff += dt * CLOCK_SPEED;
	long double timerDiff = dt * TIMER_SPEED;
	if (state->delayTimer > 0) {
		UByte diff = timerDiff > state->delayTimer
			? state->delayTimer : timerDiff;
		state->delayTimer -= diff;
	}
	if (state->soundTimer > 0) {
		emitBeep(state);
		UByte diff = timerDiff > state->delayTimer
			? state->delayTimer : timerDiff;
		state->soundTimer -= diff;
	}

	while (state->cycleDiff > 0) {
		int cyclesTaken = cpuStep(state);
		if (cyclesTaken < 0)
			return 1;
		state->cycleDiff -= cyclesTaken;
		if (state->registers.pc == breakpoint)
			return 1;
	}
	return 0;
}
