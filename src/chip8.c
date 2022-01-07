#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//First available address for rom data
static const int START_ADDRESS = 0x200;
typedef struct {
    uint8_t registers[16];
    uint8_t memory[4096];
    uint16_t index_reg;
    uint16_t pc;
    uint16_t stack[16];
    uint8_t stack_ptr;
    uint8_t delay_tmr;
    uint16_t sound_tmr;
    uint8_t keypad[16];
    unsigned int video[64 * 32];
    uint16_t opcode;
} Chip8;

void open_rom(FILE *rom, Chip8 *chip8) {
    if (rom) {
        //For getting the size of the rom file
        fseek(rom, 0, SEEK_END);
        int size = ftell(rom);
        fseek(rom, 0, SEEK_SET);

        //&chip8.memory is for writing starting at a certain position
        //(0x200) Represents the first memory address not occupied by the chip8 interpreter
        fread(&chip8->memory[START_ADDRESS], sizeof(uint8_t), size, rom);

        fclose(rom);
    }
}

void load_fontset(Chip8 *chip8) {
    const int FONTSET_SIZE = 80;
    const int FONTSET_START_ADDRESS = 0x050;
    //Each element represents a byte of binary in which you can see the character being "drawn", see docs for more info
    uint8_t fontset[] =
        {
            0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
            0x20, 0x60, 0x20, 0x20, 0x70,  // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
            0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
            0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
            0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
            0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
            0xF0, 0x80, 0xF0, 0x80, 0x80   // F
        };
    for (int i = 0; i < FONTSET_SIZE; i++) {
        chip8->memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }
}

//Generates a number between 0 and cap
long capped_rng(int cap) {
    //Typically returns number between 0 and cap - 1
    return random() % (cap + 1);
}

void emulateCycle() {
}

void test_open_rom(Chip8 *chip8) {
    //This is the same as 0x200 - 1
    printf("%s %x\n", "Address 0x1FF (Should be empty):", chip8->memory[0x1FF]);

    printf("%s %x\n", "First rom address:", chip8->memory[START_ADDRESS]);

    //Just my ass realizing an array's address is the same as its first element's address lol
    //printf("%s %u %u\n", "Address comp:", &chip8.memory, &chip8.memory[0]);

    int n = 4096;
    for (int i = 0; i < n; i++)
        printf("%02X", chip8->memory[i]);
    printf("\n");
    printf("Opcode??: %x\n", (chip8->memory[START_ADDRESS] << 8 | chip8->memory[START_ADDRESS + 1]));
}

//Will test if generated numbers are between 0 and cap
//Will generate iter numbers
bool test_rng(int cap, int iter, bool print) {
    for (int i = 0; i < iter; i++) {
        int num = capped_rng(255);
        if (print)
            printf("%d\n", num);
        if (num > cap)
            return false;
        continue;
    }
    return true;
}

//Sets up Chip8 object
//Pass a chip8 address/reference to this function
void setupChip8(Chip8 *chip8) {
    srandom(time(NULL));
    FILE *rom = fopen("/home/thoriumsnake/Code/Projects/Chip8/roms/c8_test.c8", "rb");
    open_rom(rom, chip8);
    load_fontset(chip8);
}

void run_tests(Chip8 *chip8, bool run) {
    if (run) {
        test_open_rom(chip8);
        if (test_rng(256, 10000, false))
            printf("RNG Test success\n");
        else
            printf("RNG Test failed\n");
    }
}

int main() {
    Chip8 chip8;
    setupChip8(&chip8);
    run_tests(&chip8, true);
}