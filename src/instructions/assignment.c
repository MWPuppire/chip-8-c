#include <shared.h>
#include <registers.h>
#include <memory.h>
#include <font.h>
#include <instructions.h>

int c8_setRegister(c8_state_t *state, UWord word) {
	UByte value = (UByte) (word & 0xFF);
	c8_register_t reg = (c8_register_t) ((word >> 8) & 0xF);
	c8_writeRegister(state, reg, value);
	return 0;
}

int c8_setIRegister(c8_state_t *state, UWord word) {
	UWord value = word & 0xFFF;
	state->registers.I = value;
	return 0;
}

int c8_regDump(c8_state_t *state, UWord word) {
	UWord addr = state->registers.I;
	c8_register_t max = (c8_register_t) ((word >> 8) & 0xF) + 1;
	for (c8_register_t i = C8_REG_0; i < max; i++) {
		UByte reg = c8_readRegister(state, i);
		c8_writeMemoryByte(state, addr++, reg);
	}
#ifndef SCHIP
	state->registers.I = addr;
#endif
	return 0;
}

int c8_regLoad(c8_state_t *state, UWord word) {
	UWord addr = state->registers.I;
	c8_register_t max = (c8_register_t) ((word >> 8) & 0xF) + 1;
	for (c8_register_t i = C8_REG_0; i < max; i++) {
		UByte byte = c8_readMemoryByte(state, addr++);
		c8_writeRegister(state, i, byte);
	}
#ifndef SCHIP
	state->registers.I = addr;
#endif
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
	state->registers.I = C8_SPRITE_ADDR[value];
	return 0;
}
