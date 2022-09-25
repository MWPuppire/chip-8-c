#include <stdio.h>
#include <stdlib.h>

#include <shared.h>
#include <memory.h>
#include <opcodes.h>
#include <instructions.h>

void instructionLookup(struct instruction *inst, UWord word) {
	switch (word & 0xF000) {
	case 0x0000:
		if (word == 0x0000) {
			inst->disassembly = "(void) 0;";
			inst->execute = nop;
			inst->cycles = 1;
		} else if (word == 0x00E0) {
			inst->disassembly = "display_clear();";
			inst->execute = displayClear;
			inst->cycles = 1;
		} else if (word == 0x00EE) {
			inst->disassembly = "return;";
			inst->execute = returnInst;
			inst->cycles = 1;
		} else {
			inst->disassembly = "call_routine(0xNNN);";
		}
		break;
	case 0x1000:
		inst->disassembly = "goto NNN;";
		inst->execute = gotoInst;
		inst->cycles = 1;
		break;
	case 0x2000:
		inst->disassembly = "*(0xNNN)();";
		inst->execute = callInst;
		inst->cycles = 1;
		break;
	case 0x3000:
		inst->disassembly = "if (Vx == NN) goto next;";
		inst->execute = ifEquals;
		inst->cycles = 1;
		break;
	case 0x4000:
		inst->disassembly = "if (Vx != NN) goto next;";
		inst->execute = ifNotEquals;
		inst->cycles = 1;
		break;
	case 0x5000:
		inst->disassembly = "if (Vx == Vy) goto next;";
		inst->execute = ifEqualsReg;
		inst->cycles = 1;
		break;
	case 0x6000:
		inst->disassembly = "Vx = NN;";
		inst->execute = setRegister;
		inst->cycles = 1;
		break;
	case 0x7000:
		inst->disassembly = "Vx += NN;";
		inst->execute = pluseqImmediate;
		inst->cycles = 1;
		break;
	case 0x8000:
		switch (word & 0xF) {
		case 0x0:
			inst->disassembly = "Vx = Vy;";
			inst->execute = moveRegister;
			inst->cycles = 1;
			break;
		case 0x1:
			inst->disassembly = "Vx |= Vy;";
			inst->execute = orRegister;
			inst->cycles = 1;
			break;
		case 0x2:
			inst->disassembly = "Vx &= Vy;";
			inst->execute = andRegister;
			inst->cycles = 1;
			break;
		case 0x3:
			inst->disassembly = "Vx ^= Vy;";
			inst->execute = xorRegister;
			inst->cycles = 1;
			break;
		case 0x4:
			inst->disassembly = "Vx += Vy;";
			inst->execute = pluseqRegister;
			inst->cycles = 1;
			break;
		case 0x5:
			inst->disassembly = "Vx -= Vy;";
			inst->execute = minuseqRegister;
			inst->cycles = 1;
			break;
		case 0x6:
			inst->disassembly = "Vx >>= 1;";
			inst->execute = shiftRegisterRight;
			inst->cycles = 1;
			break;
		case 0x7:
			inst->disassembly = "Vx = Vy - Vx;";
			inst->execute = subtractionRegister;
			inst->cycles = 1;
			break;
		case 0xE:
			inst->disassembly = "Vx <<= 1;";
			inst->execute = shiftRegisterLeft;
			inst->cycles = 1;
			break;
		}
		break;
	case 0x9000:
		inst->disassembly = "if (Vx != Vy) goto next;";
		inst->execute = ifNotEqualsReg;
		inst->cycles = 1;
		break;
	case 0xA000:
		inst->disassembly = "I = NNN;";
		inst->execute = setIRegister;
		inst->cycles = 1;
		break;
	case 0xB000:
		inst->disassembly = "PC = V0 + NNN;";
		inst->execute = jumpV0;
		inst->cycles = 1;
		break;
	case 0xC000:
		inst->disassembly = "Vx = rand() & NN;";
		inst->execute = randRegister;
		inst->cycles = 1;
		break;
	case 0xD000:
		inst->disassembly = "draw(Vx, Vy, N);";
		inst->execute = draw;
		inst->cycles = 1;
		break;
	case 0xE000:
		if ((word & 0xFF) == 0x9E) {
			inst->disassembly = "if (key() == Vx) goto next;";
//			inst->execute = keyEquals;
			inst->cycles = 1;
		} else if ((word & 0xFF) == 0xA1) {
			inst->disassembly = "if (key() != Vx) goto next;";
//			inst->execute = keyNotEquals;
			inst->cycles = 1;
		}
		break;
	case 0xF000:
		switch (word & 0xFF) {
		case 0x07:
			inst->disassembly = "Vx = get_delay();";
			inst->execute = getDelay;
			inst->cycles = 1;
			break;
		case 0x0A:
			inst->disassembly = "Vx = get_key();";
//			inst->execute = getKey;
			inst->cycles = 1;
			break;
		case 0x15:
			inst->disassembly = "delay_timer(Vx);";
			inst->execute = setDelayTimer;
			inst->cycles = 1;
			break;
		case 0x18:
			inst->disassembly = "sound_timer(Vx);";
			inst->execute = setSoundTimer;
			inst->cycles = 1;
			break;
		case 0x1E:
			inst->disassembly = "I += Vx;";
			inst->execute = addToI;
			inst->cycles = 1;
			break;
		case 0x29:
			inst->disassembly = "I = sprite_addr[Vx];";
			inst->execute = spriteAddrI;
			inst->cycles = 1;
			break;
		case 0x33:
			inst->disassembly = "set_bcd(I, Vx);";
			inst->execute = bcd;
			inst->cycles = 1;
			break;
		case 0x55:
			inst->disassembly = "reg_dump(Vx, &I);";
			inst->execute = regDump;
			inst->cycles = 1;
			break;
		case 0x65:
			inst->disassembly = "reg_load(Vx, &I);";
			inst->execute = regLoad;
			inst->cycles = 1;
			break;
		}
		break;
	}
}

void unknownOpcode(struct emuState *state, struct instruction inst) {
	UWord badInst = readMemoryWord(state, state->registers.pc);
	fprintf(stderr, "Unknown or unimplemented opcode at 0x%04X: %04X\n",
		state->registers.pc, badInst);
	fprintf(stderr, "%s\n", inst.disassembly);
}

const char *disassemble(struct emuState *state, UWord pos) {
	UWord word = readMemoryWord(state, pos);
	struct instruction inst;
	instructionLookup(&inst, word);
	return inst.disassembly;
}
