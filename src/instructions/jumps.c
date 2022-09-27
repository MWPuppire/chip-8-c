#include <shared.h>
#include <call.h>
#include <instructions.h>
#include <registers.h>

int gotoInst(struct emuState *state, UWord word) {
	UWord dest = word & 0xFFF;
	state->registers.pc = dest;
	return 0;
}

int callInst(struct emuState *state, UWord word) {
	UWord pos = word & 0xFFF;
	callRoutine(state, pos);
	return 0;
}

int returnInst(struct emuState *state, UWord UNUSED(word)) {
	returnRoutine(state);
	return 0;
}

int jumpV0(struct emuState *state, UWord word) {
	UWord value = word & 0xFFF;
#ifdef SCHIP
	UWord offset = readRegister(state, (word >> 8) & 0xF);
#else
	UWord offset = state->registers.v0;
#endif
	state->registers.pc = value + offset;
	return 0;
}

int jumpOffset(struct emuState *state, UWord word) {
	UWord value = word & 0xFFF;
	UWord offset = readRegister(state, (word >> 8) & 0xF);
	state->registers.pc = value + offset;
	return 0;
}
