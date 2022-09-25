#include <stdio.h>
#include <stdlib.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include <chip8.h>

// eight pixels for every CHIP-8 pixel
const int PIXEL_SCALE = 8;

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

	struct emuState *emu = malloc(sizeof(struct emuState));
	if (emu == NULL) {
		fprintf(stderr, "Could not allocate emulator state.\n");
		return 1;
	}

	cpuBoot(emu);
	int fail = loadROM(emu, rom, size);
	free(rom);
	if (fail) {
		fprintf(stderr, "Error loading ROM.\n");
		return 1;
	}

	SDL_SetMainReady();
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		fprintf(stderr, "Failed to initialize SDL: %s\n",
			SDL_GetError());
		return 1;
	}

	SDL_Window *window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, PIXEL_SCALE * SCREEN_WIDTH,
		PIXEL_SCALE * SCREEN_HEIGHT, 0);
	if (window == NULL) {
		fprintf(stderr, "Failed to create a window: %s\n",
			SDL_GetError());
		return 1;
	}
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) {
		fprintf(stderr, "Failed to create renderer: %s\n",
			SDL_GetError());
		return 1;
	}

	// get the time to compare to with deltatime
	double lasttime = 0;
	deltatime(&lasttime);

	int error = 0;

	while (1) {
		int quit = 0;
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				quit = 1;
				break;
			case SDL_KEYDOWN:
				switch (e.key.keysym.sym) {
				case SDLK_ESCAPE: quit = 1; break;
				case SDLK_1: pressKey(emu, 0x1); break;
				case SDLK_2: pressKey(emu, 0x2); break;
				case SDLK_3: pressKey(emu, 0x3); break;
				case SDLK_4: pressKey(emu, 0xC); break;
				case SDLK_q: pressKey(emu, 0x4); break;
				case SDLK_w: pressKey(emu, 0x5); break;
				case SDLK_e: pressKey(emu, 0x6); break;
				case SDLK_r: pressKey(emu, 0xD); break;
				case SDLK_a: pressKey(emu, 0x7); break;
				case SDLK_s: pressKey(emu, 0x8); break;
				case SDLK_d: pressKey(emu, 0x9); break;
				case SDLK_f: pressKey(emu, 0xE); break;
				case SDLK_z: pressKey(emu, 0xA); break;
				case SDLK_x: pressKey(emu, 0x0); break;
				case SDLK_c: pressKey(emu, 0xB); break;
				case SDLK_v: pressKey(emu, 0xF); break;
				}
				break;
			case SDL_KEYUP:
				switch (e.key.keysym.sym) {
				case SDLK_1: releaseKey(emu, 0x1); break;
				case SDLK_2: releaseKey(emu, 0x2); break;
				case SDLK_3: releaseKey(emu, 0x3); break;
				case SDLK_4: releaseKey(emu, 0xC); break;
				case SDLK_q: releaseKey(emu, 0x4); break;
				case SDLK_w: releaseKey(emu, 0x5); break;
				case SDLK_e: releaseKey(emu, 0x6); break;
				case SDLK_r: releaseKey(emu, 0xD); break;
				case SDLK_a: releaseKey(emu, 0x7); break;
				case SDLK_s: releaseKey(emu, 0x8); break;
				case SDLK_d: releaseKey(emu, 0x9); break;
				case SDLK_f: releaseKey(emu, 0xE); break;
				case SDLK_z: releaseKey(emu, 0xA); break;
				case SDLK_x: releaseKey(emu, 0x0); break;
				case SDLK_c: releaseKey(emu, 0xB); break;
				case SDLK_v: releaseKey(emu, 0xF); break;
				}
				break;
			default:
				// don't care
				break;
			}
		}
		if (quit) {
			break;
		}

		double dt = deltatime(&lasttime);
		cpu_status_t status = emulate(emu, dt);
		if (status == CPU_ERROR) {
			fprintf(stderr, "Error emulating\n");
			error = 1;
			break;
		}

		for (UByte x = 0; x < SCREEN_WIDTH; x++) {
			for (UByte y = 0; y < SCREEN_HEIGHT; y++) {
				UByte pixelSet = readFromScreen(emu, x, y);
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

		if (shouldBeep(emu)) {
			// TODO actually beep
			printf("Beep\n");
		}
	}

	free(emu);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return error;
}
