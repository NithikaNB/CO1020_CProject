# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude -O2
LDFLAGS = -lm

# Directories
SRCDIR = src
DEMODIR = demo
TESTDIR = tests

# Source files
COMMON_SRC = $(SRCDIR)/canvas.c
DEMO_SRC = $(COMMON_SRC) $(DEMODIR)/main.c
MATH_SRC = $(COMMON_SRC) $(SRCDIR)/math3d.c $(TESTDIR)/test_math.c

# Targets
DEMO_TARGET = demo.exe
TEST_TARGET = test_math.exe

# Default build
all: $(DEMO_TARGET) $(TEST_TARGET)

# Build demo
$(DEMO_TARGET): $(DEMO_SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Build math3d test
$(TEST_TARGET): $(MATH_SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Run demo
run-demo: $(DEMO_TARGET)
	./$(DEMO_TARGET)

# Run test
run-test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Debug build
debug: CFLAGS += -DDEBUG -g
debug: clean all

# Release build
release: CFLAGS += -O3 -DNDEBUG
release: clean all

# Clean build artifacts
clean:
	-del /Q *.exe *.pgm 2>nul || echo Cleaned (Windows)
	@rm -f *.exe *.pgm 2>/dev/null || echo Cleaned (Unix)

# Help message
help:
	@echo "Available targets:"
	@echo "  all        - Build both demo and test_math"
	@echo "  run-demo   - Run the demo executable"
	@echo "  run-test   - Run the test_math executable"
	@echo "  debug      - Clean and build with debug symbols"
	@echo "  release    - Clean and build optimized release"
	@echo "  clean      - Remove executables and pgm files"
	@echo "  help       - Show this help message"

.PHONY: all run-demo run-test debug release clean help
