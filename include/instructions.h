#pragma once

#include "shared.h"

#define INSTRUCTION(x) int x(struct emuState *state, UWord word)

// arithmetic.c
INSTRUCTION(pluseqImmediate);
INSTRUCTION(pluseqRegister);
INSTRUCTION(minuseqRegister);
INSTRUCTION(subtractionRegister);
INSTRUCTION(addToI);

// assignment.c
INSTRUCTION(setRegister);
INSTRUCTION(setIRegister);
INSTRUCTION(regDump);
INSTRUCTION(regLoad);
INSTRUCTION(moveRegister);
INSTRUCTION(spriteAddrI);

// bitwise.c
INSTRUCTION(orRegister);
INSTRUCTION(andRegister);
INSTRUCTION(xorRegister);
INSTRUCTION(shiftRegisterRight);
INSTRUCTION(shiftRegisterLeft);

// branches.c
INSTRUCTION(ifEquals);
INSTRUCTION(ifNotEquals);
INSTRUCTION(ifEqualsReg);
INSTRUCTION(ifNotEqualsReg);

// display.c
INSTRUCTION(draw);
INSTRUCTION(displayClear);

// input.c
INSTRUCTION(keyEquals);
INSTRUCTION(keyNotEquals);
INSTRUCTION(getKey);

// jumps.c
INSTRUCTION(gotoInst);
INSTRUCTION(callInst);
INSTRUCTION(returnInst);
INSTRUCTION(jumpV0);
INSTRUCTION(jumpOffset);

// misc.c
INSTRUCTION(bcd);
INSTRUCTION(randRegister);
INSTRUCTION(nop);

// timers.c
INSTRUCTION(getDelay);
INSTRUCTION(setDelayTimer);
INSTRUCTION(setSoundTimer);
