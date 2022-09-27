#include <shared.h>
#include <registers.h>

const char *C8_REGISTER_NAMES[C8_REGISTER_COUNT] = {
	"v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8", "v9",
	"vA", "vB", "vC", "vD", "vE", "vF", "Address", "PC"
};

int c8_registerByName(const char *str, c8_register_t *reg) {
	if (str[0] == 'I' && str[1] == '\0') {
		*reg = C8_REG_I;
		return 0;
	} else if (strlen(str) != 2) {
		return -1;
	} else if (str[0] == 'P' && str[1] == 'C') {
		*reg = C8_REG_PC;
		return 0;
	} else if (str[0] != 'v' && str[0] != 'V') {
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

UByte c8_readRegister(c8_state_t *state, c8_register_t reg) {
	switch (reg) {
	case C8_REG_0:  return state->registers.v0;
	case C8_REG_1:  return state->registers.v1;
	case C8_REG_2:  return state->registers.v2;
	case C8_REG_3:  return state->registers.v3;
	case C8_REG_4:  return state->registers.v4;
	case C8_REG_5:  return state->registers.v5;
	case C8_REG_6:  return state->registers.v6;
	case C8_REG_7:  return state->registers.v7;
	case C8_REG_8:  return state->registers.v8;
	case C8_REG_9:  return state->registers.v9;
	case C8_REG_A:  return state->registers.vA;
	case C8_REG_B:  return state->registers.vB;
	case C8_REG_C:  return state->registers.vC;
	case C8_REG_D:  return state->registers.vD;
	case C8_REG_E:  return state->registers.vE;
	case C8_REG_F:  return state->registers.vF;
	case C8_REG_I:  return state->registers.I;
	case C8_REG_PC: return state->registers.pc;
	default: return 0;
	}
}

void c8_writeRegister(c8_state_t *state, c8_register_t reg, UByte value) {
	switch (reg) {
	case C8_REG_0:  state->registers.v0 = value; break;
	case C8_REG_1:  state->registers.v1 = value; break;
	case C8_REG_2:  state->registers.v2 = value; break;
	case C8_REG_3:  state->registers.v3 = value; break;
	case C8_REG_4:  state->registers.v4 = value; break;
	case C8_REG_5:  state->registers.v5 = value; break;
	case C8_REG_6:  state->registers.v6 = value; break;
	case C8_REG_7:  state->registers.v7 = value; break;
	case C8_REG_8:  state->registers.v8 = value; break;
	case C8_REG_9:  state->registers.v9 = value; break;
	case C8_REG_A:  state->registers.vA = value; break;
	case C8_REG_B:  state->registers.vB = value; break;
	case C8_REG_C:  state->registers.vC = value; break;
	case C8_REG_D:  state->registers.vD = value; break;
	case C8_REG_E:  state->registers.vE = value; break;
	case C8_REG_F:  state->registers.vF = value; break;
	case C8_REG_I:  state->registers.I  = value; break;
	case C8_REG_PC: state->registers.pc = value; break;
	}
}
