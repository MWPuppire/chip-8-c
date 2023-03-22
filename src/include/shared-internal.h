#include "shared.h"

#ifdef __GNUC__
#	define UNUSED(x) UNUSED_##x __attribute__((__unused__))
#else
#	define UNUSED(x) UNUSED_##x
#endif

#define C8_ADDRESSABLE_MEM (1 << 12)

#ifndef C8_CLOCK_SPEED
#	define C8_CLOCK_SPEED 500
#endif

#ifndef C8_TIMER_SPEED
#	define C8_TIMER_SPEED 60
#endif

#define C8_SCREEN_MEM    (C8_SCREEN_WIDTH * C8_SCREEN_HEIGHT / 8)

#define C8_STACK_SIZE 16

struct c8_state {
	struct {
		UByte v0;
		UByte v1;
		UByte v2;
		UByte v3;
		UByte v4;
		UByte v5;
		UByte v6;
		UByte v7;
		UByte v8;
		UByte v9;
		UByte vA;
		UByte vB;
		UByte vC;
		UByte vD;
		UByte vE;
		UByte vF;
		UWord I;
		UWord pc;
	} registers;
	UWord randomState;
	UWord callStack[C8_STACK_SIZE];
	UByte delayTimer;
	UByte soundTimer;
	UByte memory[C8_ADDRESSABLE_MEM];
	UByte screen[C8_SCREEN_MEM];
	double cycleDiff;
	double timerDiff;
	int callStackPos;
	int awaitingKey;
	bool input[16];
	bool exited;
#if defined(SCHIP) || defined(XO_CHIP)
	bool hires;
	UByte registerPersistent[16];
#endif
};