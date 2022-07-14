#pragma once

#include "shared.h"

UByte writeToScreen(struct emuState *state, UByte x, UByte y);
UByte readFromScreen(struct emuState *state, UByte x, UByte y);
void clearScreen(struct emuState *state);
