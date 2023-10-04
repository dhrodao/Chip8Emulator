GCC=gcc
CFLAGS=-Wall -Wshadow
CINCLUDE=-I.
CLIBS=-lSDL3 -lpthread

all:
	$(GCC) $(CFLAGS) $(CINCLUDE) $(CLIBS) chip8.c main.c -o main

clean:
	rm main

.PHONY: all clean