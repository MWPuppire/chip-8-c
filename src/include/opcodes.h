#pragma once

#include "shared.h"

typedef int (*c8_inst_execute_t)(c8_state_t *state, UWord instruction);

struct c8_instruction {
	char *disassembly;
	c8_inst_execute_t execute;
	int cycles;
};

c8_status_t c8_instructionLookup(c8_emu_mode_t mode, struct c8_instruction *inst, UWord word);
