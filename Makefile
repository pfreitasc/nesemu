# compiler:
CC = gcc

#OBJS specifies which files to compile as part of the project
OBJS = src/cpu.c src/instructions.c src/nestest.c src/cartridge.c src/nes.c src/ppu.c src/graphics.c

# compiler flags: 
COMPILER_FLAGS  = -g2 -gdwarf -Wall

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2

# the build target executable:
TARGET = bin/nestest

# object files to build:
#OBJ = src/cpu.o src/instructions.o src/nestest.o src/cartridge.o src/nes.o src/ppu.o src/graphics.o

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(TARGET)