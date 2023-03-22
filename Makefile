.PHONY: all clean distclean

CC = gcc
CFLAGS = -I include -I ext/include -I src/include -Wall -Wextra -I /opt/homebrew/include -Werror -flto -O2

SRCS = $(wildcard src/*.c) $(wildcard src/instructions/*.c )
OBJS = $(SRCS:.c=.o)

all: bin/chip8 bin/chip8_debug

%.o: %.c Makefile include/*.h
	$(CC) $(CFLAGS) -c $< -o $@

bin/libchip8.dylib: Makefile $(OBJS) | bin
	$(CC) -fPIC -shared $(OBJS) -o $@

bin/chip8: Makefile frontend.c bin/libchip8.dylib | bin
	$(CC) $(CFLAGS) frontend.c bin/libchip8.dylib -l SDL2 -o $@

bin/chip8_debug: Makefile frontend.c bin/libchip8.dylib | bin
	$(CC) $(CFLAGS) frontend.c bin/libchip8.dylib -l readline -l SDL2 -D DEBUG_REPL -o $@

bin:
	mkdir -p bin

distclean: clean
	rm -f bin/libchip8.dylib bin/chip8 bin/chip8_debug
	rmdir bin

clean:
	rm -f $(OBJS)
