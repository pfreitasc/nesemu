# compiler:
CC = gcc

# compiler flags: 
CFLAGS  = -g2 -gdwarf -Wall

# the build target executable:
TARGET = bin/cputest

# object files to build:
OBJ = src/cpu.o src/instructions.o src/cputest.o

all: $(TARGET)

$(TARGET): $(OBJ)
		$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)