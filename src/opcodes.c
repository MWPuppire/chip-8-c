#include <stdio.h>
#include <stdlib.h>

#include <shared-internal.h>
#include <memory.h>
#include <opcodes.h>
#include <instructions.h>

// If I ever want more precise instruction timings:
// https://jackson-s.me/2019/07/13/Chip-8-Instruction-Scheduling-and-Frequency.html

int c8_instructionLookup(struct c8_instruction *inst, UWord word) {
	inst->execute = NULL;
	switch (word & 0xF000) {
	case 0x0000:
		if (word == 0x0000) {
			inst->disassembly = "(void) 0;";
			inst->execute = c8_nop;
			inst->cycles = 1;
#if defined(SCHIP) || defined(XO_CHIP)
		} else if ((word & 0x00F0) == 0x00C0) {
			inst->disassembly = "scroll_down(N);";
#endif
#ifdef XO_CHIP
		} else if ((word & 0x00F0) == 0x00D0) {
			inst->disassembly = "scroll_up(N);";
#endif
		} else if (word == 0x00E0) {
			inst->disassembly = "display_clear();";
			inst->execute = c8_displayClear;
			inst->cycles = 1;
		} else if (word == 0x00EE) {
			inst->disassembly = "return;";
			inst->execute = c8_returnInst;
			inst->cycles = 1;
#if defined(SCHIP) || defined(XO_CHIP)
		} else if (word == 0x00FB) {
			inst->disassembly = "scroll_right();";
		} else if (word == 0x00FC) {
			inst->disassembly = "scroll_left();";
		} else if (word == 0x00FD) {
			inst->disassembly = "exit();";
			inst->execute = c8_exitInst;
			inst->cycles = 1;
		} else if (word == 0x00FE) {
			inst->disassembly = "low_res();";
			inst->execute = c8_lowres;
			inst->cycles = 1;
		} else if (word == 0x00FF) {
			inst->disassembly = "high_res();";
			inst->execute = c8_highres;
			inst->cycles = 1;
#endif
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
#ifdef XO_CHIP
	case 0x5000:
		if ((word & 0xF) == 0x0) {
			inst->disassembly = "if (Vx == Vy) goto next;";
			inst->execute = c8_ifEqualsReg;
			inst->cycles = 1;
		} else if ((word & 0xF) == 0x2) {
			inst->disassembly = "reg_dump(Vx, Vy, &I);";
		} else if ((word & 0xF) == 0x3) {
			inst->disassembly = "reg_load(Vx, Vy, &I);";
		}
		break;
#else
	case 0x5000:
		inst->disassembly = "if (Vx == Vy) goto next;";
		inst->execute = c8_ifEqualsReg;
		inst->cycles = 1;
		break;
#endif
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
			inst->execute = c8_orRegister;
			inst->cycles = 1;
			break;
		case 0x2:
			inst->disassembly = "Vx &= Vy;";
			inst->execute = c8_andRegister;
			inst->cycles = 1;
			break;
		case 0x3:
			inst->disassembly = "Vx ^= Vy;";
			inst->execute = c8_xorRegister;
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
			inst->execute = c8_shiftRegisterRight;
			inst->cycles = 1;
			break;
		case 0x7:
			inst->disassembly = "Vx = Vy - Vx;";
			inst->execute = c8_subtractionRegister;
			inst->cycles = 1;
			break;
		case 0xE:
			inst->disassembly = "Vx <<= 1;";
			inst->execute = c8_shiftRegisterLeft;
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
		inst->execute = c8_jumpV0;
		inst->cycles = 1;
		break;
	case 0xC000:
		inst->disassembly = "Vx = rand() & NN;";
		inst->execute = c8_randRegister;
		inst->cycles = 1;
		break;
	case 0xD000:
		inst->disassembly = "draw(Vx, Vy, N);";
		inst->execute = c8_draw;
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
#ifdef XO_CHIP
		case 0x00:
			inst->disassembly = "I = read_and_skip_next_word();";
			break;
		case 0x01:
			inst->disassembly = "set_drawing_plane(N);";
			break;
		case 0x02:
			inst->disassembly = "load_audio_pattern(I);";
			break;
#endif
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
#if defined(SCHIP) || defined(XO_CHIP)
		case 0x30:
			inst->disassembly = "I = digit_addr[Vx];";
			break;
#endif
		case 0x33:
			inst->disassembly = "set_bcd(I, Vx);";
			inst->execute = c8_bcd;
			inst->cycles = 1;
			break;
#ifdef XO_CHIP
		case 0x3A:
			inst->disassembly = "set_audio_hertz(Vx)";
			break;
#endif
		case 0x55:
			inst->disassembly = "reg_dump(V0, Vx, &I);";
			inst->execute = c8_regDump;
			inst->cycles = 1;
			break;
		case 0x65:
			inst->disassembly = "reg_load(V0, Vx, &I);";
			inst->execute = c8_regLoad;
			inst->cycles = 1;
			break;
#if defined(SCHIP) || defined(XO_CHIP)
		case 0x75:
			inst->disassembly = "persist_dump(Vx);";
			inst->execute = c8_persistentDump;
			inst->cycles = 1;
			break;
		case 0x85:
			inst->disassembly = "persist_load(Vx);";
			inst->execute = c8_persistentLoad;
			inst->cycles = 1;
			break;
#endif
		}
		break;
	}
	return inst->execute == NULL;
}
