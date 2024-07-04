#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "state.h"
#include "loadROM.h"
#include "font.h"
#include "cpu.h"
#include "render_screen.h"

#define VIDEO_WIDTH 64
#define VIDEO_HEIGHT 32

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <Scale> <Delay> <ROM>\n", argv[0]);
        return EXIT_FAILURE;
    }
    printf("yippee!\n");

    // Parse command line arguments
    int video_scale = atoi(argv[1]);
    int cycle_delay = atoi(argv[2]);
    char *rom_file_name = argv[3];

    // Initialise Chip-8 state
    chip8_state state;
    initialise_state(&state);
    loadROM(rom_file_name, &state);

    // Initialise SDL for rendering
    RenderContext renderCtx;
    int scaled_width = VIDEO_WIDTH * video_scale;
    int scaled_height = VIDEO_HEIGHT * video_scale;
    if (render_initialise(&renderCtx, "CHIP-8 Emulator", scaled_width, scaled_height, VIDEO_WIDTH, VIDEO_HEIGHT) != 0) {
        fprintf(stderr, "Failed to initialise rendering context\n");
        return EXIT_FAILURE;
    }

    // Main loop variables
    bool quit = false;
    uint32_t video_pitch = sizeof(state.video[0]) * VIDEO_WIDTH;
    clock_t last_cycle_time = clock();

    // Main loop
    while (!quit) {
        // Process input
        quit = render_process_input(state.keys);

        // Timing for the cycle delay
        clock_t current_time = clock();
        float dt = (float)(current_time - last_cycle_time) / CLOCKS_PER_SEC * 1000.0f; // Convert to milliseconds

        if (dt > cycle_delay) {
            last_cycle_time = current_time;

            // Execute a Chip-8 cycle
            emu_cycle(&state);

            // Update the rendering
            render_update(&renderCtx, state.video, video_pitch);
        }
    }

    // Clean up SDL resources
    render_cleanup(&renderCtx);

    return 0;
}
