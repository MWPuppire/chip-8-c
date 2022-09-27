.PHONY: all clean distclean

CC = gcc
CFLAGS = -I include -I ext/include -Wall -Wextra -I /opt/homebrew/include

SRCS = $(wildcard src/*.c) $(wildcard src/instructions/*.c )
OBJS = $(SRCS:.c=.o)

all: bin/chip8 bin/chip8_debugger

*/.c.o:
	$(CC) $(CFLAGS) -c $<

bin/libchip8.dylib: Makefile $(OBJS) | bin
	$(CC) -fPIC -shared $(OBJS) -o $@

bin/chip8: Makefile frontends/sdl.c bin/libchip8.dylib | bin
	$(CC) $(CFLAGS) frontends/sdl.c bin/libchip8.dylib -l SDL2 -o $@

bin/chip8_debugger: Makefile frontends/debug-repl.c bin/libchip8.dylib | bin
	$(CC) $(CFLAGS) frontends/debug-repl.c bin/libchip8.dylib -l readline -l SDL2 -o $@

bin:
	mkdir -p bin

distclean: clean
	rm -f bin/libchip8.dylib bin/chip8 bin/chip8_debugger
	rmdir bin

clean:
	rm -f $(OBJS)
