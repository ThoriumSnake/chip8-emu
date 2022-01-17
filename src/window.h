#pragma once
#include <stdbool.h>

extern bool game_running;

int initialize_sdl(int window_width, int window_height, bool resizable);
void update(bool debug);
void render(int pitch);
void destroy_window();
void process_input(bool debug);