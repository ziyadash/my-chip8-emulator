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

# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------
# The suites exercise cpu.c, font.c and loadROM.c, none of which touch SDL, so
# they build and run without a display. AddressSanitizer is on because the
# out-of-bounds framebuffer write these tests cover is exactly what it catches.

TEST_DIR   = tests
TEST_SRC   = $(wildcard $(TEST_DIR)/test_*.c)
TEST_BINS  = $(TEST_SRC:.c=)
TEST_DEPS  = cpu.c font.c loadROM.c
TEST_FLAGS = -Wall -Wextra -g -O1 -fsanitize=address

$(TEST_DIR)/%: $(TEST_DIR)/%.c $(TEST_DEPS)
	$(CC) $(TEST_FLAGS) $(INCLUDE) $< $(TEST_DEPS) -o $@

test: $(TEST_BINS)
	@fail=0; \
	for t in $(TEST_BINS); do \
		echo "=== $$t"; \
		./$$t || fail=1; \
	done; \
	if [ $$fail -ne 0 ]; then echo "TESTS FAILED"; exit 1; fi; \
	echo "All test suites passed."

# Clean Up
clean:
	rm -f $(OBJ) $(TARGET) $(TEST_BINS)

# Run the emulator (adjust as necessary)
run: $(TARGET)
	./$(TARGET) $(SCALE) $(DELAY) $(ROM)

# Phony Targets
.PHONY: all clean run test
