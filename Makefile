# Define directories
SRC_DIR := source
BUILD_DIR := build
INCLUDE_DIR := include

# Define source and object files
SRC_FILES := $(wildcard $(SRC_DIR)/*.c $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES)))

# Define the executable name
EXECUTABLE := wtcc

# Compiler settings
CC := gcc
CXX := g++
CWARNFLAGS := -Wall -Wextra -Werror -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wunused -Wuninitialized -Wmaybe-uninitialized -Wformat=2 -Wswitch-enum -Wmissing-prototypes -Wmissing-declarations -Wmissing-field-initializers -Wstrict-aliasing -Wduplicated-cond -Wlogical-op
CXXWARNFLAGS := -Wall -Wextra -Werror -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wunused -Wuninitialized -Wmaybe-uninitialized -Wformat=2 -Wswitch-enum -Wnon-virtual-dtor -Woverloaded-virtual -Wdeprecated-declarations
CFLAGS := -I$(INCLUDE_DIR) -O3 $(CWARNFLAGS)
CXXFLAGS := -I$(INCLUDE_DIR) -O3 $(CXXWARNFLAGS) -std=c++20
LDFLAGS := -lm

# Default target
all: $(EXECUTABLE)

# Link the object files to create the executable
$(EXECUTABLE): $(OBJ_FILES)
	$(CXX) $(OBJ_FILES) -o $@ $(LDFLAGS)

# Compile C source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile C++ source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean the build directory
clean:
	rm -rf $(BUILD_DIR) $(EXECUTABLE).*

