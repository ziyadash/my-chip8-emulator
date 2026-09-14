#include <string.h>

#include "state.h"
#include "loadROM.h"
#include "font.h"

// Initialize opcode tables
void initialise_opcode_tables(chip8_state *state);

// Fetch, decode, and execute an opcode
void execute_opcode(chip8_state *state);

// initialise the actual chip8 system
void initialise_state(chip8_state *state);

void emu_cycle(chip8_state *state);

// decrement the delay and sound timers; call at 60 Hz
void update_timers(chip8_state *state);