#include <shared.h>
#include <registers.h>

const char *REGISTER_NAMES[REGISTER_COUNT] = {
	"v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8", "v9",
	"vA", "vB", "vC", "vD", "vE", "vF", "Address", "PC"
};

int registerByName(const char *str, UByte *reg) {
	if (strlen(str) != 2) {
		return -1;
	} else if (str[0] != 'v' && str[0] != 'V') {
		return -1;
	} else if (str[1] >= '0' && str[1] <= '9') {
		*reg = str[1] - '0';
		return 0;
	} else if (str[1] >= 'A' && str[1] <= 'F') {
		*reg = str[1] - 'A' + 10;
		return 0;
	} else if (str[1] >= 'a' && str[1] <= 'f') {
		*reg = str[1] - 'a' + 10;
		return 0;
	} else {
		return -1;
	}
}

void resetRegisters(struct emuState *state) {
	state->registers.v0 = 0;
	state->registers.v1 = 0;
	state->registers.v2 = 0;
	state->registers.v3 = 0;
	state->registers.v4 = 0;
	state->registers.v5 = 0;
	state->registers.v6 = 0;
	state->registers.v7 = 0;
	state->registers.v8 = 0;
	state->registers.v9 = 0;
	state->registers.vA = 0;
	state->registers.vB = 0;
	state->registers.vC = 0;
	state->registers.vD = 0;
	state->registers.vE = 0;
	state->registers.vF = 0;
	state->registers.I  = 0;
	state->registers.pc = 0x0200;
}

UByte readRegister(struct emuState *state, UByte reg) {
	switch (reg) {
	case 0x0: return state->registers.v0;
	case 0x1: return state->registers.v1;
	case 0x2: return state->registers.v2;
	case 0x3: return state->registers.v3;
	case 0x4: return state->registers.v4;
	case 0x5: return state->registers.v5;
	case 0x6: return state->registers.v6;
	case 0x7: return state->registers.v7;
	case 0x8: return state->registers.v8;
	case 0x9: return state->registers.v9;
	case 0xA: return state->registers.vA;
	case 0xB: return state->registers.vB;
	case 0xC: return state->registers.vC;
	case 0xD: return state->registers.vD;
	case 0xE: return state->registers.vE;
	case 0xF: return state->registers.vF;
	}
	return 0;
}

void writeRegister(struct emuState *state, UByte reg, UByte value) {
	switch (reg) {
	case 0x0: state->registers.v0 = value; break;
	case 0x1: state->registers.v1 = value; break;
	case 0x2: state->registers.v2 = value; break;
	case 0x3: state->registers.v3 = value; break;
	case 0x4: state->registers.v4 = value; break;
	case 0x5: state->registers.v5 = value; break;
	case 0x6: state->registers.v6 = value; break;
	case 0x7: state->registers.v7 = value; break;
	case 0x8: state->registers.v8 = value; break;
	case 0x9: state->registers.v9 = value; break;
	case 0xA: state->registers.vA = value; break;
	case 0xB: state->registers.vB = value; break;
	case 0xC: state->registers.vC = value; break;
	case 0xD: state->registers.vD = value; break;
	case 0xE: state->registers.vE = value; break;
	case 0xF: state->registers.vF = value; break;
	}
}
