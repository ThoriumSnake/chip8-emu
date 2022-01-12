build:
	gcc -std=c11 -Wall -pedantic ./src/*.c -lSDL2 -o chip8

run:
	./chip8 ./roms/c8_test.c8 30 3
