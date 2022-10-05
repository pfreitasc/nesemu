# compiler:
CC = gcc

# compiler flags: 
CFLAGS  = -g2 -gdwarf -Wall

# the build target executable:
TARGET = bin/nestest

# object files to build:
OBJ = src/cpu.o src/instructions.o src/nestest.o src/cartridge.o src/nes.o src/ppu.o

all: $(TARGET)

$(TARGET): $(OBJ)
		$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)