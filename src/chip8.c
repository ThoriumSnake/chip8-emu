#include "chip8.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "instructions.h"

//First available address for rom data
static const int START_ADDRESS = 0x200;
const int FONTSET_START_ADDRESS = 0x050;
//For counting cycles in debugging
static int cycle = 0;

uint8_t registers[16] = {0};
uint8_t memory[4096];
uint16_t index_reg = 0;
uint16_t pc = START_ADDRESS;
uint16_t stack[16] = {0};
uint8_t stack_ptr = 0;
uint8_t delay_tmr = 0;
uint16_t sound_tmr = 0;
bool keypad[16] = {false};
unsigned int video[VIDEO_WIDTH * VIDEO_HEIGHT] = {0};
uint16_t opcode = 0;

int open_rom(FILE *rom);
void load_fontset();

//Ignoring warning, as random() function is properly declared but warning is given anyway
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
//Sets up Chip8 object
//Pass a chip8 address/reference to this function
int setupChip8(FILE *rom) {
    srandom(time(NULL));
    if (!open_rom(rom))
        return false;

    load_fontset();
    return true;
}
#pragma GCC diagnostic push

int open_rom(FILE *rom) {
    if (rom) {
        //For getting the size of the rom file
        fseek(rom, 0, SEEK_END);
        int size = ftell(rom);
        fseek(rom, 0, SEEK_SET);

        //&memory is for writing starting at a certain position
        //(0x200) Represents the first memory address not occupied by the chip8 interpreter
        fread(&memory[START_ADDRESS], sizeof(uint8_t), size, rom);

        fclose(rom);
        return true;
    }
    return false;
}

void load_fontset(void) {
    const int FONTSET_SIZE = 80;
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

//Ignoring warning, as random() function is properly declared but warning is given anyway
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
//Generates a number between 0 and cap
long capped_rng(int cap) {
    //Typically returns number between 0 and cap - 1
    return random() % (cap + 1);
}
#pragma GCC diagnostic pop

void executeOpcode(void) {
    //Removes all lower bits
    switch (opcode & 0xF000) {
        //Checks for opcode categories
        case 0x0000:
            //Checks if lower nibble is different
            switch (opcode & 0x000F) {
                case 0x0000:  //CLS
                    op_00E0();
                    break;

                case 0x000E:  //RET
                    op_00EE();
                    break;
            }
            break;

        case 0x1000:  //JMP addr
            op_1nnn();
            break;
        case 0x2000:  //CALL addr
            op_2nnn();
            break;
        case 0x3000:  //SE Vx, byte
            op_3xnn();
            break;
        case 0x4000:  //SNE Vx, byte
            op_4xnn();
            break;
        case 0x5000:
            op_5xy0();
            break;
        case 0x6000:
            op_6xnn();
            break;
        case 0x7000:
            op_7xnn();
            break;

        case 0x8000:
            switch (opcode & 0x000F) {
                case 0x0000:
                    op_8xy0();
                    break;
                case 0x0001:
                    op_8xy1();
                    break;
                case 0x0002:
                    op_8xy2();
                    break;
                case 0x0003:
                    op_8xy3();
                    break;
                case 0x0004:
                    op_8xy4();
                    break;
                case 0x0005:
                    op_8xy5();
                    break;
                case 0x0006:
                    op_8xy6();
                    break;
                case 0x0007:
                    op_8xy7();
                    break;
                case 0x000E:
                    op_8xyE();
                    break;
            }
            break;

        case 0x9000:
            op_9xy0();
            break;
        case 0xA000:
            op_Annn();
            break;
        case 0xB000:
            op_Bnnn();
            break;
        case 0xC000:
            op_Cxnn();
            break;
        case 0xD000:
            op_Dxyn();
            break;

        case 0xE000:
            switch (opcode & 0x000F) {
                case 0x000E:
                    op_Ex9E();
                    break;

                case 0x0001:
                    op_ExA1();
                    break;
            }
            break;

        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x0007:
                    op_Fx07();
                    break;
                case 0x000A:
                    op_Fx0A();
                    break;
                case 0x0015:
                    op_Fx15();
                    break;
                case 0x0018:
                    op_Fx18();
                    break;
                case 0x001E:
                    op_Fx1E();
                    break;
                case 0x0029:
                    op_Fx29();
                    break;
                case 0x0033:
                    op_Fx33();
                    break;
                case 0x0055:
                    op_Fx55();
                    break;
                case 0x0065:
                    op_Fx65();
                    break;
            }
            break;

        default:
            printf("Unknown opcode: 0x%X", opcode);
    }
}

void emulate_cycle(void) {
    //Gets the first half of a opcode, shifts to the left to make space and sets the second half using OR bitmask
    opcode = (memory[pc] << 8) | memory[pc + 1];
    pc += 2;
    executeOpcode();

    if (delay_tmr > 0)
        delay_tmr--;
    if (sound_tmr > 0)
        sound_tmr--;
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

void run_tests(bool run) {
    if (run) {
        test_open_rom();
        if (test_rng(256, 10000, false))
            printf("RNG Test success\n");
        else
            printf("RNG Test failed\n");
    }
}

void print_debug(void) {
    cycle++;
    printf("Cycle: %d\n", cycle);
    int n = 0;
    printf("PC: %04X\n", pc);
    printf("I: %04X\n", index_reg);
    printf("Opcode: %04X\n", opcode);

    printf("Delay Timer: %02X\n", delay_tmr);
    printf("Sound Timer: %02X\n", sound_tmr);

    printf("Stack Pointer: %02X\n", stack_ptr);

    printf("Stack: \n");
    n = 16;
    for (int i = 0; i < n; i++)
        if (stack[i])
            printf("%X: %02X\n", i, stack[i]);

    printf("Registers: \n");
    n = 16;
    for (int i = 0; i < n; i++)
        printf("V%X: %02X\n", i, registers[i]);

    for (int i = 0; i < 16; i++) {
        printf("%x: %d\n", i, keypad[i]);
    }

    /*printf("Memory:\n");
    n = 4096;
    for (int i = 0; i < n; i++)
        printf("%02X", memory[i]);
    printf("\n");*/
}