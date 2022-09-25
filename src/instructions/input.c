#include <shared.h>
#include <memory.h>
#include <registers.h>
#include <instructions.h>

int keyEquals(struct emuState *state, UWord word) {
	UByte reg = (word & 0xF00) >> 8;
	int key = (int) readRegister(state, reg);
	if (state->input[key])
		state->registers.pc += 2;
	return 0;
}

int keyNotEquals(struct emuState *state, UWord word) {
	UByte reg = (word & 0xF00) >> 8;
	int key = (int) readRegister(state, reg);
	if (!state->input[key])
		state->registers.pc += 2;
	return 0;
}

int getKey(struct emuState *state, UWord word) {
	UByte reg = (word & 0xF00) >> 8;
	state->awaitingKey = (int) reg;
	return 0;
}
