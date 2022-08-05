#pragma once

#include "shared.h"

struct instruction {
	char *disassembly;
	int (*execute)(struct emuState *state, UWord instruction);
	int cycles;
};

void instructionLookup(struct instruction *inst, UWord word);
void unknownOpcode(struct emuState *state, struct instruction inst);
const char *disassemble(struct emuState *state, UWord pos);
