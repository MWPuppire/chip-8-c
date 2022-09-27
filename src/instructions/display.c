#include <shared.h>
#include <memory.h>
#include <registers.h>
#include <screen.h>
#include <instructions.h>

int draw(struct emuState *state, UWord word) {
	UByte x = readRegister(state, (word & 0xF00) >> 8);
	UByte y = readRegister(state, (word & 0x0F0) >> 4);
	UByte n = (UByte) word & 0xF;
	UWord idx = state->registers.I;
	UByte flag = 0;
	// currently has wrapping behavior
	// CHIP-8 and SCHIP-8 have clipping
	for (int i = 0; i < n; i++) {
		UByte tempX = x;
		UByte byte = readMemoryByte(state, idx);
		while (byte > 0) {
			if ((byte & 128) == 128)
				flag |= writeToScreen(state,
					tempX % SCREEN_WIDTH,
					y % SCREEN_HEIGHT);
			tempX += 1;
			byte <<= 1;
		}
		y += 1;
		idx += 1;
	}
	state->registers.vF = flag;
	return 0;
}

int displayClear(struct emuState *state, UWord UNUSED(word)) {
	clearScreen(state);
	return 0;
}
