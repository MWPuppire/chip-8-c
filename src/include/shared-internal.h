#include "shared.h"

#ifdef __GNUC__
#	define UNUSED(x) UNUSED_##x __attribute__((__unused__))
#else
#	define UNUSED(x) UNUSED_##x
#endif

#define C8_ADDRESSABLE_MEM (1 << 12)

#ifndef C8_CLOCK_SPEED
#	define C8_CLOCK_SPEED 500.0
#endif

#ifndef C8_TIMER_SPEED
#	define C8_TIMER_SPEED 60.0
#endif

#ifndef C8_VBLANK_SPEED
#	define C8_VBLANK_SPEED 60.0
#endif

#define C8_SCREEN_MEM    (C8_SCREEN_WIDTH * C8_SCREEN_HEIGHT / 8)

#define C8_STACK_SIZE 16

struct c8_state {
	double cycleDiff;
	double timerDiff;
	double vblankDiff;
	c8_emu_mode_t mode;
	int callStackPos;
	int awaitingKey;
	UWord regI;
	UWord regPC;
	UWord randomState;
	UWord callStack[C8_STACK_SIZE];
	UByte registers[16];
	UByte delayTimer;
	UByte soundTimer;
	UByte memory[C8_ADDRESSABLE_MEM];
	UByte screen[C8_SCREEN_MEM];
	UByte registerPersistent[16];
	bool input[16];
	bool exited;
	bool hires;
	bool vblankWait;
};
