#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

#include "chip8.h"

static const int WINDOW_WIDTH = 800;
static const int WINDOW_HEIGHT = 600;

bool game_running = false;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

//These two are used to essentially lock the fps to a certain value
static int fps;
static int frame_target_time;

int last_frame_ticks = 0;

int initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    window = SDL_CreateWindow(
        "Chip8",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0);
    if (!window) {
        fprintf(stderr, "Error creating SDL Window.\n");
        return false;
    }
    SDL_SetWindowResizable(window, true);

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return false;
    }

    return true;
}

void setup() {
}

void process_input() {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        //Used for closing the window with the window's close button (The red X)
        case SDL_QUIT:
            game_running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
                game_running = false;
    }
}
void update() {
    //Uses SDL_Delay to prevent program from eating up all the cpu
    //https://www.udemy.com/course/game-loop-c-sdl/learn/lecture/18690134#overview for details
    //If statement to prevent negative values
    if (SDL_GetTicks64() < last_frame_ticks + frame_target_time)
        SDL_Delay(last_frame_ticks + frame_target_time - SDL_GetTicks64());

    //How long the previous frame took to process, can be used to make objects move in terms of pixels or units per seconds instead of per frame
    //Has been converted to seconds
    float delta_time = (SDL_GetTicks64() - last_frame_ticks) / 1000.0f;

    //Current time elapsed, used in calculations
    last_frame_ticks = SDL_GetTicks64();
}
void render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    //Swaps the back and front buffer
    SDL_RenderPresent(renderer);
}

void destroy_window() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("Program usage is: %s <ROM> <FPS> <VIDEO SCALE>", argv[0]);
        return 1;
    }

    //Not exactly and ideal solution, I think it doesn't properly check for errors?? 0 Fucks given for now
    fps = (int)strtol(argv[2], (char**)NULL, 10);
    printf("FPS: %d\n\n", fps);
    frame_target_time = 1000 / fps;

    FILE* rom = fopen(argv[1], "rb");
    setupChip8(rom);
    run_tests(true);

    game_running = initialize_window();
    setup();
    while (game_running) {
        process_input();
        update();
        render();
    }

    //Cleans shit after game is closed
    destroy_window();

    return 0;
}
