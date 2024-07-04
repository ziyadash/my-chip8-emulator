#include "cpu.h"
#include <time.h>

void initialise_state(chip8_state* state)
{
	memset(state, 0, sizeof(chip8_state));
	state->program_counter = PROGRAM_OFFSET;
	load_font(state);
}

// Unique opcodes
void op_1nnn(chip8_state *state) {
    // Jump to location nnn.
    uint16_t address = state->opcode & 0x0FFFu;
    state->program_counter = address;
}

void op_2nnn(chip8_state *state) { 
    // Call subroutine at location nnn. 
    uint16_t address = state->opcode & 0x0FFFu;

	state->stack[state->stack_pointer] = state->program_counter;
	++state->stack_pointer;
	state->program_counter = address;
}

void op_3xkk(chip8_state *state) { 
    // Skip next instruction if Vx = kk.
    uint8_t val = state->opcode & 0x00FFu;
    uint8_t vx_index = (state->opcode & 0x0F00u) >> 8;

    if (state->v_register[vx_index] == val) {
        state->program_counter += 2;
    }
}

void op_4xkk(chip8_state *state) {
    // Skip next instruction if Vx ! kk.
    uint8_t val = state->opcode & 0x00FFu;
    uint8_t vx_index = (state->opcode & 0x0F00u) >> 8;

    if (state->v_register[vx_index] != val) {
        state->program_counter += 2;
    }
}

void op_5xy0(chip8_state *state) { 
    // Skip next instruction if Vx = Vy.
    uint8_t vx_index = (state->opcode & 0x0F00u) >> 8;
    uint8_t vy_index = (state->opcode & 0x00F0u) >> 4;

    if (state->v_register[vx_index] == state->v_register[vy_index]) {
            state->program_counter += 2;
    }
}

void op_6xkk(chip8_state *state) { 
    // Set Vx = kk.
    uint8_t vx_index = (state->opcode & 0x0F00) >> 8;
    uint8_t kk = state->opcode & 0x00FF;

    state->v_register[vx_index] = kk;
}

void op_7xkk(chip8_state *state) { 
    // Increment Vx by kk.
    uint8_t vx_index = (state->opcode & 0x0F00) >> 8;
    uint8_t kk = state->opcode & 0x00FF;

    state->v_register[vx_index] += kk;
}

void op_9xy0(chip8_state *state) { 
    // Skip next instruction if Vx != Vy.
    uint8_t vx_index = (state->opcode & 0x0F00u) >> 8;
    uint8_t vy_index = (state->opcode & 0x00F0u) >> 4;

    if (state->v_register[vx_index] != state->v_register[vy_index]) {
        state->program_counter += 2;
    }
}

void op_Annn(chip8_state *state) { 
    // Set I = nnn.
    uint16_t address = state->opcode & 0x0FFF;
    state->index_register = address;
}

void op_Bnnn(chip8_state *state) { 
    // Jump to location nnn + V0.

    uint16_t address = (state->opcode & 0x0FFFu) + state->v_register[0x0];
    state->program_counter = address;
}

void op_Cxkk(chip8_state *state) {  
    // Set Vx = random byte AND kk.
    uint8_t vx_index = (state->opcode & 0x0F00u) >> 8;
    uint8_t kk = state->opcode & 0x00FF;
    
    // generate random number, make sure it fits in 8 bits
    srand(time(NULL));
    uint8_t random_byte = rand() % 256;

    state->v_register[vx_index] = random_byte & kk;
}

