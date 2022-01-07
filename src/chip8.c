#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//First available address for rom data
static const int START_ADDRESS = 0x200;

static uint8_t registers[16];
static uint8_t memory[4096];
static uint16_t index_reg;
static uint16_t pc = START_ADDRESS;
static uint16_t stack[16];
static uint8_t stack_ptr = 0;
static uint8_t delay_tmr;
static uint16_t sound_tmr;
static uint8_t keypad[16];
static unsigned int video[64 * 32];
static uint16_t opcode;

void open_rom(FILE *rom) {
    if (rom) {
        //For getting the size of the rom file
        fseek(rom, 0, SEEK_END);
        int size = ftell(rom);
        fseek(rom, 0, SEEK_SET);

        //&memory is for writing starting at a certain position
        //(0x200) Represents the first memory address not occupied by the chip8 interpreter
        fread(&memory[START_ADDRESS], sizeof(uint8_t), size, rom);

        fclose(rom);
    }
}

void load_fontset() {
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
        memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }
}

//Generates a number between 0 and cap
long capped_rng(int cap) {
    //Typically returns number between 0 and cap - 1
    return random() % (cap + 1);
}

void executeOpcode() {
    //Removes all lower bits
    switch (opcode & 0xF000) {
        case 0x0000:
            //Checks if lower nibble is different
            switch (opcode & 0x000F) {
                case 0x0000:
                    memset(video, 0, sizeof(video));

                case 0x000E:
                    break;
            }
            break;

        default:
            printf("Unknown 0x0000 opcode: 0x%X", opcode);
    }
}

void emulateCycle() {
    //Gets the first half of a opcode, shifts to the left to make space and sets the second half using OR bitmask
    opcode = (memory[pc] << 8) | memory[pc + 1];
    pc += 2;
}

void test_open_rom() {
    //This is the same as 0x200 - 1
    printf("%s %x\n", "Address 0x1FF (Should be empty):", memory[0x1FF]);

    printf("%s %x\n", "First rom address:", memory[START_ADDRESS]);

    //Just my ass realizing an array's address is the same as its first element's address lol
    //printf("%s %u %u\n", "Address comp:", &chip8.memory, &chip8.memory[0]);

    int n = 4096;
    for (int i = 0; i < n; i++)
        printf("%02X", memory[i]);
    printf("\n");
    printf("Opcode??: %x\n", (memory[START_ADDRESS] << 8 | memory[START_ADDRESS + 1]));
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
void setupChip8() {
    srandom(time(NULL));
    FILE *rom = fopen("/home/thoriumsnake/Code/Projects/Chip8/roms/c8_test.c8", "rb");
    open_rom(rom);
    load_fontset();
    pc = START_ADDRESS;
}

void run_tests(bool run) {
    if (run) {
        test_open_rom();
        if (test_rng(256, 10000, false))
            printf("RNG Test success\n");
        else
            printf("RNG Test failed\n");
    }
}

int main() {
    setupChip8();
    run_tests(true);
}