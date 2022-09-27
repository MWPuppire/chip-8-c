#include <shared.h>
#include <memory.h>
#include <registers.h>
#include <instructions.h>

int c8_keyEquals(c8_state_t *state, UWord word) {
	c8_register_t reg = (c8_register_t) ((word >> 8) & 0xF);
	int key = (int) c8_readRegister(state, reg);
	if (state->input[key])
		state->registers.pc += 2;
	return 0;
}

int c8_keyNotEquals(c8_state_t *state, UWord word) {
	c8_register_t reg = (c8_register_t) ((word >> 8) & 0xF);
	int key = (int) c8_readRegister(state, reg);
	if (!state->input[key])
		state->registers.pc += 2;
	return 0;
}

int c8_getKey(c8_state_t *state, UWord word) {
	int reg = (int) ((word >> 8) & 0xF);
	state->awaitingKey = reg;
	return 0;
}
