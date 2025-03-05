# Compiler
CXX := g++
CXXFLAGS := -g -Wall -Iinclude/
LDFLAGS := -Llib -lSDL2main -lSDL2 -lSDL2_image

# Source files and build paths
SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

# Source files and object files
SRCS := main.cpp $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir $(SRCS)))  # Strip path and store in obj/

TARGET := $(BIN_DIR)/game.exe

# Default target
all: $(TARGET)

# Linking
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(OBJS) $(LDFLAGS) -o $(TARGET)

# Compilation rule
$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/main.o: main.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ensure obj and bin directories exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Clean build files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)