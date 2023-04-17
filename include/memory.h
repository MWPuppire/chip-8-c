#pragma once

#include "shared.h"

UByte c8_readMemoryByte(const c8_state_t *state, UWord position);
SByte c8_readMemorySByte(const c8_state_t *state, UWord position);
UWord c8_readMemoryWord(const c8_state_t *state, UWord position);
SWord c8_readMemorySWord(const c8_state_t *state, UWord position);
void c8_writeMemoryByte(c8_state_t *state, UWord position, UByte value);
void c8_writeMemoryWord(c8_state_t *state, UWord position, UWord value);
void c8_clearMemory(c8_state_t *state);
c8_status_t c8_loadROM(c8_state_t *state, const unsigned char *rom, size_t size);
void c8_dumpMemory(const c8_state_t *state, unsigned char *buf, size_t size);
void c8_loadMemory(c8_state_t *state, const unsigned char *buf, size_t size);