void op_Dxyn(chip8_state *state) {  
    // Display n-byte sprite starting at memory location I at (Vx, Vy) and
    // set VF = collision when needed.
    uint8_t Vx = (state->opcode & 0x0F00u) >> 8;
    uint8_t Vy = (state->opcode & 0x00F0u) >> 4;
    uint8_t height = state->opcode & 0x000Fu;

    // Wrap xPos and yPos if going beyond screen boundaries
    uint8_t xPos = state->v_register[Vx] % VIDEO_COLS;
    uint8_t yPos = state->v_register[Vy] % VIDEO_ROWS;

    // Initialise collision flag
    state->v_register[0xF] = 0;

    // Loop through the rows of the sprite
    for (unsigned int row = 0; row < height; ++row)
    {
        // Get the current byte of the sprite from memory
        uint8_t spriteByte = state->memory[state->index_register + row];

        // Loop through the columns (bits) of the sprite byte
        // A sprite is 8 pixels wide
        for (unsigned int col = 0; col < 8; ++col)
        {
            // Check if the bit at this column in the sprite byte is set
            uint8_t spritePixel = spriteByte & (0x80u >> col);

            // Pointer to the screen pixel
            uint32_t screenIndex = ((yPos + row) % VIDEO_ROWS) * VIDEO_ROWS + ((xPos + col) % VIDEO_COLS);
            uint32_t* screenPixel = &state->video[screenIndex];

            // If the sprite pixel is on (not zero)
            if (spritePixel) {
                // Check if the screen pixel is currently on (white)
                if (*screenPixel == 0xFFFFFFFF) {
                    // Collision detected
                    state->v_register[0xF] = 1;
                }

                // Draw the sprite pixel by XORing it with the screen pixel
                *screenPixel ^= 0xFFFFFFFF;
            }
        }
    }
}

// 8xy_ grouped opcodes
void op_8xy0(chip8_state *state) { 
    // Set Vx = Vy.
    uint8_t vx_index = (state->opcode & 0x0F00u) >> 8;
    uint8_t vy_index = (state->opcode & 0x00F0u) >> 4;

    state->v_register[vx_index] = state->v_register[vy_index]; 
}

void op_8xy1(chip8_state *state) { 
    // Set Vx = Vx OR Vy.
    uint8_t vx_index = (state->opcode & 0x0F00u) >> 8;
    uint8_t vy_index = (state->opcode & 0x00F0u) >> 4;

    state->v_register[vx_index] |= state->v_register[vy_index]; 
}

void op_8xy2(chip8_state *state) { 
    // Set Vx = Vx AND Vy.
    uint8_t vx_index = (state->opcode & 0x0F00u) >> 8;
    uint8_t vy_index = (state->opcode & 0x00F0u) >> 4;

    state->v_register[vx_index] &= state->v_register[vy_index]; 
}

void op_8xy3(chip8_state *state) { 
    // Set Vx = Vx XOR Vy.
    uint8_t vx_index = (state->opcode & 0x0F00u) >> 8;
    uint8_t vy_index = (state->opcode & 0x00F0u) >> 4;

    state->v_register[vx_index] ^= state->v_register[vy_index]; 
}
void op_8xy4(chip8_state *state) { 
    // Set Vx = Vx + Vy, set VF = carry if needed.
    // carry is needed if the sum can't be stored in 8 bits i.e. exceeds 255
    uint8_t vx_index = (state->opcode & 0x0F00u) >> 8;
    uint8_t vy_index = (state->opcode & 0x00F0u) >> 4;

    uint16_t sum = state->v_register[vx_index] + state->v_register[vy_index];

    if (sum > 255) {
        state->v_register[0xF] = 1;
    } else {
        state->v_register[0xF] = 0;
    }
    state->v_register[vx_index] = sum % 256;
}
void op_8xy5(chip8_state *state) { 
    // Set Vx = Vx - Vy, set VF = NOT borrow.
    // If Vx > Vy, then VF is set to 1, otherwise 0. 
    uint8_t vx_index = (state->opcode & 0x0F00u) >> 8;
    uint8_t vy_index = (state->opcode & 0x00F0u) >> 4;

    uint16_t diff = state->v_register[vx_index] + state->v_register[vy_index];

    if (state->v_register[vx_index] > state->v_register[vy_index]) {
        state->v_register[0xF] = 1;
    } else {
        state->v_register[0xF] = 0;
    }


    state->v_register[vx_index] = diff;

}
void op_8xy6(chip8_state *state) { 
    // Set Vx = Vx >> 1 (shift right by 1)
    // save LSB in VF
    uint8_t vx_index = (state->opcode & 0x0F00u) >> 8;

    state->v_register[0xF] = state->v_register[vx_index] & 0x1;
    state->v_register[vx_index] >>= 1;

}
void op_8xy7(chip8_state *state) {
    // Set Vx = Vy - Vx, set VF = NOT borrow. 
    uint8_t vx_index = (state->opcode & 0x0F00u) >> 8;
    uint8_t vy_index = (state->opcode & 0x00F0u) >> 4;

	if (state->v_register[vy_index] > state->v_register[vx_index])
	{
		state->v_register[0xF] = 1;
	}
	else
	{
		state->v_register[0xF] = 0;
	}

    uint16_t diff = state->v_register[vy_index] - state->v_register[vx_index];
	state->v_register[vx_index] = diff;

}
void op_8xyE(chip8_state *state) { 
    // Set Vx = Vx << 1 (shift left by 1)
    // save MSB in VF
    uint8_t vx_index = (state->opcode & 0x0F00u) >> 8;

    state->v_register[0xF] = (state->v_register[vx_index] & 0x80) >> 7;
    state->v_register[vx_index] <<= 1;
}

