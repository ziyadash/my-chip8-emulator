#include "loadROM.h"

#include <errno.h>
#include <string.h>

// A ROM is loaded at PROGRAM_OFFSET, so anything larger than the space above
// it cannot be stored without running off the end of state->memory.
#define MAX_ROM_SIZE (MEMORY_SPACE - PROGRAM_OFFSET)

// Opens fileName as given. If that fails and the name carries no directory
// component, retries inside ./ROMs, where this repo keeps its bundled ROMs,
// so that both "tetris.ch8" and a real path work.
static FILE *open_rom(const char *fileName) {
    FILE *fptr = fopen(fileName, "rb");

    if (fptr != NULL || strchr(fileName, '/') != NULL) {
        return fptr;
    }

    char filePath[512];
    int written = snprintf(filePath, sizeof(filePath), "./ROMs/%s", fileName);
    if (written < 0 || (size_t)written >= sizeof(filePath)) {
        return NULL;
    }

    return fopen(filePath, "rb");
}

int loadROM(const char *fileName, chip8_state *state) {
    FILE *fptr = open_rom(fileName);
    if (fptr == NULL) {
        fprintf(stderr, "Could not open ROM '%s': %s\n", fileName, strerror(errno));
        return -1;
    }

    // Measure the file up front so an oversized ROM is refused rather than
    // written past the end of state->memory
    if (fseek(fptr, 0, SEEK_END) != 0) {
        fprintf(stderr, "Could not seek in ROM '%s': %s\n", fileName, strerror(errno));
        fclose(fptr);
        return -1;
    }

    long size = ftell(fptr);
    if (size < 0) {
        fprintf(stderr, "Could not size ROM '%s': %s\n", fileName, strerror(errno));
        fclose(fptr);
        return -1;
    }

    if (size == 0) {
        fprintf(stderr, "ROM '%s' is empty\n", fileName);
        fclose(fptr);
        return -1;
    }

    if (size > MAX_ROM_SIZE) {
        fprintf(stderr, "ROM '%s' is %ld bytes; only %d bytes are available from 0x%03X\n",
                fileName, size, MAX_ROM_SIZE, PROGRAM_OFFSET);
        fclose(fptr);
        return -1;
    }

    rewind(fptr);

    // Read straight into the program region; the previous intermediate
    // malloc'd buffer bought nothing and added a failure path of its own
    size_t bytes_read = fread(state->memory + PROGRAM_OFFSET, 1, (size_t)size, fptr);
    if (bytes_read != (size_t)size) {
        fprintf(stderr, "Short read on ROM '%s': expected %ld bytes, got %zu\n",
                fileName, size, bytes_read);
        fclose(fptr);
        return -1;
    }

    fclose(fptr);
    return 0;
}
