# Compiler
CC = gcc

# Compiler Flags
CFLAGS = -Wall -Wextra -g -O2

# Linker Flags (SDL2)
LDFLAGS = -lSDL2

# Executable Name
TARGET = chip8_emulator

# Source Files
SRC = cpu.c font.c loadROM.c main.c render_screen.c

# Object Files (replace .c with .o in the SRC list)
OBJ = $(SRC:.c=.o)

# Include Path (assuming headers are in the current directory)
INCLUDE = -I .

# Default Rule
all: $(TARGET)

# Linking
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

# Compilation
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

# Clean Up
clean:
	rm -f $(OBJ) $(TARGET)

# Run the emulator (adjust as necessary)
run: $(TARGET)
	./$(TARGET) $(SCALE) $(DELAY) $(ROM)

# Phony Targets
.PHONY: all clean run
