#pragma once

#include "shared.h"

UByte readMemoryByte(struct emuState *state, UWord position);
SByte readMemorySByte(struct emuState *state, UWord position);
UWord readMemoryWord(struct emuState *state, UWord position);
SWord readMemorySWord(struct emuState *state, UWord position);
void writeMemoryByte(struct emuState *state, UWord position, UByte value);
void writeMemoryWord(struct emuState *state, UWord position, UWord value);
void clearMemory(struct emuState *state);
