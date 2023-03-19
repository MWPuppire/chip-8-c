#pragma once

#include "shared.h"

c8_state_t *c8_newState(void);
void c8_seedRandom(c8_state_t *state, UWord seed);
void c8_cpuBoot(c8_state_t *state);
int c8_cpuStep(c8_state_t *state);
c8_status_t c8_emulate(c8_state_t *state, double dt);
c8_status_t c8_emulateUntil(c8_state_t *state, double dt, int *breakpoints, int n);
bool c8_shouldBeep(c8_state_t *state);