// 00E_ grouped opcodes
void op_00E0(chip8_state *state) { 
    // clear the display
    // set buffer to 0
    memset(state->video, 0, sizeof(state->video));
}

void op_00EE(chip8_state *state) { 
    // return from a subroutine
    // decrement the stack pointer, and return the program counter
    // to where the stack pointer is pointing
    state->stack_pointer--;
    state->program_counter = state->stack[state->stack_pointer];
}

// Ex__ grouped opcodes
void op_ExA1(chip8_state *state) {  
    // Skip next instruction if key with the value of Vx is NOT pressed.
    uint8_t vx_index = (state->opcode & 0x0F00) >> 8;
    
    uint8_t key = state->v_register[vx_index];
    if (!state->keys[key]) {
        state->program_counter += 2;
    }
}

void op_Ex9E(chip8_state *state) { 
    // Skip next instruction if key with the value of Vx is pressed.
    uint8_t vx_index = (state->opcode & 0x0F00) >> 8;
    
    uint8_t key = state->v_register[vx_index];
    if (state->keys[key]) {
        state->program_counter += 2;
    }
}

// Fx__ grouped opcodes
void op_Fx07(chip8_state *state) {  
    // Set Vx = delay timer value.
    uint8_t vx_index = (state->opcode & 0x0F00) >> 8;

    state->v_register[vx_index] = state->delay_timer;
}

void op_Fx0A(chip8_state *state) {  
    // Wait for a key press, store the value of the key in Vx.
    uint8_t vx_index = (state->opcode & 0x0F00u) >> 8;

    // Iterate over all possible key values (0 to 15)
    for (uint8_t i = 0; i < 16; ++i)
    {
        if (state->keys[i])
        {
            // Store the key value in Vx
            state->v_register[vx_index] = i;
            return; // Exit the function as soon as a key is detected
        }
    }

    // If no key is pressed, decrement the PC to wait for the next key press
    state->program_counter -= 2;
}

void op_Fx15(chip8_state *state) { 
    // Set delay timer = Vx.
    uint8_t vx_index = (state->opcode & 0x0F00u) >> 8;

    state->delay_timer = state->v_register[vx_index];
}

void op_Fx18(chip8_state *state) {  
    // Set sound timer = Vx.
    uint8_t vx_index = (state->opcode & 0x0F00u) >> 8;

    state->sound_timer = state->v_register[vx_index];
}

void op_Fx1E(chip8_state *state) { 
    // Set I = I + Vx.
    uint8_t vx_index = (state->opcode & 0x0F00u) >> 8;

    state->index_register += state->v_register[vx_index];

}
void op_Fx29(chip8_state *state) { 
    // Set I = location of sprite for digit Vx.
    // a font character is 5 bytes, and they are stored starting at 0x50.
    uint8_t vx_index = (state->opcode & 0x0F00u) >> 8;
    uint8_t digit = state->v_register[vx_index];

    state->index_register = FONT_OFFSET + (5 * digit);
}

void op_Fx33(chip8_state *state) { 
    // Store BCD representation of Vx in memory locations I, I+1, and I+2.
    uint8_t vx_index = (state->opcode & 0x0F00u) >> 8;
    uint8_t val = state->v_register[vx_index];
    uint16_t index = state->index_register;

    // store it in "big endian", ones place in the highest index
    // integer division by 10 eliminates that place value
    // ones place
    state->memory[index + 2] = val % 10;
    val /= 10;

    // tens place
    state->memory[index + 1] = val % 10;
    val /= 10;

    // hundreds place
    state->memory[index] = val % 10;
}

