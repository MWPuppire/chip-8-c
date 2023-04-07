#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#ifdef DEBUG_REPL
#	include <ctype.h>
#	include <assert.h>
#	include <signal.h>
#	include <readline/readline.h>
#	define TJE_IMPLEMENTATION
#	include "tiny_jpeg.h"
#endif

#include <chip8.h>

#include <SDL2/SDL.h>

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
inline static double deltatime(double *lasttime) {
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	double time = (double) ts.tv_sec + (double) ts.tv_nsec / 1e9;
	double diff = time - (lasttime == 0 ? 0.0 : *lasttime);
	lasttime != 0 && (*lasttime = time);
	return diff;
}
#else
inline static double deltatime(double *lasttime) {
	clock_t val = clock();
	double time = (double) val / CLOCKS_PER_SEC;
	double diff = time - (lasttime == 0 ? 0.0 : *lasttime);
	lasttime != 0 && (*lasttime = time);
	return diff;
}
#endif

#ifdef DEBUG_REPL
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
const int COMMAND_COUNT = 28;
enum command {
	NO_COMMAND,
	BACKTRACE, BRK, CYCLES, DISASSEMBLE, DUMP_DISPLAY, DUMP_MEMORY, FINISH,
	GOTO, HELP, LISTBRK, LOAD_MEMORY, LOAD_ROM, NEXT, PAUSE, READ, REBOOT,
	RECVKEY, REGS, REMBRK, RESUME, SETADDR, SETREG, STEP, TIMERS,
	TOGGLE_KEY, QUIT, WRITE
};
const char *COMMAND_NAMES[COMMAND_COUNT] = {
	"MISSING", "backtrace", "brk", "cycles", "disassemble", "dump_display",
	"dump_memory", "finish", "goto", "help", "listbrk", "load_memory",
	"load_rom", "next", "pause", "read", "reboot", "recvkey", "regs",
	"rembrk", "resume", "setaddr", "setreg", "step", "timers", "toggle_key",
	"quit", "write"
};
const char *COMMAND_HELP[COMMAND_COUNT] = {
	"NO COMMAND",
	"backtrace - display the current call stack",
	"brk <x> - halt when PC reaches <x>",
	"cycles - display the number of CPU cycles since the last usage",
	"disassemble [x] - disassemble the instruction at <x> or current",
	"dump_display <file> - write the display to JPEG <file>",
	"dump_memory <file> - write memory to binary <file>",
	"finish - run until the current function returns",
	"goto <x> - set PC to <x>",
	"help [cmd] - display this help text or the text for <cmd>",
	"listbrk - list all breakpoints by index",
	"load_memory <file> - load memory from binary <file>",
	"load_rom <file> - load a new ROM <file>, clearing memory",
	"next - print the next instruction without executing it",
	"pause - pause execution",
	"read <x> - read byte at memory <x> and display it",
	"reboot - shut down and reboot CPU, clearing state",
	"recvkey <key> - effectively press and release <key>",
	"regs - dump all registers",
	"rembrk <x> - remove the breakpoint at index <x>",
	"resume - start or continue execution",
	"setaddr <x> - set the address register to <x>",
	"setreg <x> <y> - set register <x> to <y>",
	"step - execute only the next instruction",
	"timers - display the current timers status",
	"toggle_key <key> - toggle holding a key down",
	"quit - terminate the program",
	"write <x> <y> - write byte <y> to memory <x>",
};

const int COMMAND_ARGC[COMMAND_COUNT] = {
	-1, 1, 2, 1, 1, 2, 2, 1, 2, 1, 1, 2, 2, 1,
	1, 2, 1, 2, 1, 2, 1, 2, 3, 1, 1, 2, 1, 3
};
#endif

enum execState { PAUSED = 0, RUNNING = 1 };

struct {
	c8_state_t *state;
	enum execState running;
	bool hasRom;
	int cycles;
	int brk[16];
	int brkidx;
	double lasttime;
#ifdef DEBUG_REPL
	SDL_mutex *mutex;
#endif
} state;

#ifndef PIXEL_SCALE
// default to eight pixels for every CHIP-8 pixel
// completely arbitrary, but it should work well enough
#	define PIXEL_SCALE 8
#endif

