#pragma once

#include "shared.h"

UByte c8_readMemoryByte(c8_state_t *state, UWord position);
SByte c8_readMemorySByte(c8_state_t *state, UWord position);
UWord c8_readMemoryWord(c8_state_t *state, UWord position);
SWord c8_readMemorySWord(c8_state_t *state, UWord position);
void c8_writeMemoryByte(c8_state_t *state, UWord position, UByte value);
void c8_writeMemoryWord(c8_state_t *state, UWord position, UWord value);
void c8_clearMemory(c8_state_t *state);
