#pragma once

#include "shared.h"

UByte c8_writeToScreen(c8_state_t *state, UByte x, UByte y);
UByte c8_readFromScreen(c8_state_t *state, UByte x, UByte y);
void c8_clearScreen(c8_state_t *state);
