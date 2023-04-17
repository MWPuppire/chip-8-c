#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define C8_SCREEN_WIDTH  128
#define C8_SCREEN_HEIGHT 64

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
	C8_OK,
	C8_AWAITING_KEY,
	C8_BREAK,
	C8_UNKNOWN_OP,
	C8_INVALID_FILE,
	C8_OP_NOT_DEFINED,
	C8_EXITED,
} c8_status_t;

typedef enum {
	C8_CHIP_8 = 0,
	C8_SUPER_CHIP = 1,
	C8_XO_CHIP = 2,
} c8_emu_mode_t;

typedef struct c8_state c8_state_t;

extern const int C8_VERSION_MAJOR;
extern const int C8_VERSION_MINOR;
extern const int C8_VERSION_PATCH;
extern const char *C8_VERSION_STRING;
