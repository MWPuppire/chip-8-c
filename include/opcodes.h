#pragma once

#include "shared.h"

struct c8_instruction {
	char *disassembly;
	int (*execute)(c8_state_t *state, UWord instruction);
	int cycles;
};

int c8_instructionLookup(struct c8_instruction *inst, UWord word);
const char *c8_disassemble(c8_state_t *state, UWord pos);