void op_Fx55(chip8_state *state) {  
    // Store registers V0 through Vx in memory starting at location I.
    uint8_t vx_index = (state->opcode & 0x0F00u) >> 8;
    uint16_t index = state->index_register;

    for (uint8_t i = 0; i <= vx_index; ++i) {
        state->memory[index + i] = state->v_register[i];
    }
}

void op_Fx65(chip8_state *state) { 
    // Read registers V0 through Vx from memory starting at location I.
    uint8_t vx_index = (state->opcode & 0x0F00u) >> 8;
    uint16_t index = state->index_register;

    for (uint8_t i = 0; i <= vx_index; ++i) {
        state->v_register[i] = state->memory[index + i];
    }
}

void execute_opcode(chip8_state *state) {
    // Fetch opcode (already stored in state->opcode)

    // Decode and execute opcode
    uint16_t opcode = state->opcode;
    uint16_t first_nibble = (opcode & 0xF000u) >> 12;

    switch (first_nibble) {
        case 0x0:
            // Special handling for 0x00E0 and 0x00EE
            switch (opcode & 0x00FFu) {
                case 0x00E0:
                    op_00E0(state);
                    break;
                case 0x00EE:
                    op_00EE(state);
                    break;
                default:
                    printf("Unknown opcode: 0x%X\n", opcode);
                    break;
            }
            break;
        case 0x1:
            op_1nnn(state);
            break;
        case 0x2:
            op_2nnn(state);
            break;
        case 0x3:
            op_3xkk(state);
            break;
        case 0x4:
            op_4xkk(state);
            break;
        case 0x5:
            op_5xy0(state);
            break;
        case 0x6:
            op_6xkk(state);
            break;
        case 0x7:
            op_7xkk(state);
            break;
        case 0x8:
            switch (opcode & 0x000Fu) {
                case 0x0:
                    op_8xy0(state);
                    break;
                case 0x1:
                    op_8xy1(state);
                    break;
                case 0x2:
                    op_8xy2(state);
                    break;
                case 0x3:
                    op_8xy3(state);
                    break;
                case 0x4:
                    op_8xy4(state);
                    break;
                case 0x5:
                    op_8xy5(state);
                    break;
                case 0x6:
                    op_8xy6(state);
                    break;
                case 0x7:
                    op_8xy7(state);
                    break;
                case 0xE:
                    op_8xyE(state);
                    break;
                default:
                    printf("Unknown opcode: 0x%X\n", opcode);
                    break;
            }
            break;
        case 0x9:
            op_9xy0(state);
            break;
        case 0xA:
            op_Annn(state);
            break;
        case 0xB:
            op_Bnnn(state);
            break;
        case 0xC:
            op_Cxkk(state);
            break;
        case 0xD:
            op_Dxyn(state);
            break;
        case 0xE:
            switch (opcode & 0x00FFu) {
                case 0x9E:
                    op_Ex9E(state);
                    break;
                case 0xA1:
                    op_ExA1(state);
                    break;
                default:
                    printf("Unknown opcode: 0x%X\n", opcode);
                    break;
            }
            break;
        case 0xF:
            switch (opcode & 0x00FFu) {
                case 0x07:
                    op_Fx07(state);
                    break;
                case 0x0A:
                    op_Fx0A(state);
                    break;
                case 0x15:
                    op_Fx15(state);
                    break;
                case 0x18:
                    op_Fx18(state);
                    break;
                case 0x1E:
                    op_Fx1E(state);
                    break;
                case 0x29:
                    op_Fx29(state);
                    break;
                case 0x33:
                    op_Fx33(state);
                    break;
                case 0x55:
                    op_Fx55(state);
                    break;
                case 0x65:
                    op_Fx65(state);
                    break;
                default:
                    printf("Unknown opcode: 0x%X\n", opcode);
                    break;
            }
            break;
        default:
            printf("Unknown opcode: 0x%X\n", opcode);
            break;
    }
}


void emu_cycle(chip8_state *state) {
    // Fetch opcode
    state->opcode = (state->memory[state->program_counter] << 8) | state->memory[state->program_counter + 1];

    // Increment the PC before executing
    state->program_counter += 2;

    // Decode and execute the fetched opcode
    execute_opcode(state);

    // Decrement the delay timer if it's been set
    if (state->delay_timer > 0) {
        --state->delay_timer;
    }

    // Decrement the sound timer if it's been set
    if (state->sound_timer > 0) {
        --state->sound_timer;
    }
}