unsigned char *readfile(const char *name, size_t *size) {
	FILE *f = fopen(name, "rb");
	if (f == NULL) {
		*size = 0;
		return NULL;
	}
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	unsigned char *str = malloc(fsize + 1);
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

int readROM(c8_state_t *state, char *file) {
	size_t size;
	unsigned char *rom = readfile(file, &size);
	if (rom == NULL)
		return 1;

	int fail = c8_loadROM(state, rom, size);
	free(rom);
	if (fail)
		return 1;

	return 0;
}

#ifdef DEBUG_REPL
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

int dumpMemory(c8_state_t *state, const char *file) {
	unsigned char data[0x1000];
	c8_dumpMemory(state, data, 0x1000);
	return writefile(file, data, 0x1000);
}

int dumpDisplay(c8_state_t *state, const char *file) {
	unsigned char data[4 * C8_SCREEN_WIDTH * C8_SCREEN_HEIGHT];
	for (int x = 0; x < C8_SCREEN_WIDTH; x++) {
		for (int y = 0; y < C8_SCREEN_HEIGHT; y++) {
			UByte pixelSet = c8_readFromScreen(state, x, y);
			unsigned char color = pixelSet ? 0xFF : 0x00;
			data[(x + y * C8_SCREEN_WIDTH) * 4 + 0] = color;
			data[(x + y * C8_SCREEN_WIDTH) * 4 + 1] = color;
			data[(x + y * C8_SCREEN_WIDTH) * 4 + 2] = color;
			data[(x + y * C8_SCREEN_WIDTH) * 4 + 3] = 0xFF;
		}
	}
	return !tje_encode_to_file(file, C8_SCREEN_WIDTH, C8_SCREEN_HEIGHT, 4, data);
}

void sigIntHandler(int signal) {
	assert(signal == SIGINT);
	if (state.running == RUNNING) {
		state.running = PAUSED;
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

char **commandCompletion(const char *text, int start, int end) {
	(void) end;
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

	c8_state_t *emu = state.state;

	switch (commandId) {
	case NO_COMMAND: {
		printf("Unknown command `%s'.\n", base);
		printf("For help, use `help'.\n");
		break;
	}
	case BACKTRACE: {
		UWord frames[16];
		size_t frameCount = c8_callStack(emu, frames, 16);
		for (size_t i = 0; i < frameCount; i++) {
			printf("0x%04x\n", frames[i]);
		}
		break;
	}
	case BRK: {
		UWord loc;
		if (parseWord(args[1], &loc) != 0) {
			break;
		}
		if (state.brkidx == 16) {
			printf("Cannot add more breakpoints; "
				"please remove one\n");
			break;
		}
		state.brk[state.brkidx++] = loc;
		break;
	}
	case CYCLES: {
		printf("%d cycles since last call.\n", state.cycles);
		state.cycles = 0;
		break;
	}
	case DISASSEMBLE: {
		UWord pos = c8_readPC(emu);
		if (argc > 1) {
			if (parseWord(args[1], &pos) != 0) {
				break;
			}
		}
		printf("%s\n", c8_disassemble(emu, pos));
		break;
	}
	case DUMP_DISPLAY: {
		int fail = dumpDisplay(emu, args[1]);
		if (fail)
			printf("Failed to write to file.\n");
		break;
	}
	case DUMP_MEMORY: {
		int fail = dumpMemory(emu, args[1]);
		if (fail)
			printf("Failed to write to file.\n");
		break;
	}
	case FINISH: {
		int nestedCalls = 1;
		do {
			UWord pos = c8_readPC(emu);
			UWord inst = c8_readMemoryWord(emu, pos);
			if (inst == 0x00EE) // return opcode
				nestedCalls--;
			else if ((inst & 0xF000) == 0x2000) // call opcode
				nestedCalls++;
		} while (nestedCalls > 0);
		break;
	}
	case GOTO: {
		UWord loc;
		if (parseWord(args[1], &loc) != 0) {
			break;
		}
		c8_writePC(emu, loc);
		break;
	}
	case HELP: {
		if (argc > 1) {
			enum command helpCmd = matchCommand(args[1]);
			if (helpCmd == NO_COMMAND) {
				printf("Unknown command `%s'.\n", args[1]);
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
		if (state.brkidx == 0) {
			printf("No breakpoints\n");
		} else {
			for (int i = 0; i < state.brkidx; i++) {
				printf("%d: 0x%04x\n", i, state.brk[i]);
			}
		}
		break;
	}
	case LOAD_MEMORY: {
		size_t size;
		const unsigned char *buf = readfile(args[1], &size);
		if (size < 0x1000) {
			printf("File too small to write as memory.\n");
			break;
		}
		c8_loadMemory(emu, buf, 0x1000);
		break;
	}
	case LOAD_ROM: {
		if (readROM(emu, args[1]))
			printf("Error loading ROM.\n");
		else
			state.hasRom = true;
		break;
	}
	case NEXT: {
		UWord pc = c8_readPC(emu);
		const char *disassembly = c8_disassemble(emu, pc);
		if (disassembly == NULL) {
			printf("Unknown instruction ahead\n");
		} else {
			printf("%s\n", disassembly);
		}
		break;
	}
	case PAUSE: {
		state.running = PAUSED;
		break;
	}
	case READ: {
		UWord loc;
		if (parseWord(args[1], &loc) != 0) {
			break;
		}
		printf("0x%02x\n", c8_readMemoryByte(emu, loc));
		break;
	}
	case REBOOT: {
		c8_cpuBoot(emu);
		state.hasRom = false;
		break;
	}
	case RECVKEY: {
		UByte key;
		if (parseByte(args[1], &key) != 0) {
			break;
		}
		if (key > 16) {
			break;
		}
		c8_pressKey(emu, (int) key);
		c8_releaseKey(emu, (int) key);
		break;
	}
	case REGS: {
		for (c8_register_t i = 0; i < C8_REG_8; i++)
			printf("v%x: %02x\tv%x: %02x\n", i, c8_readRegister(emu, i),
				i + 8, c8_readRegister(emu, i + 8));
		printf("Program Counter: %04x\n", c8_readPC(emu));
		printf("Address: %04x\n", c8_readAddressRegister(emu));
		break;
	}
	case REMBRK: {
		UByte idx;
		if (parseByte(args[1], &idx) != 0) {
			break;
		}
		if (idx >= state.brkidx) {
			printf("Breakpoint %d not set\n", idx);
			break;
		}
		for (int i = idx; i < state.brkidx; i++) {
			state.brk[i] = state.brk[i + 1];
		}
		state.brkidx--;
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
	case SETADDR: {
		UWord loc;
		if (parseWord(args[1], &loc) != 0) {
			break;
		}
		c8_writeAddressRegister(emu, loc);
		break;
	}
	case SETREG: {
		c8_register_t reg;
		if (c8_registerByName(args[1], &reg) != 0) {
			printf("Unknown register name\n");
			break;
		}
		UByte byte;
		if (parseByte(args[2], &byte) != 0) {
			break;
		}
		c8_writeRegister(emu, reg, byte);
		break;
	}
	case STEP: {
		if (!state.hasRom) {
			printf("No ROM loaded.\n");
			break;
		}
		int cycles;
		c8_status_t status = c8_cpuStep(emu, &cycles);
		if (status == C8_EXITED) {
			printf("ROM exited via `exit` instruction");
		} else if (status == C8_UNKNOWN_OP) {
			UWord pc = c8_readPC(emu);
			printf("Unknown instruction: %04X\n", c8_readMemoryWord(emu, pc));
			const char *disassembly = c8_disassemble(emu, pc);
			if (disassembly != NULL)
				printf("%s\n", disassembly);
		} else {
			state.cycles += cycles;
		}
		break;
	}
	case TIMERS: {
		printf("Delay timer: %02x\n", c8_delayTimer(emu));
		printf("Sound timer: %02x\n", c8_soundTimer(emu));
		break;
	}
	case TOGGLE_KEY: {
		UByte key;
		if (parseByte(args[1], &key) != 0) {
			break;
		}
		if (key > 16) {
			break;
		}
		if (c8_isKeyDown(emu, (int) key)) {
			c8_releaseKey(emu, (int) key);
		} else {
			c8_pressKey(emu, (int) key);
		}
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
		c8_writeMemoryByte(emu, loc, byte);
		break;
	}
	}

command_cleanup:
	free(base);
	free(args);
}

int debugRepl(void *data) {
	(void) data;
	while (true) {
		bool wasRunning = state.running == RUNNING;
		char *buffer = readline("> ");
		if (!buffer)
			continue;
		if (strlen(buffer) == 0) {
			free(buffer);
			continue;
		}
		add_history(buffer);
		if (SDL_LockMutex(state.mutex) == -1) {
			fprintf(stderr, "Error running command");
			free(buffer);
			continue;
		}
		executeCommand(buffer);
		SDL_UnlockMutex(state.mutex);
		free(buffer);
		if (state.running && !wasRunning) {
			deltatime(&state.lasttime);
		}
	}
}
#endif

int main(int argc, char *argv[]) {
	// drop the program name
	argc--; argv++;

	int exitCode = 0;
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	c8_state_t *emu = state.state = c8_newState();
	if (emu == NULL) {
		fprintf(stderr, "Could not allocate memory.\n");
		return 1;
	}

	state.brkidx = 0;

#ifdef DEBUG_REPL
	state.mutex = SDL_CreateMutex();

	if (argc > 0) {
		int fail = readROM(emu, argv[0]);
		if (fail)
			fprintf(stderr, "Error loading ROM.\n");
		else
			state.hasRom = true;
	}

	if (signal(SIGINT, sigIntHandler) == SIG_ERR) {
		fprintf(stderr, "Error setting signal handler.\n");
		exitCode = 1;
		goto cleanup;
	}

	rl_attempted_completion_function = commandCompletion;
#else
	if (argc < 1) {
		fprintf(stderr, "Must provide a ROM argument.\n");
		exitCode = 1;
		goto cleanup;
	}
	int fail = readROM(emu, argv[0]);
	if (fail) {
		fprintf(stderr, "Error loading ROM.\n");
		exitCode = 1;
		goto cleanup;
	} else {
		state.hasRom = true;
		state.running = RUNNING;
	}
#endif

	SDL_SetMainReady();
	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
		return 1;
	}

	window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, PIXEL_SCALE * C8_SCREEN_WIDTH,
		PIXEL_SCALE * C8_SCREEN_HEIGHT, 0);
	if (window == NULL) {
		fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
		return 1;
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED
		| SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) {
		fprintf(stderr, "Renderer failed: %s\n", SDL_GetError());
		exitCode = 1;
		goto cleanup;
	}

	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

#ifdef DEBUG_REPL
	SDL_Thread *debugThread = SDL_CreateThread(debugRepl, "debug", &state);
	SDL_DetachThread(debugThread);
#endif

	// get the time to compare to with deltatime
	deltatime(&state.lasttime);

	while (true) {
#ifdef DEBUG_REPL
		if (SDL_TryLockMutex(state.mutex) == 0) {
#endif
			bool quit = false;
			SDL_Event e;
			while (SDL_PollEvent(&e)) {
				switch (e.type) {
				case SDL_QUIT:
					quit = true;
					break;
				case SDL_KEYDOWN:
					switch (e.key.keysym.sym) {
					case SDLK_ESCAPE:
#ifdef DEBUG_REPL
						state.running = PAUSED;
#else
						quit = true;
#endif
						break;
					case SDLK_1: c8_pressKey(emu, 0x1); break;
					case SDLK_2: c8_pressKey(emu, 0x2); break;
					case SDLK_3: c8_pressKey(emu, 0x3); break;
					case SDLK_4: c8_pressKey(emu, 0xC); break;
					case SDLK_q: c8_pressKey(emu, 0x4); break;
					case SDLK_w: c8_pressKey(emu, 0x5); break;
					case SDLK_e: c8_pressKey(emu, 0x6); break;
					case SDLK_r: c8_pressKey(emu, 0xD); break;
					case SDLK_a: c8_pressKey(emu, 0x7); break;
					case SDLK_s: c8_pressKey(emu, 0x8); break;
					case SDLK_d: c8_pressKey(emu, 0x9); break;
					case SDLK_f: c8_pressKey(emu, 0xE); break;
					case SDLK_z: c8_pressKey(emu, 0xA); break;
					case SDLK_x: c8_pressKey(emu, 0x0); break;
					case SDLK_c: c8_pressKey(emu, 0xB); break;
					case SDLK_v: c8_pressKey(emu, 0xF); break;
					default: break; // don't care
					}
					break;
				case SDL_KEYUP:
					switch (e.key.keysym.sym) {
					case SDLK_1: c8_releaseKey(emu, 0x1); break;
					case SDLK_2: c8_releaseKey(emu, 0x2); break;
					case SDLK_3: c8_releaseKey(emu, 0x3); break;
					case SDLK_4: c8_releaseKey(emu, 0xC); break;
					case SDLK_q: c8_releaseKey(emu, 0x4); break;
					case SDLK_w: c8_releaseKey(emu, 0x5); break;
					case SDLK_e: c8_releaseKey(emu, 0x6); break;
					case SDLK_r: c8_releaseKey(emu, 0xD); break;
					case SDLK_a: c8_releaseKey(emu, 0x7); break;
					case SDLK_s: c8_releaseKey(emu, 0x8); break;
					case SDLK_d: c8_releaseKey(emu, 0x9); break;
					case SDLK_f: c8_releaseKey(emu, 0xE); break;
					case SDLK_z: c8_releaseKey(emu, 0xA); break;
					case SDLK_x: c8_releaseKey(emu, 0x0); break;
					case SDLK_c: c8_releaseKey(emu, 0xB); break;
					case SDLK_v: c8_releaseKey(emu, 0xF); break;
					default: break; // don't care
					}
					break;
				default:
					// don't care
					break;
				}
			}
			if (quit) {
#ifdef DEBUG_REPL
				SDL_UnlockMutex(state.mutex);
#endif
				break;
			} else if (state.running == RUNNING) {
				double dt = deltatime(&state.lasttime);
				int cycles;
				c8_status_t status = c8_emulateUntil(emu, dt, &cycles, state.brk, state.brkidx);
				state.cycles += cycles;
				if (status == C8_BREAK) {
					state.running = PAUSED;
					printf("Breakpoint reached; pausing\n");
#ifdef DEBUG_REPL
					SDL_UnlockMutex(state.mutex);
#endif
					continue;
				} else if (status == C8_UNKNOWN_OP) {
					UWord pc = c8_readPC(emu);
					fprintf(stderr, "Unknown instruction: %04X\n", c8_readMemoryWord(emu, pc));
					const char *disassembly = c8_disassemble(emu, pc);
					if (disassembly != NULL)
						printf("%s\n", disassembly);
#ifdef DEBUG_REPL
					state.running = PAUSED;
					SDL_UnlockMutex(state.mutex);
					continue;
#else
					exitCode = 1;
					break;
#endif
				} else if (status == C8_EXITED) {
					printf("ROM exited via `exit` instruction");
				}
				if (c8_shouldBeep(emu)) {
					// TODO actually beep
					printf("Beep\n");
				}
			}

			for (UByte x = 0; x < C8_SCREEN_WIDTH; x++) {
				for (UByte y = 0; y < C8_SCREEN_HEIGHT; y++) {
					UByte pixelSet = c8_readFromScreen(emu, x, y);
					Uint8 color = pixelSet ? 0xFF : 0x00;
					SDL_SetRenderDrawColor(renderer, color, color,
						color, SDL_ALPHA_OPAQUE);
					SDL_Rect rect = {
						.x = (int) x * PIXEL_SCALE,
						.y = (int) y * PIXEL_SCALE,
						.w = PIXEL_SCALE, .h = PIXEL_SCALE,
					};
					SDL_RenderFillRect(renderer, &rect);
				}
			}
			SDL_RenderPresent(renderer);

#ifdef DEBUG_REPL
			SDL_UnlockMutex(state.mutex);
		}
#endif
		SDL_Delay(1);
	}

cleanup:
	free(emu);
#ifdef DEBUG_REPL
	SDL_UnlockMutex(state.mutex);
	SDL_DestroyMutex(state.mutex);
#endif
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return exitCode;
}
