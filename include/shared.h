#pragma once

#include <stdint.h>
#include <string.h>

#ifdef __GNUC__
#	define UNUSED(x) UNUSED_##x __attribute__((__unused__))
#else
#	define UNUSED(x) UNUSED_##x
#endif

#define ADDRESSABLE_MEM (1 << 12)

#ifndef CLOCK_SPEED
#define CLOCK_SPEED 500
#endif

#ifndef TIMER_SPEED
#define TIMER_SPEED 60
#endif

#define SCREEN_WIDTH  64
#define SCREEN_HEIGHT 32
#define SCREEN_MEM    (SCREEN_WIDTH * SCREEN_HEIGHT / 8)

#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN \
	|| defined(__BIG_ENDIAN__) \
	|| defined(__ARMEB__) \
	|| defined(__THUMBEB__) \
	|| defined(__AARCH64EB__) \
	|| defined(_MIBSEB) \
	|| defined(__MIBSEB) \
	|| defined(__MIBSEB__)
// some checks I can use
// this won't cover everything
// but I can assume it's little-endian otherwise
#define BIG_ENDIAN
#endif

#if defined(UINT16_MAX) && defined(INT16_MAX)
typedef  int16_t SWord;
typedef uint16_t UWord;
#else
#define NO_EXACT_WORD
typedef  int_least16_t SWord;
typedef uint_least16_t UWord;
#endif

#if defined(UINT8_MAX) && defined(INT8_MAX)
typedef  int8_t SByte;
typedef uint8_t UByte;
#else
#define NO_EXACT_BYTE
typedef  int_least8_t SByte;
typedef uint_least8_t UByte;
#endif

struct emuState {
	long double cycleDiff;
/*
	union {
		UByte memory[ADDRESSABLE_MEM];
		struct __attribute__((packed)) {
			UByte _memMirror[ADDRESSABLE_MEM - SCREEN_MEM];
			UByte screen[SCREEN_MEM];
		};
	};
*/
	UByte memory[ADDRESSABLE_MEM];
	UByte screen[SCREEN_MEM];
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
	UByte input[16];
	UWord callStack[64];
	UWord randomState;
	int callStackPos;
};
