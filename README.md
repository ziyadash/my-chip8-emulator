# CHIP-8 Emulator

## Overview

This project is a CHIP-8 emulator written in C. The CHIP-8 is a simple, interpreted programming language that was first used on some early personal computers. This emulator allows you to run classic CHIP-8 programs and games, providing a window to retro computing.

## Features

- Emulates the CHIP-8 CPU, including all 34 opcodes.
- Displays graphics in a 64x32 monochrome screen.
- Handles input from the CHIP-8's original 16-key keypad, using the below keymap:
Keypad       Keyboard
+-+-+-+-+    +-+-+-+-+
|1|2|3|C|    |1|2|3|4|
+-+-+-+-+    +-+-+-+-+
|4|5|6|D|    |Q|W|E|R|
+-+-+-+-+ => +-+-+-+-+
|7|8|9|E|    |A|S|D|F|
+-+-+-+-+    +-+-+-+-+
|A|0|B|F|    |Z|X|C|V|
+-+-+-+-+    +-+-+-+-+
- Implements a basic SDL-based rendering and input system.

## Prerequisites

To build and run the emulator, you will need:

- `gcc` or another C compiler.
- `SDL2` library installed on your system.

## Building the Emulator

1. Open your terminal and navigate to the project directory.
2. Run the following command to build the emulator:

   ```bash
   make
   ```
## Running the emulator
To run the emulator, use the following command:
  ```bash
  ./chip8_emulator <Scale> <Delay> <ROM>
```
where:
<Scale>: The scale factor for the display window (e.g., 10 for a 640x320 window).
<Delay>: The cycle delay in milliseconds, controlling the speed of emulation.
<ROM>: The path to the CHIP-8 ROM file you want to load.

For example,
  ```bash
  ./chip8_emulator 10 2 tetris.ch8
  ```

## ROMs
Two ROMs can be found in this repo's ROMs folder. More can be found [here](https://github.com/dmatlack/chip8/tree/master/roms). 

