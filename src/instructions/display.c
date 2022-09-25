#include <shared.h>
#include <memory.h>
#include <registers.h>
#include <screen.h>
#include <instructions.h>

int draw(struct emuState *state, UWord word) {
	UByte x = readRegister(state, (word & 0xF00) >> 8) % SCREEN_WIDTH;
	UByte y = readRegister(state, (word & 0x0F0) >> 4) % SCREEN_HEIGHT;
	UByte n = (UByte) word & 0xF;
	UWord idx = state->registers.I;
	UByte flag = 0;
	for (int i = 0; i < n; i++) {
		UByte tempX = x + 7;
		UByte byte = readMemoryByte(state, idx);
		while (byte > 0) {
			if (byte & 1)
				flag |= writeToScreen(state, tempX, y);
			tempX -= 1;
			byte >>= 1;
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
