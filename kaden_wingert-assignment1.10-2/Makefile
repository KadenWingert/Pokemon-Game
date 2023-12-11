# Makefile for connect4.cpp

# Compiler and flags
CC = g++
CFLAGS = -Wall -Werror

# Libraries
LIBS = -lncurses

# Source and object files
SRC = connect4.cpp
OBJ = $(SRC:.cpp=.o)

# Target executable
TARGET = connect4

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
