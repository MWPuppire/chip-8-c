#pragma once

#include "shared.h"

void cpuBoot(struct emuState *state);
int cpuStep(struct emuState *state);
int emulate(struct emuState *state, long double dt);
int emulateUntil(struct emuState *state, long double dt, int breakpoint);
