#include <chip8.h>

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

char *readfile(const char *name, long *size) {
	FILE *f = fopen(name, "rb");
	if (f == NULL) {
		*size = -1;
		return NULL;
	}
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	char *str = malloc(fsize + 1);
	if (str == NULL) {
		*size = -1;
		return NULL;
	}

	fread(str, 1, fsize, f);
	fclose(f);

	str[fsize] = '\0';

	*size = fsize;
	return str;
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Error: please provide a ROM file as an argument.\n");
		return 1;
	}

	long size;
	char *rom = readfile(argv[1], &size);
	if (size == -1) {
		fprintf(stderr, "Error reading ROM file.\n");
		return 1;
	}

	struct emuState state = { 0 };

	cpuBoot(&state);
	int fail = loadROM(&state, rom, size);
	free(rom);
	if (fail) {
		fprintf(stderr, "Error loading ROM.\n");
		return 1;
	}

	// get the time to compare to with deltatime
	long double lasttime = 0;
	deltatime(&lasttime);

	while (1) {
		long double dt = deltatime(&lasttime);
		int fail = emulate(&state, dt);
		if (fail) {
			fprintf(stderr, "error emulating.\n");
			return 1;
		}
	}
	return 0;
}
