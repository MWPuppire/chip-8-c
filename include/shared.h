#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#if defined(SCHIP) || defined(XO_CHIP)
#	define C8_SCREEN_WIDTH  128
#	define C8_SCREEN_HEIGHT 64
#else
#	define C8_SCREEN_WIDTH 64
#	define C8_SCREEN_HEIGHT 32
#endif

#if defined(UINT16_MAX) && defined(INT16_MAX)
	typedef  int16_t SWord;
	typedef uint16_t UWord;
#else
	typedef  int_fast16_t SWord;
	typedef uint_fast16_t UWord;
#endif

#if defined(UINT8_MAX) && defined(INT8_MAX)
	typedef  int8_t SByte;
	typedef uint8_t UByte;
#else
	typedef  int_fast8_t SByte;
	typedef uint_fast8_t UByte;
#endif

typedef enum {
	C8_OK, C8_AWAITING_KEY, C8_BREAK, C8_UNKNOWN_OP, C8_INVALID_FILE,
	C8_EXITED
} c8_status_t;

typedef struct c8_state c8_state_t;
