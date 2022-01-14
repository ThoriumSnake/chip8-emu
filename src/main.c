#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

#include "chip8.h"

bool game_running = false;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;

//These two are used to essentially lock the fps to a certain value
static int fps;
static float frame_target_time;

int last_frame_ticks = 0;

int initialize_sdl(int window_width, int window_height) {
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
    SDL_SetWindowResizable(window, SDL_TRUE);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return false;
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);

    return true;
}

void setup() {
}

void process_input(bool debug) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
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
}

void update(bool debug) {
    //Uses SDL_Delay to prevent program from eating up all the cpu
    //https://www.udemy.com/course/game-loop-c-sdl/learn/lecture/18690134#overview for details
    //If statement to prevent negative values
    if (SDL_GetTicks64() < last_frame_ticks + frame_target_time)
        SDL_Delay(last_frame_ticks + frame_target_time - SDL_GetTicks64());

    //Current time elapsed, used in calculations
    last_frame_ticks = SDL_GetTicks64();

    if (!debug)
        emulate_cycle();
    print_debug();
}

void render(int pitch) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_UpdateTexture(texture, NULL, video, pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    //Swaps the back and front buffer
    SDL_RenderPresent(renderer);
}

void destroy_window() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("Program usage is: %s <ROM> <FPS> <VIDEO SCALE>", argv[0]);
        return 1;
    }

    bool debug = false;
    //Checks if debug value was passed in terminal
    if (argc > 4) {
        if (strcmp(argv[4], "debug") == 0)
            debug = true;
    }

    //Not exactly and ideal solution, I think it doesn't properly check for errors?? 0 Fucks given for now
    fps = (int)strtol(argv[2], (char**)NULL, 10);
    frame_target_time = 1000 / fps;

    int video_scale = (int)strtol(argv[3], (char**)NULL, 10);

    //Size of a row of pixels for SDL_UpdateTexture
    size_t pitch = sizeof(video[0]) * VIDEO_WIDTH;

    FILE* rom = fopen(argv[1], "rb");
    if (!setupChip8(rom)) {
        printf("ROM could not be read, maybe the path is wrong?\n");
        return 0;
    }
    run_tests(true);

    game_running = initialize_sdl(VIDEO_WIDTH * video_scale, VIDEO_HEIGHT * video_scale);
    setup();
    while (game_running) {
        process_input(debug);
        update(debug);
        render(pitch);
    }

    //Cleans shit after game is closed
    destroy_window();

    return 0;
}
