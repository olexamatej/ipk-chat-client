# Compiler
CC := g++
LDFLAGS=-pthread
# Compiler flags
CFLAGS := -Wall -Wextra -std=c++20 -g

# Source files
SRCS := inputParser.cpp tcp_client.cpp main.cpp udp_client.cpp

# Object files
OBJS := $(SRCS:.cpp=.o)

# Header files
HDRS := $(wildcard *.h)

# Target executable
TARGET := main

# Default target
all: $(TARGET)

# Rule to compile object files
%.o: %.cpp $(HDRS)
	$(CC) $(LDFLAGS) $(CFLAGS) -c $< -o $@

# Rule to link object files and create the executable
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(CFLAGS) $^ -o $@

# Clean rule
clean:
	rm -f $(OBJS) $(TARGET)

run:
	./main