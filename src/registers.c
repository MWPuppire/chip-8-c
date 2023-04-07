#include <string.h>

#include <shared-internal.h>
#include <registers.h>

const char *C8_REGISTER_NAMES[C8_REGISTER_COUNT] = {
	"v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8", "v9",
	"vA", "vB", "vC", "vD", "vE", "vF"
};

int c8_registerByName(const char *str, c8_register_t *reg) {
	if (strlen(str) != 2 || (str[0] != 'v' && str[0] != 'V')) {
		return -1;
	} else if (str[1] >= '0' && str[1] <= '9') {
		*reg = (c8_register_t) (str[1] - '0');
		return 0;
	} else if (str[1] >= 'A' && str[1] <= 'F') {
		*reg = (c8_register_t) (str[1] - 'A' + 10);
		return 0;
	} else if (str[1] >= 'a' && str[1] <= 'f') {
		*reg = (c8_register_t) (str[1] - 'a' + 10);
		return 0;
	} else {
		return -1;
	}
}

void c8_resetRegisters(c8_state_t *state) {
	memset(state->registers, 0, sizeof(UByte) * 16);
	state->regI = 0;
	state->regPC = 0x0200;
}

UByte c8_readRegister(c8_state_t *state, c8_register_t reg) {
	return state->registers[reg];
}
void c8_writeRegister(c8_state_t *state, c8_register_t reg, UByte value) {
	state->registers[reg] = value;
}

UWord c8_readAddressRegister(c8_state_t *state) {
	return state->regI;
}
void c8_writeAddressRegister(c8_state_t *state, UWord value) {
	state->regI = value;
}

UWord c8_readPC(c8_state_t *state) {
	return state->regPC;
}
void c8_writePC(c8_state_t *state, UWord value) {
	state->regPC = value;
}
