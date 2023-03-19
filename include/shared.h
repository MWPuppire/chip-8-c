#pragma once

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#ifdef __GNUC__
#	define UNUSED(x) UNUSED_##x __attribute__((__unused__))
#else
#	define UNUSED(x) UNUSED_##x
#endif

#define C8_ADDRESSABLE_MEM (1 << 12)

#ifndef C8_CLOCK_SPEED
#	ifdef SCHIP
#		define C8_CLOCK_SPEED 1000
#	else
#		define C8_CLOCK_SPEED 500
#	endif
#endif

#ifndef C8_TIMER_SPEED
#	define C8_TIMER_SPEED 60
#endif

#define C8_SCREEN_WIDTH  64
#define C8_SCREEN_HEIGHT 32
#define C8_SCREEN_MEM    (C8_SCREEN_WIDTH * C8_SCREEN_HEIGHT / 8)

#define C8_STACK_SIZE 16

#if defined(UINT16_MAX) && defined(INT16_MAX)
	typedef  int16_t SWord;
	typedef uint16_t UWord;
#else
#	define C8_NO_EXACT_WORD
	typedef  int_fast16_t SWord;
	typedef uint_fast16_t UWord;
#endif

#if defined(UINT8_MAX) && defined(INT8_MAX)
	typedef  int8_t SByte;
	typedef uint8_t UByte;
#else
#	define C8_NO_EXACT_BYTE
	typedef  int_fast8_t SByte;
	typedef uint_fast8_t UByte;
#endif

typedef enum {
	C8_OK, C8_AWAITING_KEY, C8_BREAK, C8_UNKNOWN_OP, C8_INVALID_FILE
} c8_status_t;

typedef struct {
	double cycleDiff;
	UByte memory[C8_ADDRESSABLE_MEM];
	UByte screen[C8_SCREEN_MEM];
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
	UByte delayTimer;
	UByte soundTimer;
	bool input[16];
	UWord callStack[C8_STACK_SIZE];
	UWord randomState;
	int callStackPos;
	int awaitingKey;
	double timerDiff;
} c8_state_t;
