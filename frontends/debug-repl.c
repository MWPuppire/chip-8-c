#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <readline/readline.h>

#include <chip8.h>

#define TJE_IMPLEMENTATION
#include "tiny_jpeg.h"

int strncmpcase(const char *s1, const char *s2, size_t len) {
	if (s1 == NULL || s2 == NULL)
		return s2 == NULL ? s1 == NULL : -1;
	for (size_t i = 0; i < len; s1++, s2++, i++) {
		int diff = tolower((unsigned char) *s1) - tolower((unsigned char) *s2);
		if (diff != 0 || !*s1)
			return diff;
	}
	return 0;
}

const int COMMAND_START = 1;
const int COMMAND_COUNT = 23;
enum command {
	NO_COMMAND,
	BACKTRACE, BRK, CYCLES, DISASSEMBLE, DUMP_DISPLAY, DUMP_MEMORY, FINISH,
	HELP, LISTBRK, LOAD_MEMORY, LOAD_ROM, LOG_INSTRUCTIONS, NEXT, READ,
	REBOOT, REGS, RESUME, SETREG, STEP, TIMERS, QUIT, WRITE
};
const char *COMMAND_NAMES[COMMAND_COUNT] = {
	"MISSING", "backtrace", "brk", "cycles", "disassemble", "dump_display",
	"dump_memory", "finish", "help", "listbrk", "load_memory", "load_rom",
	"log_instructions", "next", "read", "reboot", "regs", "resume",
	"setreg", "step", "timers", "quit", "write"
};
const char *COMMAND_HELP[COMMAND_COUNT] = {
	"NO COMMAND",
	"backtrace - display the current call stack",
	"brk [x] - halt when PC reaches <x>, or remove the breakpoint",
	"cycles - display the number of CPU cycles since the last usage",
	"disassemble [x] - disassemble the instruction at <x> or current",
	"dump_display <file> - write the display to JPEG <file>",
	"dump_memory <file> - write memory to binary <file>",
	"finish - run until teh current function returns",
	"help [cmd] - display this help text or the text for <cmd>",
	"listbrk - list all breakpoints",
	"load_memory <file> - load memory from binary <file>",
	"load_rom <file> - load a new ROM <file>, clearing memory",
	"log_instructions [yes|no] - set whether to log executed instructions",
	"next - run the next instruction without calling a function",
	"read <x> - read byte at memory <x> and display it",
	"reboot - shut down and reboot CPU, clearing state",
	"regs - dump all registers",
	"resume - start or continue execution",
	"setreg <x> <y> - set register <x> to <y>",
	"step - execute only the next instruction",
	"timers - display the current timers status",
	"quit - terminate the program",
	"write <x> <y> - write byte <y> to memory <x>",
};

const int COMMAND_ARGC[COMMAND_COUNT] = {
	-1, 1, 1, 1, 1, 2, 2, 1, 1, 1, 2, 2,
	1, 1, 2, 1, 1, 1, 3, 1, 1, 1, 3
};

enum execState { PAUSED = 0, RUNNING = 1 };

struct {
	struct emuState *state;
	enum execState running;
	bool logging;
	bool hasRom;
	int brk;
	int cycles;
} state;

char *readfile(const char *name, size_t *size) {
	FILE *f = fopen(name, "rb");
	if (f == NULL) {
		*size = 0;
		return NULL;
	}
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	char *str = malloc(fsize + 1);
	if (str == NULL) {
		*size = 0;
		return NULL;
	}

	fread(str, 1, fsize, f);
	fclose(f);

	str[fsize] = '\0';

	*size = fsize;
	return str;
}

int writefile(const char *name, const unsigned char *buf, size_t size) {
	FILE *f = fopen(name, "wb");
	if (f == NULL)
		return 1;

	size_t written = fwrite(buf, 1, size, f);
	if (written != size)
		return 1;

	fclose(f);
	return 0;
}

int readROM(struct emuState *state, char *file) {
	size_t size;
	char *rom = readfile(file, &size);
	if (rom == NULL)
		return 1;

	int fail = loadROM(state, rom, size);
	free(rom);
	if (fail)
		return 1;

	return 0;
}

int dumpDisplay(struct emuState *state, const char *file) {
	const static int NUM_COMPONENTS = 4;
	static unsigned char data[NUM_COMPONENTS * SCREEN_WIDTH * SCREEN_HEIGHT] = { 0 };
	for (int x = 0; x < SCREEN_WIDTH; x++) {
		for (int y = 0; y < SCREEN_HEIGHT; y++) {
			unsigned char val = readFromScreen(state, x, y) ? 0xFF : 0x00;
			data[(x + y * SCREEN_WIDTH) * NUM_COMPONENTS + 0] = val;
			data[(x + y * SCREEN_WIDTH) * NUM_COMPONENTS + 1] = val;
			data[(x + y * SCREEN_WIDTH) * NUM_COMPONENTS + 2] = val;
			data[(x + y * SCREEN_WIDTH) * NUM_COMPONENTS + 3] = 0xFF;
		}
	}
	return !tje_encode_to_file(file, SCREEN_WIDTH, SCREEN_HEIGHT, NUM_COMPONENTS, data);
}

