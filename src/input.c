#include <shared.h>

#include <stdlib.h>
#include <stdbool.h>

void clearInput(struct emuState *state) {
	memset(state->input, 0, sizeof(bool) * 16);
}
