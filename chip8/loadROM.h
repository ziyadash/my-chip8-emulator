#include <stdio.h>
#include <stdlib.h>
#include "state.h"

// Loads a program (ROM) into the state. Returns 0 on success, or -1 after
// printing the reason to stderr. fileName may be a path, or a bare name to be
// looked up in ./ROMs.
int loadROM(const char *fileName, chip8_state *state);