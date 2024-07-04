#ifndef STATE_H
#define STATE_H
#include <stdint.h>
/*
this defines the structure of the actual CPU object;
- 16 8-bit registers, labelled V0 to VF. Each register, naturally, can hold values from 0x0 to 0xFF

- 4096 (16^3) bytes of memory. Address space is labelled from 0x000 to 0xFFF. The space is segmented as follows:
    - 0x000-0x1FF: read-only, originally reserved for the chip-8 interpeter. this is an emulator so we won't use this.
    except for writing in the 16 characters the system uses, 0 to F
    - 0x200-0xFFF: instructions from the ROM are stored starting at 0x200.
    - Anything else is free for programs to use. 

- A 16-bit index register

- A 16 bit PC (program counter). Keeps track of which instruction we are up to. An instruction is 2 bytes but 
memory is indexed in individual bytes, so we will need to concatenate 2 bytes together, PC and PC + 1, for an instruction,
then increment PC by 2.

- A 16-level stack. Each stack level can hold a 16-bit PC. 

- An 8-bit stack pointer for our stack, similar to PC. 

- An 8-bit delay timer. If timer value is 0, it stays at 0. If loaded with a value, it decrements at a rate of 60 Hz. 

- An 8-bit sound timer. TODO: IMPLEMENT LATER

- 16 input keys

- 64x32 Monochrome Display Memory
*/

// define some constants
#define MEMORY_SPACE 4096
#define NUM_V_REG 16
#define STACK_DEPTH 16
#define VIDEO_ROWS 64
#define VIDEO_COLS 32
#define NUM_KEYS 16
#define FONT_OFFSET 0x50
#define PROGRAM_OFFSET 0x200

// Forward declaration of chip8_state
typedef struct chip8_state chip8_state;

struct chip8_state {
    // actual characteristics of the chip8 system
    uint8_t v_register[NUM_V_REG];
    uint8_t memory[MEMORY_SPACE];
    uint16_t index_register;
    uint16_t program_counter; // should be initialised to PROGRAM_OFFSET
    uint16_t stack[STACK_DEPTH];
    uint8_t stack_pointer;
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint32_t video[VIDEO_ROWS * VIDEO_COLS]; // use a 32 bit int to make using SDL easier
    uint8_t keys[NUM_KEYS];
    uint32_t opcode; // an instruction
};

#endif // STATE_H
