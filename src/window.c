#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

#include "chip8.h"

bool game_running;
static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* texture = NULL;

int initialize_sdl(int window_width, int window_height, bool resizable) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    window = SDL_CreateWindow(
        "Chip8",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        0);
    if (!window) {
        fprintf(stderr, "Error creating SDL Window.\n");
        return false;
    }
    SDL_SetWindowResizable(window, resizable);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return false;
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, VIDEO_WIDTH, VIDEO_HEIGHT);

    return true;
}

void update(bool debug) {
    if (!debug) {
        emulate_cycle();
        print_debug();
    }
}

void render(int pitch) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_UpdateTexture(texture, NULL, video, pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    //Swaps the back and front buffer
    SDL_RenderPresent(renderer);
}

void destroy_window(void) {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void process_input(bool debug) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT: {
                game_running = false;
            } break;

            case SDL_KEYDOWN: {
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE: {
                        game_running = false;
                    } break;

                    case SDLK_SPACE: {
                        if (debug) {
                            emulate_cycle();
                            print_debug();
                        }
                    }

                    case SDLK_x: {
                        keypad[0] = 1;
                    } break;

                    case SDLK_1: {
                        keypad[1] = 1;
                    } break;

                    case SDLK_2: {
                        keypad[2] = 1;
                    } break;

                    case SDLK_3: {
                        keypad[3] = 1;
                    } break;

                    case SDLK_q: {
                        keypad[4] = 1;
                    } break;

                    case SDLK_w: {
                        keypad[5] = 1;
                    } break;

                    case SDLK_e: {
                        keypad[6] = 1;
                    } break;

                    case SDLK_a: {
                        keypad[7] = 1;
                    } break;

                    case SDLK_s: {
                        keypad[8] = 1;
                    } break;

                    case SDLK_d: {
                        keypad[9] = 1;
                    } break;

                    case SDLK_z: {
                        keypad[0xA] = 1;
                    } break;

                    case SDLK_c: {
                        keypad[0xB] = 1;
                    } break;

                    case SDLK_4: {
                        keypad[0xC] = 1;
                    } break;

                    case SDLK_r: {
                        keypad[0xD] = 1;
                    } break;

                    case SDLK_f: {
                        keypad[0xE] = 1;
                    } break;

                    case SDLK_v: {
                        keypad[0xF] = 1;
                    } break;
                }
            } break;

            case SDL_KEYUP: {
                switch (event.key.keysym.sym) {
                    case SDLK_x: {
                        keypad[0] = 0;
                    } break;

                    case SDLK_1: {
                        keypad[1] = 0;
                    } break;

                    case SDLK_2: {
                        keypad[2] = 0;
                    } break;

                    case SDLK_3: {
                        keypad[3] = 0;
                    } break;

                    case SDLK_q: {
                        keypad[4] = 0;
                    } break;

                    case SDLK_w: {
                        keypad[5] = 0;
                    } break;

                    case SDLK_e: {
                        keypad[6] = 0;
                    } break;

                    case SDLK_a: {
                        keypad[7] = 0;
                    } break;

                    case SDLK_s: {
                        keypad[8] = 0;
                    } break;

                    case SDLK_d: {
                        keypad[9] = 0;
                    } break;

                    case SDLK_z: {
                        keypad[0xA] = 0;
                    } break;

                    case SDLK_c: {
                        keypad[0xB] = 0;
                    } break;

                    case SDLK_4: {
                        keypad[0xC] = 0;
                    } break;

                    case SDLK_r: {
                        keypad[0xD] = 0;
                    } break;

                    case SDLK_f: {
                        keypad[0xE] = 0;
                    } break;

                    case SDLK_v: {
                        keypad[0xF] = 0;
                    } break;
                }
            } break;
        }
    }
}