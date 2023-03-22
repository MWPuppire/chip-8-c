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
INSTRUCTION(regLoad);
INSTRUCTION(moveRegister);
INSTRUCTION(spriteAddrI);
#if defined(SCHIP) || defined(XO_CHIP)
INSTRUCTION(persistentDump);
INSTRUCTION(persistentLoad);
// INSTRUCTION(spriteAddrBig);
#endif
#ifdef XO_CHIP
// INSTRUCTION(regDumpY);
// INSTRUCTION(regLoadY);
// INSTRUCTION(setIWord);
#endif

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
INSTRUCTION(keyEquals);
INSTRUCTION(keyNotEquals);

// display.c
INSTRUCTION(draw);
INSTRUCTION(displayClear);
#if defined(SCHIP) || defined(XO_CHIP)
INSTRUCTION(lowres);
INSTRUCTION(highres);
// INSTRUCTION(scrollDown);
// INSTRUCTION(scrollRight);
// INSTRUCTION(scrollLeft);
#endif
#ifdef XO_CHIP
// INSTRUCTION(scrollUp);
// INSTRUCTION(setDrawingPlane);
#endif

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
INSTRUCTION(getKey);
#if defined(SCHIP) || defined(XO_CHIP)
INSTRUCTION(exitInst);
#endif
#ifdef XO_CHIP
// INSTRUCTION(loadAudioBuffer);
// INSTRUCTION(setAudioHertz);
#endif

// timers.c
INSTRUCTION(getDelay);
INSTRUCTION(setDelayTimer);
INSTRUCTION(setSoundTimer);
