#include "instructions.h"

#include <stdio.h>
#include <string.h>

#include "chip8.h"

//These functions were created following Cowgod's Chip-8 Technical Reference and mattmilokay's Mastering Chip8, however there's some differences on certain instructions, so certain programs may work differently, I'm leaning towards Mastering Chip8 for the sake of correctness, but Cowgod's may be better suited to a few modern Chip8 programs

//Shifting value to the right to place register argument at the right bit, otherwise value would be much bigger
int get_vx(void) {
    int vx = (opcode & 0x0F00) >> 8;
    return vx;
}

int get_vy(void) {
    int vy = (opcode & 0x00F0) >> 4;
    return vy;
}

void op_00E0(void) {  //CLS
    memset(video, 0, sizeof(video));
}

void op_00EE(void) {  //RET
    stack_ptr--;
    pc = stack[stack_ptr];
}

void op_1nnn(void) {  //JMP addr
    //Removes opcode from address
    uint16_t address = opcode & 0x0FFF;
    pc = address;
}

void op_2nnn(void) {  //CALL addr
    uint16_t address = opcode & 0x0FFF;
    stack[stack_ptr] = pc;
    stack_ptr++;
    pc = address;
}
void op_3xnn(void) {  //SE Vx, byte
    uint8_t value = opcode & 0x00FF;
    if (registers[get_vx()] == value)
        pc += 2;
}

void op_4xnn(void) {  //SNE Vx, byte
    uint8_t value = opcode & 0x00FF;
    if (registers[get_vx()] != value)
        pc += 2;
}

void op_5xy0(void) {  //SE Vx, Vy
    if (registers[get_vx()] == registers[get_vy()])
        pc += 2;
}
void op_6xnn(void) {  //LD Vx, byte
    uint8_t value = opcode & 0x00FF;
    registers[get_vx()] = value;
}

void op_7xnn(void) {  //Add Vx, byte
    uint8_t value = opcode & 0x00FF;
    registers[get_vx()] += value;
}

void op_8xy0(void) {
    registers[get_vx()] = registers[get_vy()];
}
void op_8xy1(void) {
    int vx = get_vx();
    int vy = get_vy();
    registers[vx] = registers[vx] | registers[vy];
}

void op_8xy2(void) {
    int vx = get_vx();
    int vy = get_vy();
    registers[vx] = registers[vx] & registers[vy];
}

void op_8xy3(void) {
    int vx = get_vx();
    int vy = get_vy();
    registers[vx] = registers[vx] ^ registers[vy];
}

void op_8xy4(void) {  //ADD Vx, Vy
    int vx = get_vx();
    int vy = get_vy();
    int add = registers[vx] + registers[vy];
    registers[vx] = add % 256;
    if (add > 255)
        registers[0xF] = 0x01;
    else
        registers[0xF] = 0x00;
}

void op_8xy5(void) {  //SUB Vx, Vy
    int vx = get_vx();
    int vy = get_vy();
    int borrow = registers[vx] > registers[vy];
    registers[0xF] = borrow;
    registers[vx] = registers[vx] - registers[vy];
}

void op_8xy6(void) {  //SHR Vx {, Vy}
    int vx = get_vx();
    int vy = get_vy();
    //Doing this to clear all other bits except the LSB
    registers[0xF] = registers[vy] & 0x1;
    registers[vx] = registers[vy] >> 1;
}

void op_8xy7(void) {
    int vx = get_vx();
    int vy = get_vy();
    int borrow = registers[vy] > registers[vx];
    registers[0xF] = borrow;
    registers[vx] = registers[vy] - registers[vx];
}

void op_8xyE(void) {  // SHL Vx {, Vy}
    int vx = get_vx();
    int vy = get_vy();
    //Doing this to clear all other bits except the MSB, 0x80 represents 1000 0000
    //Shifting to place bit in right place
    registers[0xF] = registers[vy] & 0x80 >> 7;
    registers[vx] = registers[vy] << 1;
}

