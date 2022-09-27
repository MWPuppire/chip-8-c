#include <shared.h>
#include <memory.h>
#include <registers.h>
#include <screen.h>
#include <instructions.h>

int c8_draw(c8_state_t *state, UWord word) {
	c8_register_t xReg = (c8_register_t) ((word >> 8) & 0xF);
	c8_register_t yReg = (c8_register_t) ((word >> 4) & 0xF);
	UByte x = c8_readRegister(state, xReg);
	UByte y = c8_readRegister(state, yReg);
	UByte n = (UByte) word & 0xF;
	UWord idx = state->registers.I;
	UByte flag = 0;
	// currently has wrapping behavior
	// CHIP-8 and SCHIP-8 have clipping
	for (int i = 0; i < n; i++) {
		UByte tempX = x;
		UByte byte = c8_readMemoryByte(state, idx);
		while (byte > 0) {
			if ((byte & 128) == 128)
				flag |= c8_writeToScreen(state,
					tempX % C8_SCREEN_WIDTH,
					y % C8_SCREEN_HEIGHT);
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
