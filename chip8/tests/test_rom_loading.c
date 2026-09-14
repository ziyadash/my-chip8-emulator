// Regression tests for loadROM.
//
// loadROM used to call exit(0) when a file could not be opened, terminating
// the emulator with a success status and no message. It also hardcoded its
// path as ./ROMs/%s, and never checked the file size before copying into
// memory -- a ROM larger than MEMORY_SPACE - PROGRAM_OFFSET (3584 bytes)
// overran memory[] and corrupted the chip8_state fields laid out after it.

#include "test_helpers.h"
#include "../cpu.h"
#include "../loadROM.h"
#include "../state.h"

#include <stdlib.h>
#include <string.h>

// Writes `size` bytes of `fill` to `path`. Returns 0 on success.
static int write_fixture(const char *path, unsigned char fill, long size) {
    FILE *f = fopen(path, "wb");
    if (f == NULL) {
        return -1;
    }
    for (long i = 0; i < size; i++) {
        fputc(fill, f);
    }
    fclose(f);
    return 0;
}

int main(void) {
    chip8_state s;
    const char *ok_rom      = "test_ok.ch8";
    const char *big_rom     = "test_oversized.ch8";
    const char *empty_rom   = "test_empty.ch8";
    const long  max_rom     = MEMORY_SPACE - PROGRAM_OFFSET;

    if (write_fixture(ok_rom, 0xAB, 16) != 0 ||
        write_fixture(big_rom, 0xCD, max_rom + 1) != 0 ||
        write_fixture(empty_rom, 0x00, 0) != 0) {
        fprintf(stderr, "could not create test fixtures\n");
        return 1;
    }

    printf("failures are reported rather than silently exiting:\n");
    initialise_state(&s);
    check("missing file rejected", loadROM("no_such_rom.ch8", &s) != 0);
    check("empty file rejected",   loadROM(empty_rom, &s) != 0);

    // The important one: an oversized ROM must be refused, not copied past
    // the end of memory[] into the fields that follow it.
    initialise_state(&s);
    uint16_t pc_before = s.program_counter;
    uint16_t i_before  = s.index_register;
    uint8_t  sp_before = s.stack_pointer;

    check("oversized ROM rejected", loadROM(big_rom, &s) != 0);
    check("program_counter not corrupted", s.program_counter == pc_before);
    check("index_register not corrupted",  s.index_register  == i_before);
    check("stack_pointer not corrupted",   s.stack_pointer   == sp_before);
    check("video buffer not corrupted",    s.video[0] == 0);

    printf("a good ROM loads at PROGRAM_OFFSET:\n");
    initialise_state(&s);
    check("load succeeds", loadROM(ok_rom, &s) == 0);
    check("first byte at 0x200",  s.memory[PROGRAM_OFFSET] == 0xAB);
    check("last byte at 0x20F",   s.memory[PROGRAM_OFFSET + 15] == 0xAB);
    check("nothing written past the ROM", s.memory[PROGRAM_OFFSET + 16] == 0x00);
    check("font still intact at 0x50",    s.memory[FONT_OFFSET] == 0xF0);

    remove(ok_rom);
    remove(big_rom);
    remove(empty_rom);

    return test_report("rom loading");
}
