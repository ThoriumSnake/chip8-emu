#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define VIDEO_WIDTH 64
#define VIDEO_HEIGHT 32

extern uint8_t registers[16];
extern uint8_t memory[4096];
extern uint16_t index_reg;
extern uint16_t pc;
extern uint16_t stack[16];
extern uint8_t stack_ptr;
extern uint8_t delay_tmr;
extern uint16_t sound_tmr;
extern bool keypad[16];
extern unsigned int video[VIDEO_WIDTH * VIDEO_HEIGHT];
extern uint16_t opcode;

extern const int FONTSET_START_ADDRESS;

long capped_rng(int cap);
int setupChip8(FILE *rom);
void run_tests(bool run);
void emulate_cycle(void);
void print_debug(void);