#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

#include "chip8.h"
#include "window.h"

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("Program usage is: %s <ROM> <FRAME MS> <VIDEO SCALE>", argv[0]);
        return 1;
    }

    bool debug = false;
    //Checks if debug value was passed in terminal
    if (argc > 4) {
        if (strcmp(argv[4], "debug") == 0)
            debug = true;
    }

    FILE* rom = fopen(argv[1], "rb");
    if (!setupChip8(rom)) {
        printf("ROM could not be read, maybe the path is wrong?\n");
        return 0;
    }

    //Not exactly and ideal solution, I think it doesn't properly check for errors?? 0 Fucks given for now
    int fps_ms_target = (int)strtol(argv[2], (char**)NULL, 10);

    int video_scale = (int)strtol(argv[3], (char**)NULL, 10);

    //Size of a row of pixels for SDL_UpdateTexture
    int pitch = sizeof(video[0]) * VIDEO_WIDTH;

    run_tests(false);

    game_running = initialize_sdl(VIDEO_WIDTH * video_scale, VIDEO_HEIGHT * video_scale, false);
    while (game_running) {
        uint64_t start_frame_ms = SDL_GetTicks64();

        process_input(debug);
        update(debug);
        render(pitch);

        uint64_t end_frame_ms = start_frame_ms - SDL_GetTicks64();
        if (end_frame_ms < fps_ms_target)
            SDL_Delay(fps_ms_target - end_frame_ms);
    }

    //Cleans shit after game is closed
    destroy_window();

    return 0;
}
