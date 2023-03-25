#include <shared-internal.h>
#include <memory.h>
#include <registers.h>
#include <screen.h>
#include <instructions.h>

int c8_draw(c8_state_t *state, UWord word) {
#ifdef COSMAC
	state->vblankWait = true;
#endif
	c8_register_t xReg = (c8_register_t) ((word >> 8) & 0xF);
	c8_register_t yReg = (c8_register_t) ((word >> 4) & 0xF);
#if defined(SCHIP) || defined(XO_CHIP)
	UByte x, y;
	if (state->hires) {
		x = c8_readRegister(state, xReg) % C8_SCREEN_WIDTH;
		y = c8_readRegister(state, yReg) % C8_SCREEN_HEIGHT;
	} else {
		x = c8_readRegister(state, xReg) % (C8_SCREEN_WIDTH / 2);
		y = c8_readRegister(state, yReg) % (C8_SCREEN_HEIGHT / 2);
	}
#else
	UByte x = c8_readRegister(state, xReg) % C8_SCREEN_WIDTH;
	UByte y = c8_readRegister(state, yReg) % C8_SCREEN_HEIGHT;
#endif
	UByte n = (UByte) word & 0xF;
	UWord idx = state->registers.I;
	UByte flag = 0;
	for (int i = 0; i < n; i++) {
		UByte tempX = x;
		UByte byte = c8_readMemoryByte(state, idx);
		while (byte > 0) {
			if ((byte & 128) == 128) {
#ifdef XO_CHIP
				if (state->hires) {
					flag |= c8_writeToScreen(state,
						tempX % C8_SCREEN_WIDTH,
						y % C8_SCREEN_HEIGHT);
				} else {
					flag |= c8_writeToScreen(state,
						tempX % (C8_SCREEN_WIDTH / 2),
						y % (C8_SCREEN_HEIGHT / 2));
				}
#else
				flag |= c8_writeToScreen(state, tempX, y);
#endif
			}
			tempX += 1;
			byte <<= 1;
		}
		y += 1;
		idx += 1;
	}
	state->registers.vF = flag;
	return 0;
}

int c8_displayClear(c8_state_t *state, UWord UNUSED(word)) {
	c8_clearScreen(state);
	return 0;
}

#if defined(SCHIP) || defined(XO_CHIP)
int c8_lowres(c8_state_t *state, UWord UNUSED(word)) {
	state->hires = false;
	return 0;
}

int c8_highres(c8_state_t *state, UWord UNUSED(word)) {
	state->hires = true;
	return 0;
}
#endif
