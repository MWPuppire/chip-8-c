#include <string.h>

#include <shared-internal.h>
#include <screen.h>

static inline bool c8_writeToScreenLowres(c8_state_t *state, UByte x, UByte y) {
	size_t pos = (size_t) x / 8 + (size_t) y * (C8_SCREEN_WIDTH / 8);
	UByte byte = 1 << (x & 7);
	bool toggle = (state->screen[pos] & byte) > 0;
	state->screen[pos] ^= byte;
	return toggle;
}

bool c8_writeToScreen(c8_state_t *state, UByte x, UByte y) {
	if (!state->hires) {
		bool toggle = 0;
		x <<= 1;
		y <<= 1;
		if (x >= C8_SCREEN_WIDTH || y >= C8_SCREEN_HEIGHT)
			return 0;
		toggle |= c8_writeToScreenLowres(state, x + 0, y + 0);
		toggle |= c8_writeToScreenLowres(state, x + 1, y + 0);
		toggle |= c8_writeToScreenLowres(state, x + 0, y + 1);
		toggle |= c8_writeToScreenLowres(state, x + 1, y + 1);
		return toggle;
	} else {
		if (x >= C8_SCREEN_WIDTH || y >= C8_SCREEN_HEIGHT)
			return 0;
		return c8_writeToScreenLowres(state, x, y);
	}
}

UByte c8_readFromScreen(const c8_state_t *state, UByte x, UByte y) {
	size_t pos = (size_t) x / 8 + (size_t) y * (C8_SCREEN_WIDTH / 8);
	return (state->screen[pos] & (1 << (x & 7))) != 0;
}

void c8_clearScreen(c8_state_t *state) {
	memset(state->screen, 0, sizeof(state->screen));
}
