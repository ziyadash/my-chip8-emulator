// Regression tests for op_Dxyn, the sprite draw instruction.
//
// The constants describing the screen were once named VIDEO_ROWS (64) and
// VIDEO_COLS (32), which had width and height the wrong way round. op_Dxyn
// trusted those names, so x wrapped at 32 instead of 64 and y wrapped at 64
// instead of 32 -- the latter indexing past the end of video[].

#include "test_helpers.h"
#include "../cpu.h"
#include "../state.h"

void op_Dxyn(chip8_state *state);   // defined in cpu.c

int main(void) {
    chip8_state s;

    // A sprite drawn beyond column 32 must stay where it was put
    initialise_state(&s);
    s.memory[0x300] = 0xFF;            // one solid 8-pixel row
    s.index_register = 0x300;
    s.v_register[0] = 40;              // Vx
    s.v_register[1] = 0;               // Vy
    s.opcode = 0xD011;                 // draw V0,V1 height 1
    op_Dxyn(&s);

    printf("horizontal placement at x=40:\n");
    check("pixel lit at x=40", s.video[0 * VIDEO_WIDTH + 40] == 0xFFFFFFFF);
    check("pixel lit at x=47", s.video[0 * VIDEO_WIDTH + 47] == 0xFFFFFFFF);
    check("nothing wrapped to x=8", s.video[0 * VIDEO_WIDTH + 8] == 0);

    // A sprite running off the bottom must wrap to the top, not run off the
    // end of the framebuffer
    initialise_state(&s);
    for (int i = 0; i < 4; i++) {
        s.memory[0x300 + i] = 0xFF;
    }
    s.index_register = 0x300;
    s.v_register[0] = 0;
    s.v_register[1] = 30;
    s.opcode = 0xD014;                 // height 4, so rows 30,31,0,1
    op_Dxyn(&s);

    printf("vertical wrap from y=30, height 4:\n");
    check("row 30 lit",       s.video[30 * VIDEO_WIDTH] == 0xFFFFFFFF);
    check("row 31 lit",       s.video[31 * VIDEO_WIDTH] == 0xFFFFFFFF);
    check("wrapped to row 0", s.video[ 0 * VIDEO_WIDTH] == 0xFFFFFFFF);
    check("wrapped to row 1", s.video[ 1 * VIDEO_WIDTH] == 0xFFFFFFFF);

    // Drawing is an XOR, and VF reports whether a lit pixel was turned off
    initialise_state(&s);
    s.memory[0x300] = 0xFF;
    s.index_register = 0x300;
    s.v_register[0] = 10;
    s.v_register[1] = 5;
    s.opcode = 0xD011;
    op_Dxyn(&s);

    printf("collision flag and XOR erase:\n");
    check("VF clear on first draw", s.v_register[0xF] == 0);
    op_Dxyn(&s);                       // same sprite again
    check("VF set on overdraw",     s.v_register[0xF] == 1);
    check("pixels XORed back off",  s.video[5 * VIDEO_WIDTH + 10] == 0);

    return test_report("sprites");
}
