#pragma once

#include "shared.h"

void clearInput(struct emuState *state);
void pressKey(struct emuState *state, int key);
void releaseKey(struct emuState *state, int key);
bool isKeyDown(struct emuState *state, int key);
