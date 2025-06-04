
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude
LDFLAGS = -lm

SRCDIR = src
DEMODIR = demo
SOURCES = $(SRCDIR)/canvas.c $(DEMODIR)/main.c

TARGET = demo.exe

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) $(LDFLAGS) -o $(TARGET)

run: $(TARGET)
	$(TARGET)

clean:
	del /Q $(TARGET) *.pgm 2>nul || echo "Clean completed"

.PHONY: all run clean