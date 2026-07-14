# Define the compiler for Linux
CXX = g++

# Compiler flags
# -O3: Optimize for speed
# -pthread: Support for multi-threading
# -static: Bundle all libraries so it runs without dependencies
CXXFLAGS = -O3 -pthread -static

# The name of the output binary
TARGET = lib

# The source file
SRC = ddos_file.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
