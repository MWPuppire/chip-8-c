#include <shared.h>
#include <registers.h>

#include <stdlib.h>
#include <stdbool.h>

void clearInput(struct emuState *state) {
	memset(state->input, 0, sizeof(bool) * 16);
}

void pressKey(struct emuState *state, int key) {
	state->input[key] = true;
	if (state->awaitingKey != -1) {
		writeRegister(state, (UByte) state->awaitingKey, (UByte) key);
		state->awaitingKey = -1;
	}
}
void releaseKey(struct emuState *state, int key) {
	state->input[key] = false;
}

bool isKeyDown(struct emuState *state, int key) {
	return state->input[key];
}
