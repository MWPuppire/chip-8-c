#pragma once

#include "shared.h"

#define INSTRUCTION(inst) int c8_##inst(c8_state_t *state, UWord word)

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
INSTRUCTION(schipRegDump);
INSTRUCTION(regLoad);
INSTRUCTION(schipRegLoad);
INSTRUCTION(moveRegister);
INSTRUCTION(spriteAddrI);
INSTRUCTION(persistentDump);
INSTRUCTION(persistentLoad);
// INSTRUCTION(spriteAddrBig);
// INSTRUCTION(regDumpY);
// INSTRUCTION(regLoadY);
// INSTRUCTION(setIWord);

// bitwise.c
INSTRUCTION(orRegister);
INSTRUCTION(cosmacOrRegister);
INSTRUCTION(andRegister);
INSTRUCTION(cosmacAndRegister);
INSTRUCTION(xorRegister);
INSTRUCTION(cosmacXorRegister);
INSTRUCTION(shiftRegisterRight);
INSTRUCTION(schipShiftRegisterRight);
INSTRUCTION(shiftRegisterLeft);
INSTRUCTION(schipShiftRegisterLeft);

// branches.c
INSTRUCTION(ifEquals);
INSTRUCTION(ifNotEquals);
INSTRUCTION(ifEqualsReg);
INSTRUCTION(ifNotEqualsReg);
INSTRUCTION(keyEquals);
INSTRUCTION(keyNotEquals);

// display.c
INSTRUCTION(cosmacDraw);
INSTRUCTION(schipDraw);
INSTRUCTION(xoDraw);
INSTRUCTION(displayClear);
INSTRUCTION(lowres);
INSTRUCTION(highres);
// INSTRUCTION(scrollDown);
// INSTRUCTION(scrollRight);
// INSTRUCTION(scrollLeft);
// INSTRUCTION(scrollUp);
// INSTRUCTION(setDrawingPlane);

// jumps.c
INSTRUCTION(gotoInst);
INSTRUCTION(callInst);
INSTRUCTION(returnInst);
INSTRUCTION(jumpV0);
INSTRUCTION(schipJumpV0);
INSTRUCTION(jumpOffset);

// misc.c
INSTRUCTION(bcd);
INSTRUCTION(randRegister);
INSTRUCTION(nop);
INSTRUCTION(getKey);
INSTRUCTION(exitInst);
// INSTRUCTION(loadAudioBuffer);
// INSTRUCTION(setAudioHertz);

// timers.c
INSTRUCTION(getDelay);
INSTRUCTION(setDelayTimer);
INSTRUCTION(setSoundTimer);
