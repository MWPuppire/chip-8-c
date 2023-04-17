#include <shared-internal.h>
#include <registers.h>
#include <memory.h>
#include <instructions.h>

// locations of font characters in memory
const UWord C8_SPRITE_ADDR[16] = {
	0x50, 0x55, 0x5A, 0x5F, 0x64, 0x69,
	0x6E, 0x73, 0x78, 0x7D, 0x82, 0x87,
	0x8C, 0x91, 0x96, 0x9B
};

int c8_setRegister(c8_state_t *state, UWord word) {
	UByte value = (UByte) (word & 0xFF);
	c8_register_t reg = (c8_register_t) ((word >> 8) & 0xF);
	c8_writeRegister(state, reg, value);
	return 0;
}

int c8_setIRegister(c8_state_t *state, UWord word) {
	UWord value = word & 0xFFF;
	state->regI = value;
	return 0;
}

int c8_regDump(c8_state_t *state, UWord word) {
	UWord addr = state->regI;
	c8_register_t max = (c8_register_t) ((word >> 8) & 0xF) + 1;
	for (c8_register_t i = C8_REG_0; i < max; i++) {
		UByte reg = c8_readRegister(state, i);
		c8_writeMemoryByte(state, addr++, reg);
	}
	state->regI = addr;
	return 0;
}

int c8_schipRegDump(c8_state_t *state, UWord word) {
	UWord addr = state->regI;
	c8_register_t max = (c8_register_t) ((word >> 8) & 0xF) + 1;
	for (c8_register_t i = C8_REG_0; i < max; i++) {
		UByte reg = c8_readRegister(state, i);
		c8_writeMemoryByte(state, addr++, reg);
	}
	return 0;
}

int c8_regLoad(c8_state_t *state, UWord word) {
	UWord addr = state->regI;
	c8_register_t max = (c8_register_t) ((word >> 8) & 0xF) + 1;
	for (c8_register_t i = C8_REG_0; i < max; i++) {
		UByte byte = c8_readMemoryByte(state, addr++);
		c8_writeRegister(state, i, byte);
	}
	state->regI = addr;
	return 0;
}

int c8_schipRegLoad(c8_state_t *state, UWord word) {
	UWord addr = state->regI;
	c8_register_t max = (c8_register_t) ((word >> 8) & 0xF) + 1;
	for (c8_register_t i = C8_REG_0; i < max; i++) {
		UByte byte = c8_readMemoryByte(state, addr++);
		c8_writeRegister(state, i, byte);
	}
	return 0;
}

int c8_moveRegister(c8_state_t *state, UWord word) {
	c8_register_t regX = (UByte) ((word >> 8) & 0xF);
	c8_register_t regY = (UByte) ((word >> 4) & 0xF);
	UByte value = c8_readRegister(state, regY);
	c8_writeRegister(state, regX, value);
	return 0;
}

int c8_spriteAddrI(c8_state_t *state, UWord word) {
	c8_register_t reg = (c8_register_t) ((word >> 8) & 0xF);
	UByte value = c8_readRegister(state, reg) & 0xF;
	state->regI = C8_SPRITE_ADDR[value];
	return 0;
}

int c8_persistentDump(c8_state_t *state, UWord word) {
	c8_register_t max = (c8_register_t) ((word >> 8) & 0xF) + 1;
	for (c8_register_t i = C8_REG_0; i < max; i++) {
		UByte reg = c8_readRegister(state, i);
		state->registerPersistent[i] = reg;
	}
	return 0;
}

int c8_persistentLoad(c8_state_t *state, UWord word) {
	c8_register_t max = (c8_register_t) ((word >> 8) & 0xF) + 1;
	for (c8_register_t i = C8_REG_0; i < max; i++) {
		UByte byte = state->registerPersistent[i];
		c8_writeRegister(state, i, byte);
	}
	return 0;
}
