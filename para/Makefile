
# Compiler and flags
CXX = g++-15
CXXFLAGS = -std=c++17 -O3  -fopenmp

# Target executable name
TARGET = mergesort_para

# Source files
SRC = mergesort_para.cpp

# Default target
all: $(TARGET)

# Build rule
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Clean rule
clean:
	rm -f $(TARGET)
