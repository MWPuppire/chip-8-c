#include <stdio.h>
#include <stdlib.h>

#include <shared-internal.h>
#include <memory.h>
#include <opcodes.h>
#include <instructions.h>

// If I ever want more precise instruction timings:
// https://jackson-s.me/2019/07/13/Chip-8-Instruction-Scheduling-and-Frequency.html

int c8_opNotDefined(c8_state_t *UNUSED(state), UWord UNUSED(word)) {
	return 0;
}

const static c8_inst_execute_t regDumpTable[] = {
	c8_regDump,
	c8_schipRegDump,
	c8_regDump,
};
const static c8_inst_execute_t regLoadTable[] = {
	c8_regLoad,
	c8_schipRegLoad,
	c8_regLoad,
};
const static c8_inst_execute_t orRegisterTable[] = {
	c8_cosmacOrRegister,
	c8_orRegister,
	c8_orRegister,
};
const static c8_inst_execute_t andRegisterTable[] = {
	c8_cosmacAndRegister,
	c8_andRegister,
	c8_andRegister,
};
const static c8_inst_execute_t xorRegisterTable[] = {
	c8_cosmacXorRegister,
	c8_xorRegister,
	c8_xorRegister,
};
const static c8_inst_execute_t shiftRegisterRightTable[] = {
	c8_shiftRegisterRight,
	c8_schipShiftRegisterRight,
	c8_shiftRegisterRight,
};
const static c8_inst_execute_t shiftRegisterLeftTable[] = {
	c8_shiftRegisterLeft,
	c8_schipShiftRegisterLeft,
	c8_shiftRegisterLeft,
};
const static c8_inst_execute_t drawTable[] = {
	c8_cosmacDraw,
	c8_schipDraw,
	c8_xoDraw,
};
const static c8_inst_execute_t jumpV0Table[] = {
	c8_jumpV0,
	c8_schipJumpV0,
	c8_jumpV0,
};

