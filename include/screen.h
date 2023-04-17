#pragma once

#include "shared.h"

bool c8_writeToScreen(c8_state_t *state, UByte x, UByte y);
UByte c8_readFromScreen(const c8_state_t *state, UByte x, UByte y);
void c8_clearScreen(c8_state_t *state);
