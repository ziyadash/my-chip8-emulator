#include "loadROM.h"
#include <string.h>

void loadROM(char *fileName, chip8_state *state) {
    // get filepath
    char filePath[256];
    snprintf(filePath, sizeof(filePath), "./ROMs/%s", fileName);

    // open file as binary
    FILE *fptr = fopen(filePath, "rb");
    if (fptr == NULL) {
        exit(0);
    }

    // move file pointer to the end, use ftell to get file size
    fseek(fptr, 0, SEEK_END);
    int size = ftell(fptr);
    
    // allocate buffer to store contents
    char *buffer = malloc(size);
    if (buffer == NULL) {
        fclose(fptr);
        exit(0);
    }

    // move file pointer back to start, load rom contents into state->memory
    fseek(fptr, 0, SEEK_SET);
    fread(buffer, 1, size, fptr);
    memcpy(state->memory + PROGRAM_OFFSET, buffer, size);

    // clean up
    free(buffer);
    fclose(fptr);

    return;
}

// void loadROM(const char *filename, chip8_state *state) {
//     FILE *rom = fopen(filename, "rb");
//     if (!rom) {
//         fprintf(stderr, "Failed to open ROM: %s\n", filename);
//         return -1;
//     }

//     fread(state->memory + PROGRAM_OFFSET, 1, MEMORY_SPACE - PROGRAM_OFFSET, rom);
//     fclose(rom);
//     return 0;
// }