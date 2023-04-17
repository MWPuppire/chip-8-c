#pragma once

#include "shared.h"

c8_state_t *c8_newState(void);
void c8_seedRandom(c8_state_t *state, UWord seed);
void c8_cpuBoot(c8_state_t *state);
c8_status_t c8_cpuStep(c8_state_t *state, int *cyclesTaken);
c8_status_t c8_emulate(c8_state_t *state, double dt, int *cyclesTaken);
c8_status_t c8_emulateUntil(c8_state_t *state, double dt, int *cyclesTaken, int *breakpoints, int n);
bool c8_shouldBeep(const c8_state_t *state);
UWord c8_delayTimer(const c8_state_t *state);
UWord c8_soundTimer(const c8_state_t *state);
size_t c8_callStack(const c8_state_t *state, UWord *frames, size_t frameSize);
const char *c8_disassemble(const c8_state_t *state, UWord pos);
