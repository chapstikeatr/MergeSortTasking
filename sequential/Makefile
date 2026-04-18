# Compiler and flags
CXX = g++ #-15
CXXFLAGS = -std=c++17 -O2 

# Target executable name
TARGET = mergesort_seq

# Source files
SRC = mergesort_seq_nocopy.cpp

# Default target
all: $(TARGET)

# Build rule
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Clean rule
clean:
	rm -f $(TARGET)
