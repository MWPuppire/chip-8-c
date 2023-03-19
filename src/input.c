#include <string.h>

#include <shared-internal.h>
#include <registers.h>

void c8_clearInput(c8_state_t *state) {
	memset(state->input, 0, sizeof(bool) * 16);
}

void c8_pressKey(c8_state_t *state, int key) {
	state->input[key] = true;
	if (state->awaitingKey != -1) {
		c8_writeRegister(state, (c8_register_t) state->awaitingKey,
			(UByte) key);
		state->awaitingKey = -1;
	}
}
void c8_releaseKey(c8_state_t *state, int key) {
	state->input[key] = false;
}

bool c8_isKeyDown(c8_state_t *state, int key) {
	return state->input[key];
}
