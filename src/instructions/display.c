#include <shared-internal.h>
#include <memory.h>
#include <registers.h>
#include <screen.h>
#include <instructions.h>

int c8_cosmacDraw(c8_state_t *state, UWord word) {
	state->vblankWait = true;
	c8_register_t xReg = (c8_register_t) ((word >> 8) & 0xF);
	c8_register_t yReg = (c8_register_t) ((word >> 4) & 0xF);
	UByte x = c8_readRegister(state, xReg) % (C8_SCREEN_WIDTH / 2);
	UByte y = c8_readRegister(state, yReg) % (C8_SCREEN_HEIGHT / 2);
	UByte n = (UByte) word & 0xF;
	UWord idx = state->regI;
	UByte flag = 0;
	for (int i = 0; i < n; i++) {
		UByte tempX = x;
		UByte byte = c8_readMemoryByte(state, idx);
		while (byte > 0) {
			if ((byte & 128) == 128) {
				flag |= c8_writeToScreen(state, tempX, y);
			}
			tempX += 1;
			byte <<= 1;
		}
		y += 1;
		idx += 1;
	}
	state->registers[0xF] = flag;
	return 0;
}

int c8_schipDraw(c8_state_t *state, UWord word) {
	c8_register_t xReg = (c8_register_t) ((word >> 8) & 0xF);
	c8_register_t yReg = (c8_register_t) ((word >> 4) & 0xF);
	UByte x, y;
	if (state->hires) {
		x = c8_readRegister(state, xReg) % C8_SCREEN_WIDTH;
		y = c8_readRegister(state, yReg) % C8_SCREEN_HEIGHT;
	} else {
		x = c8_readRegister(state, xReg) % (C8_SCREEN_WIDTH / 2);
		y = c8_readRegister(state, yReg) % (C8_SCREEN_HEIGHT / 2);
	}
	UByte n = (UByte) word & 0xF;
	UWord idx = state->regI;
	UByte flag = 0;
	for (int i = 0; i < n; i++) {
		UByte tempX = x;
		UByte byte = c8_readMemoryByte(state, idx);
		while (byte > 0) {
			if ((byte & 128) == 128) {
				flag |= c8_writeToScreen(state, tempX, y);
			}
			tempX += 1;
			byte <<= 1;
		}
		y += 1;
		idx += 1;
	}
	state->registers[0xF] = flag;
	return 0;
}

int c8_xoDraw(c8_state_t *state, UWord word) {
	UByte screenWidth, screenHeight;
	if (state->hires) {
		screenWidth = C8_SCREEN_WIDTH;
		screenHeight = C8_SCREEN_HEIGHT;
	} else {
		screenWidth = C8_SCREEN_WIDTH / 2;
		screenHeight = C8_SCREEN_HEIGHT / 2;
	}
	c8_register_t xReg = (c8_register_t) ((word >> 8) & 0xF);
	c8_register_t yReg = (c8_register_t) ((word >> 4) & 0xF);
	UByte x = c8_readRegister(state, xReg) % screenWidth;
	UByte y = c8_readRegister(state, yReg) % screenHeight;
	UByte n = (UByte) word & 0xF;
	UWord idx = state->regI;
	UByte flag = 0;
	for (int i = 0; i < n; i++) {
		UByte tempX = x;
		UByte byte = c8_readMemoryByte(state, idx);
		while (byte > 0) {
			if ((byte & 128) == 128) {
				flag |= c8_writeToScreen(state,
					tempX % screenWidth,
					y % screenHeight);
			}
			tempX += 1;
			byte <<= 1;
		}
		y += 1;
		idx += 1;
	}
	state->registers[0xF] = flag;
	return 0;
}

int c8_displayClear(c8_state_t *state, UWord UNUSED(word)) {
	c8_clearScreen(state);
	return 0;
}

int c8_lowres(c8_state_t *state, UWord UNUSED(word)) {
	state->hires = false;
	return 0;
}

int c8_highres(c8_state_t *state, UWord UNUSED(word)) {
	state->hires = true;
	return 0;
}