c8_status_t c8_instructionLookup(c8_emu_mode_t mode, struct c8_instruction *inst, UWord word) {
	inst->execute = NULL;
	switch (word & 0xF000) {
	case 0x0000:
		if (word == 0x0000) {
			inst->disassembly = "(void) 0;";
			inst->execute = c8_nop;
			inst->cycles = 1;
		} else if ((word & 0x00F0) == 0x00C0) {
			inst->disassembly = "scroll_down(N);";
			if (mode == C8_CHIP_8) {
				inst->execute = c8_opNotDefined;
			}
		} else if ((word & 0x00F0) == 0x00D0) {
			inst->disassembly = "scroll_up(N);";
			if (mode != C8_XO_CHIP) {
				inst->execute = c8_opNotDefined;
			}
		} else if (word == 0x00E0) {
			inst->disassembly = "display_clear();";
			inst->execute = c8_displayClear;
			inst->cycles = 1;
		} else if (word == 0x00EE) {
			inst->disassembly = "return;";
			inst->execute = c8_returnInst;
			inst->cycles = 1;
		} else if (word == 0x00FB) {
			inst->disassembly = "scroll_right();";
			if (mode == C8_CHIP_8) {
				inst->execute = c8_opNotDefined;
			}
		} else if (word == 0x00FC) {
			inst->disassembly = "scroll_left();";
			if (mode == C8_CHIP_8) {
				inst->execute = c8_opNotDefined;
			}
		} else if (word == 0x00FD) {
			inst->disassembly = "exit();";
			inst->cycles = 1;
			if (mode == C8_CHIP_8) {
				inst->execute = c8_opNotDefined;
			} else {
				inst->execute = c8_exitInst;
			}
		} else if (word == 0x00FE) {
			inst->disassembly = "low_res();";
			inst->cycles = 1;
			if (mode == C8_CHIP_8) {
				inst->execute = c8_opNotDefined;
			} else {
				inst->execute = c8_lowres;
			}
		} else if (word == 0x00FF) {
			inst->disassembly = "high_res();";
			inst->cycles = 1;
			if (mode == C8_CHIP_8) {
				inst->execute = c8_opNotDefined;
			} else {
				inst->execute = c8_highres;
			}
		} else {
			inst->disassembly = "call_routine(0xNNN);";
		}
		break;
	case 0x1000:
		inst->disassembly = "goto NNN;";
		inst->execute = c8_gotoInst;
		inst->cycles = 1;
		break;
	case 0x2000:
		inst->disassembly = "*(0xNNN)();";
		inst->execute = c8_callInst;
		inst->cycles = 1;
		break;
	case 0x3000:
		inst->disassembly = "if (Vx == NN) goto next;";
		inst->execute = c8_ifEquals;
		inst->cycles = 1;
		break;
	case 0x4000:
		inst->disassembly = "if (Vx != NN) goto next;";
		inst->execute = c8_ifNotEquals;
		inst->cycles = 1;
		break;
	case 0x5000:
		if ((word & 0xF) == 0x0) {
			inst->disassembly = "if (Vx == Vy) goto next;";
			inst->cycles = 1;
			inst->execute = c8_ifEqualsReg;
		} else if ((word & 0xF) == 0x2) {
			inst->disassembly = "reg_dump(Vx, Vy, &I);";
			if (mode != C8_XO_CHIP) {
				inst->execute = c8_opNotDefined;
			}
		} else if ((word & 0xF) == 0x3) {
			inst->disassembly = "reg_load(Vx, Vy, &I);";
			if (mode != C8_XO_CHIP) {
				inst->execute = c8_opNotDefined;
			}
		}
		break;
	case 0x6000:
		inst->disassembly = "Vx = NN;";
		inst->execute = c8_setRegister;
		inst->cycles = 1;
		break;
	case 0x7000:
		inst->disassembly = "Vx += NN;";
		inst->execute = c8_pluseqImmediate;
		inst->cycles = 1;
		break;
	case 0x8000:
		switch (word & 0xF) {
		case 0x0:
			inst->disassembly = "Vx = Vy;";
			inst->execute = c8_moveRegister;
			inst->cycles = 1;
			break;
		case 0x1:
			inst->disassembly = "Vx |= Vy;";
			inst->execute = orRegisterTable[mode];
			inst->cycles = 1;
			break;
		case 0x2:
			inst->disassembly = "Vx &= Vy;";
			inst->execute = andRegisterTable[mode];
			inst->cycles = 1;
			break;
		case 0x3:
			inst->disassembly = "Vx ^= Vy;";
			inst->execute = xorRegisterTable[mode];
			inst->cycles = 1;
			break;
		case 0x4:
			inst->disassembly = "Vx += Vy;";
			inst->execute = c8_pluseqRegister;
			inst->cycles = 1;
			break;
		case 0x5:
			inst->disassembly = "Vx -= Vy;";
			inst->execute = c8_minuseqRegister;
			inst->cycles = 1;
			break;
		case 0x6:
			inst->disassembly = "Vx >>= 1;";
			inst->execute = shiftRegisterRightTable[mode];
			inst->cycles = 1;
			break;
		case 0x7:
			inst->disassembly = "Vx = Vy - Vx;";
			inst->execute = c8_subtractionRegister;
			inst->cycles = 1;
			break;
		case 0xE:
			inst->disassembly = "Vx <<= 1;";
			inst->execute = shiftRegisterLeftTable[mode];
			inst->cycles = 1;
			break;
		}
		break;
	case 0x9000:
		inst->disassembly = "if (Vx != Vy) goto next;";
		inst->execute = c8_ifNotEqualsReg;
		inst->cycles = 1;
		break;
	case 0xA000:
		inst->disassembly = "I = NNN;";
		inst->execute = c8_setIRegister;
		inst->cycles = 1;
		break;
	case 0xB000:
		inst->disassembly = "PC = V0 + NNN;";
		inst->execute = jumpV0Table[mode];
		inst->cycles = 1;
		break;
	case 0xC000:
		inst->disassembly = "Vx = rand() & NN;";
		inst->execute = c8_randRegister;
		inst->cycles = 1;
		break;
	case 0xD000:
		inst->disassembly = "draw(Vx, Vy, N);";
		inst->execute = drawTable[mode];
		inst->cycles = 1;
		break;
	case 0xE000:
		if ((word & 0xFF) == 0x9E) {
			inst->disassembly = "if (key() == Vx) goto next;";
			inst->execute = c8_keyEquals;
			inst->cycles = 1;
		} else if ((word & 0xFF) == 0xA1) {
			inst->disassembly = "if (key() != Vx) goto next;";
			inst->execute = c8_keyNotEquals;
			inst->cycles = 1;
		}
		break;
	case 0xF000:
		switch (word & 0xFF) {
		case 0x00:
			inst->disassembly = "I = read_and_skip_next_word();";
			if (mode != C8_XO_CHIP) {
				inst->execute = c8_opNotDefined;
			}
			break;
		case 0x01:
			inst->disassembly = "set_drawing_plane(N);";
			if (mode != C8_XO_CHIP) {
				inst->execute = c8_opNotDefined;
			}
			break;
		case 0x02:
			inst->disassembly = "load_audio_pattern(I);";
			if (mode != C8_XO_CHIP) {
				inst->execute = c8_opNotDefined;
			}
			break;
		case 0x07:
			inst->disassembly = "Vx = get_delay();";
			inst->execute = c8_getDelay;
			inst->cycles = 1;
			break;
		case 0x0A:
			inst->disassembly = "Vx = get_key();";
			inst->execute = c8_getKey;
			inst->cycles = 1;
			break;
		case 0x15:
			inst->disassembly = "delay_timer(Vx);";
			inst->execute = c8_setDelayTimer;
			inst->cycles = 1;
			break;
		case 0x18:
			inst->disassembly = "sound_timer(Vx);";
			inst->execute = c8_setSoundTimer;
			inst->cycles = 1;
			break;
		case 0x1E:
			inst->disassembly = "I += Vx;";
			inst->execute = c8_addToI;
			inst->cycles = 1;
			break;
		case 0x29:
			inst->disassembly = "I = sprite_addr[Vx];";
			inst->execute = c8_spriteAddrI;
			inst->cycles = 1;
			break;
		case 0x30:
			inst->disassembly = "I = digit_addr[Vx];";
			if (mode == C8_CHIP_8) {
				inst->execute = c8_opNotDefined;
			}
			break;
		case 0x33:
			inst->disassembly = "set_bcd(I, Vx);";
			inst->execute = c8_bcd;
			inst->cycles = 1;
			break;
		case 0x3A:
			inst->disassembly = "set_audio_hertz(Vx)";
			if (mode != C8_XO_CHIP) {
				inst->execute = c8_opNotDefined;
			}
			break;
		case 0x55:
			inst->disassembly = "reg_dump(V0, Vx, &I);";
			inst->execute = regDumpTable[mode];
			inst->cycles = 1;
			break;
		case 0x65:
			inst->disassembly = "reg_load(V0, Vx, &I);";
			inst->execute = regLoadTable[mode];
			inst->cycles = 1;
			break;
		case 0x75:
			inst->disassembly = "persist_dump(Vx);";
			inst->cycles = 1;
			if (mode == C8_CHIP_8) {
				inst->execute = c8_opNotDefined;
			} else {
				inst->execute = c8_persistentDump;
			}
			break;
		case 0x85:
			inst->disassembly = "persist_load(Vx);";
			inst->cycles = 1;
			if (mode == C8_CHIP_8) {
				inst->execute = c8_opNotDefined;
			} else {
				inst->execute = c8_persistentLoad;
			}
			break;
		}
		break;
	}
	if (inst->execute == NULL) {
		return C8_UNKNOWN_OP;
	} else if (inst->execute == c8_opNotDefined) {
		return C8_OP_NOT_DEFINED;
	} else {
		return C8_OK;
	}
}
