#include <shared.h>
#include <screen.h>

UByte c8_writeToScreen(c8_state_t *state, UByte x, UByte y) {
	UByte pos = x / 8 + y * 8;
#ifdef NO_EXACT_BYTE
	pos &= 256;
#endif
	UByte byte = 1 << (x & 7);
	UByte toggle = state->screen[pos] & byte ? 1 : 0;
	state->screen[pos] ^= byte;
	return toggle;
}

UByte c8_readFromScreen(c8_state_t *state, UByte x, UByte y) {
	return (state->screen[x / 8 + y * 8] & (1 << (x & 7))) != 0;
}

void c8_clearScreen(c8_state_t *state) {
	memset(state->screen, 0, sizeof(state->screen));
}
