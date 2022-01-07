build:
	gcc -std=c11 -Wall ./src/chip8.c -lSDL2 -o chip8

run:
	./chip8
