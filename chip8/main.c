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

// The CHIP-8 delay and sound timers are specified to run at 60 Hz, and 60 Hz
// is also a sensible rate to push frames to the screen at.
#define TIMER_HZ 60
#define TIMER_INTERVAL_MS (1000.0f / TIMER_HZ)

// If the process is suspended or the window is dragged, the measured frame
// delta can be enormous. Clamp it so the catch-up loops below stay bounded.
#define MAX_FRAME_DELTA_MS 100.0f

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <Scale> <Delay> <ROM>\n", argv[0]);
        return EXIT_FAILURE;
    }
    printf("yippee!\n");

    // Parse command line arguments
    int video_scale = atoi(argv[1]);
    int cycle_delay = atoi(argv[2]);

    if (video_scale <= 0 || cycle_delay <= 0) {
        fprintf(stderr, "Scale and Delay must both be positive integers\n");
        return EXIT_FAILURE;
    }
    char *rom_file_name = argv[3];

    // Initialise Chip-8 state
    chip8_state state;
    initialise_state(&state);

    if (loadROM(rom_file_name, &state) != 0) {
        return EXIT_FAILURE;
    }

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

    // SDL_GetTicks is a wall-clock millisecond counter. clock() was used here
    // before, but that measures CPU time consumed by the process, which only
    // tracked real time because the loop below used to busy-spin.
    uint32_t previous_time = SDL_GetTicks();

    // Instructions and timers advance at different rates, so each gets its own
    // accumulator and each drains independently of the other.
    float cycle_accumulator = 0.0f;
    float timer_accumulator = 0.0f;

    // Main loop
    while (!quit) {
        // Process input
        quit = render_process_input(state.keys);

        uint32_t current_time = SDL_GetTicks();
        float dt = (float)(current_time - previous_time);
        previous_time = current_time;

        if (dt > MAX_FRAME_DELTA_MS) {
            dt = MAX_FRAME_DELTA_MS;
        }

        cycle_accumulator += dt;
        timer_accumulator += dt;

        // Step the CPU at the rate the user asked for
        while (cycle_accumulator >= cycle_delay) {
            emu_cycle(&state);
            cycle_accumulator -= cycle_delay;
        }

        // Tick the timers at a fixed 60 Hz and redraw on the same beat, so
        // neither depends on the chosen instruction speed
        bool needs_redraw = false;
        while (timer_accumulator >= TIMER_INTERVAL_MS) {
            update_timers(&state);
            timer_accumulator -= TIMER_INTERVAL_MS;
            needs_redraw = true;
        }

        if (needs_redraw) {
            render_update(&renderCtx, state.video, video_pitch);
        }

        // Hand the CPU back rather than spinning flat out between frames
        SDL_Delay(1);
    }

    // Clean up SDL resources
    render_cleanup(&renderCtx);

    return 0;
}
