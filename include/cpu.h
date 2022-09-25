#pragma once

#include "shared.h"

typedef enum {
	CPU_OK, CPU_UNKNOWN_OP, CPU_AWAITING_KEY, CPU_BREAK
} cpu_status_t;

void seedRandom(struct emuState *state, UWord seed);
void cpuBoot(struct emuState *state);
int cpuStep(struct emuState *state);
cpu_status_t emulate(struct emuState *state, long double dt);
cpu_status_t emulateUntil(struct emuState *state, long double dt, int breakpoint);
bool shouldBeep(struct emuState *state);
