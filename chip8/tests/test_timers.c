// Regression tests for the delay and sound timers.
//
// These used to be decremented inside emu_cycle, once per instruction, which
// tied their rate to the CPU speed: measured at 998 Hz, 499 Hz and 124 Hz for
// cycle delays of 1, 2 and 8 against a specified 60 Hz. They now live in
// update_timers, which the main loop drives from its own 60 Hz accumulator.

#include "test_helpers.h"
#include "../cpu.h"
#include "../state.h"

int main(void) {
    chip8_state s;
    initialise_state(&s);

    // 6xkk: set V0 = 0x42. Harmless, and re-runnable in place.
    s.memory[0x200] = 0x60;
    s.memory[0x201] = 0x42;
    s.delay_timer = 5;
    s.sound_timer = 3;

    for (int i = 0; i < 500; i++) {
        s.program_counter = 0x200;
        emu_cycle(&s);
    }

    printf("timers are independent of instruction execution:\n");
    check("delay_timer untouched by 500 instructions", s.delay_timer == 5);
    check("sound_timer untouched by 500 instructions", s.sound_timer == 3);

    printf("update_timers counts down and stops at zero:\n");
    update_timers(&s);
    update_timers(&s);
    update_timers(&s);
    check("delay_timer 5 -> 2 after three ticks", s.delay_timer == 2);
    check("sound_timer 3 -> 0 after three ticks", s.sound_timer == 0);

    for (int i = 0; i < 10; i++) {
        update_timers(&s);
    }
    check("delay_timer floors at 0",      s.delay_timer == 0);
    check("sound_timer does not wrap",    s.sound_timer == 0);

    return test_report("timers");
}
