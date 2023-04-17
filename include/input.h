#pragma once

#include "shared.h"

void c8_clearInput(c8_state_t *state);
void c8_pressKey(c8_state_t *state, int key);
void c8_releaseKey(c8_state_t *state, int key);
bool c8_isKeyDown(const c8_state_t *state, int key);