void sigIntHandler(int signal) {
	assert(signal == SIGINT);
	if (state.running) {
		state.running = 0;
		printf("(To exit, press CTRL+C again or run `quit`)\n");
		return;
	}
	exit(0);
}

char *commandGenerator(const char *text, int state) {
	static int idx, len;

	if (state == 0) {
		idx = COMMAND_START;
		len = strlen(text);
	}

	while (idx < COMMAND_COUNT) {
		const char *name = COMMAND_NAMES[idx++];
		if (strncmpcase(name, text, len) == 0) {
			return strdup(name);
		}
	}

	return NULL;
}

char **commandCompletion(const char *text, int start, int UNUSED(end)) {
	rl_attempted_completion_over = 1;
	if (start == 0)
		return rl_completion_matches(text, commandGenerator);
	return NULL;
}

char **splitString(const char *str, const char *delims, int *total) {
	size_t len = strlen(str);
	char *copy = strdup(str);
	int count = 0;
	int inString = 0;

	for (size_t i = 0; i < len; i++) {
		if (strchr(delims, copy[i]) != NULL) {
			copy[i] = '\0';
			inString = 0;
		} else if (inString == 0) {
			count++;
			inString = 1;
		}
	}

	inString = 0;
	char **out = malloc(sizeof(char *) * (count + 1));
	int i = 0;
	size_t idx = 0;
	while (i < count) {
		if (inString == 0 && copy[idx] != '\0') {
			inString = 1;
			out[i++] = copy + idx;
		} else if (copy[idx] == '\0') {
			inString = 0;
		}
		idx++;
	}
	out[count] = NULL;

	*total = count;
	return out;
}

enum command matchCommand(const char *cmd) {
	size_t len = strlen(cmd);
	for (enum command id = COMMAND_START; id < COMMAND_COUNT; id++) {
		if (strncmpcase(cmd, COMMAND_NAMES[id], len) == 0)
			return id;
	}
	return NO_COMMAND;
}

int parseByte(const char *str, UByte *out) {
	char *end;
	unsigned long val = strtoul(str, &end, 0);
	if (end == str) {
		printf("Invalid number.\n");
		return -1;
	}
	if (val >= 0x100) {
		printf("Number out of bounds for byte.\n");
		return -1;
	}
	*out = val;
	return 0;
}

int parseWord(const char *str, UWord *out) {
	char *end;
	unsigned long val = strtoul(str, &end, 0);
	if (end == str) {
		printf("Invalid number.\n");
		return -1;
	}
	if (val >= 0x10000) {
		printf("Number out of bounds for word.\n");
		return -1;
	}
	*out = val;
	return 0;
}

