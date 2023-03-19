#pragma once

#include "shared.h"

extern const unsigned char C8_FONT_SET[80];
extern const UWord C8_SPRITE_ADDR[16];

UByte c8_readMemoryByte(c8_state_t *state, UWord position);
SByte c8_readMemorySByte(c8_state_t *state, UWord position);
UWord c8_readMemoryWord(c8_state_t *state, UWord position);
SWord c8_readMemorySWord(c8_state_t *state, UWord position);
void c8_writeMemoryByte(c8_state_t *state, UWord position, UByte value);
void c8_writeMemoryWord(c8_state_t *state, UWord position, UWord value);
void c8_clearMemory(c8_state_t *state);
c8_status_t c8_loadROM(c8_state_t *state, const unsigned char *rom, size_t size);
void c8_dumpMemory(c8_state_t *state, void *buf, size_t size);
void c8_loadMemory(c8_state_t *state, void *buf, size_t size);