void op_9xy0(void) {
    if (registers[get_vx()] != registers[get_vy()])
        pc += 2;
}

void op_Annn(void) {
    uint16_t address = opcode & 0x0FFF;
    index_reg = address;
}

void op_Bnnn(void) {
    uint16_t address = (opcode & 0x0FFF) + registers[0];
    pc = address;
}

void op_Cxnn(void) {
    uint8_t value = opcode & 0x00FF;
    uint8_t random = capped_rng(255);
    registers[get_vx()] = value & random;
}

void op_Dxyn(void) {
    int xpos = registers[get_vx()];
    int ypos = registers[get_vy()];
    uint8_t sprite_height = opcode & 0x000F;

    const int BYTE_WIDTH = 8;

    for (size_t row = 0; row < sprite_height; row++) {
        uint8_t sprite_byte = memory[index_reg + row];
        for (size_t col = 0; col < BYTE_WIDTH; col++) {
            //Pointer to value to avoid repetition
            //This is for selecting array elements as if it was a 2d array
            unsigned int *video_pixel = &video[VIDEO_WIDTH * ((ypos + row) % VIDEO_HEIGHT) + ((xpos + col) % VIDEO_WIDTH)];
            //ANDing to isolate pixel bit
            unsigned int sprite_pixel = sprite_byte & (0x80 >> col);
            //Setting it to max int for XORing with the video pixel
            if (sprite_pixel) {
                sprite_pixel = 0xFFFFFFFF;
                //Setting VF register if there's a collision
                registers[0xF] = 0;
                if (*video_pixel)
                    registers[0xF] = 1;
            }

            *video_pixel = *video_pixel ^ sprite_pixel;
        }
    }
}

void op_Ex9E(void) {  //SKP Vx
    int vx = get_vx();
    uint8_t key = registers[vx];
    if (keypad[key])
        pc += 2;
}

void op_ExA1(void) {  //SKNP Vx
    int vx = get_vx();
    uint8_t key = registers[vx];
    if (!keypad[key])
        pc += 2;
}

void op_Fx07(void) {
    registers[get_vx()] = delay_tmr;
}

void op_Fx0A(void) {  //LD Vx, K
    const int KEYPAD_COUNT = 16;
    //Checks if any of the keys are pressed
    for (int key_val = 0; key_val < KEYPAD_COUNT; key_val++) {
        if (keypad[key_val]) {
            registers[get_vx()] = key_val;
            return;
        }
    }
    //This is done to "wait" if a key wasn't pressed
    pc -= 2;
}

void op_Fx15(void) {
    delay_tmr = registers[get_vx()];
}

void op_Fx18(void) {
    sound_tmr = registers[get_vx()];
}

void op_Fx1E(void) {
    index_reg += registers[get_vx()];
}

void op_Fx29(void) {  //LD F, Vx
    int vx_char = registers[get_vx()];
    //The separation between each character in the fontset array
    int const FONT_SIZE = 5;
    //Getting the position of the character sprite in the memory array
    index_reg = (vx_char * FONT_SIZE) + FONTSET_START_ADDRESS;
}

void op_Fx33(void) {
    //Stores the BCD equivalent starting at index_reg + 2, explanation on BCD: https://www.geeksforgeeks.org/bcd-or-binary-coded-decimal/
    uint8_t num = registers[get_vx()];
    for (size_t i = 2; num > 0; i--) {
        //Modulo to "separate" the digits
        memory[index_reg + i] = num % 10;
        num = num / 10;
    }
}

void op_Fx55(void) {
    //Adding one to include register VX
    for (size_t i = 0; i < get_vx() + 1; i++) {
        memory[index_reg + i] = registers[i];
    }
    index_reg = index_reg + get_vx() + 1;
}

void op_Fx65(void) {
    for (size_t i = 0; i < get_vx() + 1; i++) {
        registers[i] = memory[index_reg + i];
    }
    index_reg = index_reg + get_vx() + 1;
}