void executeCommand(const char *cmd) {
	int argc;
	char **args = splitString(cmd, " \t", &argc);
	if (argc < 1)
		return;
	char *base = args[0];

	enum command commandId = matchCommand(base);

	if (argc < COMMAND_ARGC[commandId]) {
		printf("Missing arguments for command `%s'.\n", base);
		printf("For help, use `help'.\n");
		goto command_cleanup;
	}

	struct emuState *emu = state.state;

	switch (commandId) {
	case NO_COMMAND: {
		printf("Unknown command `%s'.\n", base);
		printf("For help, use `help'.\n");
		break;
	}
	case BACKTRACE: {
		printf("Not implemented\n");
		break;
	}
	case BRK: {
		if (argc < 2) {
			state.brk = -1;
			break;
		}
		UWord loc;
		if (parseWord(args[1], &loc) != 0) {
			break;
		}
		state.brk = loc;
		break;
	}
	case CYCLES: {
		printf("%d cycles since last call.\n", state.cycles);
		state.cycles = 0;
		break;
	}
	case DISASSEMBLE: {
		UWord pos = emu->registers.pc;
		if (argc > 1) {
			if (parseWord(args[1], &pos) != 0) {
				break;
			}
		}
		printf("%s\n", disassemble(emu, pos));
		break;
	}
	case DUMP_DISPLAY: {
		int fail = dumpDisplay(emu, args[1]);
		if (fail)
			printf("Failed to write to file.\n");
		break;
	}
	case DUMP_MEMORY: {
		int fail = writefile(args[1], emu->memory, 0x1000);
		if (fail)
			printf("Failed to write to file.\n");
		break;
	}
	case FINISH: {
		printf("Not implemented\n");
		break;
	}
	case HELP: {
		if (argc > 1) {
			enum command helpCmd = matchCommand(args[1]);
			if (helpCmd == NO_COMMAND) {
				printf("Unknown command `%s'.\n", base);
				printf("For help, use `help'.\n");
			} else {
				printf("%s\n", COMMAND_HELP[(int) helpCmd]);
			}
		} else {
			for (int i = COMMAND_START; i < COMMAND_COUNT; i++)
				printf("%s\n", COMMAND_HELP[i]);
		}
		break;
	}
	case LISTBRK: {
		if (state.brk == -1) {
			printf("No breakpoints\n");
		} else {
			printf("0: %x\n", state.brk);
		}
		break;
	}
	case LOAD_MEMORY: {
		size_t size;
		const char *buf = readfile(args[1], &size);
		if (size < 0x1000) {
			printf("File too small to write as memory.\n");
			break;
		}
		memcpy(emu->memory, buf, 0x1000);
		break;
	}
	case LOAD_ROM: {
		if (readROM(emu, args[1]))
			printf("Error loading ROM.\n");
		else
			state.hasRom = true;
		break;
	}
	case LOG_INSTRUCTIONS: {
		if (argc > 1) {
			const char *arg = args[1];
			if (strncmpcase(arg, "yes", 3) == 0) {
				state.logging = true;
			} else if (strncmpcase(arg, "no", 2) == 0) {
				state.logging = false;
			} else {
				printf("Must provide yes or no.\n");
			}
		} else {
			state.logging = !state.logging;
		}
		break;
	}
	case NEXT: {
		printf("Not implemented\n");
		break;
	}
	case READ: {
		UWord loc;
		if (parseWord(args[1], &loc) != 0) {
			break;
		}
		printf("0x%02x\n", readMemoryByte(emu, loc));
		break;
	}
	case REBOOT: {
		cpuBoot(emu);
		state.hasRom = false;
		break;
	}
	case REGS: {
		for (int i = 0; i < 8; i++)
			printf("v%x: %02x\tv%x: %02x\n", i, readRegister(emu, i),
				i + 8, readRegister(emu, i + 8));
		printf("Program Counter: %04x\n", emu->registers.pc);
		printf("Address: %04x\n", emu->registers.I);
		break;
	}
	case RESUME: {
		if (!state.hasRom) {
			printf("No ROM loaded.\n");
			break;
		}
		state.running = RUNNING;
		break;
	}
	case SETREG: {
		printf("Not implemented\n");
		break;
	}
	case STEP: {
		if (!state.hasRom) {
			printf("No ROM loaded.\n");
			break;
		}
		state.cycles += cpuStep(emu);
		break;
	}
	case TIMERS: {
		printf("Delay timer: %02x\n", emu->delayTimer);
		printf("Sound timer: %02x\n", emu->soundTimer);
		break;
	}
	case QUIT: {
		exit(1);
		break;
	}
	case WRITE: {
		UWord loc;
		if (parseWord(args[1], &loc) != 0) {
			break;
		}
		UByte byte;
		if (parseByte(args[2], &byte) != 0) {
			break;
		}
		writeMemoryByte(emu, loc, byte);
		break;
	}
	}

command_cleanup:
	free(base);
	free(args);
}

int main(int argc, char *argv[]) {
	// drop the program name
	argc--; argv++;

	struct emuState *emu = state.state = malloc(sizeof(struct emuState));
	cpuBoot(emu);

	state.brk = -1;

	if (argc > 0) {
		int fail = readROM(emu, argv[0]);
		if (fail)
			fprintf(stderr, "Error loading ROM.\n");
		else
			state.hasRom = true;
	}

	if (signal(SIGINT, sigIntHandler) == SIG_ERR) {
		fprintf(stderr, "Error setting signal handler.\n");
		return 1;
	}

	rl_attempted_completion_function = commandCompletion;

	long double lasttime = 0;

	while (true) {
		if (state.running) {
			long double dt = deltatime(&lasttime);
			int startCycles = emu->cycleDiff + dt * CLOCK_SPEED;
			int fail = emulateUntil(emu, dt, state.brk);
			if (fail)
				state.running = PAUSED;
			int endCycles = emu->cycleDiff;
			state.cycles += startCycles - endCycles;
		} else {
			char *buffer = readline("> ");
			if (!buffer)
				continue;
			add_history(buffer);

			executeCommand(buffer);
			free(buffer);
			if (state.running)
				deltatime(&lasttime);
		}
	}

	return 0;
}
