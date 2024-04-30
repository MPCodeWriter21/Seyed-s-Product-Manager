# Compiler
CC          = clang
CXX         = clang++
DEBUG_FLAGS = --debug

# Directories
SRC_DIR         = src
BUILD_DIR       = build
DEBUG_BUILD_DIR = build/debug
SQLITE_DIR      = $(SRC_DIR)/sqlite
UTILS_DIR       = $(SRC_DIR)/utils

# Source files
SRC_FILES        = $(wildcard $(SRC_DIR)/*.cpp)
SQLITE_SRC_FILES = $(wildcard $(SQLITE_DIR)/*.c)
UTILS_SRC_FILES  = $(wildcard $(UTILS_DIR)/*.cpp)

# Object files
OBJ_FILES        = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES))
SQLITE_OBJ_FILES = $(patsubst $(SQLITE_DIR)/%.c,$(BUILD_DIR)/%.o,$(SQLITE_SRC_FILES))
UTILS_OBJ_FILES  = $(patsubst $(UTILS_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(UTILS_SRC_FILES))

DEBUG_OBJ_FILES        = $(patsubst $(SRC_DIR)/%.cpp,$(DEBUG_BUILD_DIR)/%.o,$(SRC_FILES))
DEBUG_SQLITE_OBJ_FILES = $(patsubst $(SQLITE_DIR)/%.c,$(DEBUG_BUILD_DIR)/%.o,$(SQLITE_SRC_FILES))
DEBUG_UTILS_OBJ_FILES  = $(patsubst $(UTILS_DIR)/%.cpp,$(DEBUG_BUILD_DIR)/%.o,$(UTILS_SRC_FILES))

# Executable
OUT = seyed_app.out
DEBUG_OUT = $(DEBUG_BUILD_DIR)/$(OUT)

# Flags
CFLAGS = -I$(SRC_DIR) -I$(SQLITE_DIR) -I$(UTILS_DIR)
CXXFLAGS = -I$(SRC_DIR) -I$(SQLITE_DIR) -I$(UTILS_DIR) -Wall -Wextra --std=c++20
LDFLAGS  = 

.PHONY: all clean debug

all: $(BUILD_DIR) $(OUT)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(DEBUG_BUILD_DIR):
	mkdir -p $(DEBUG_BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SQLITE_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(UTILS_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OUT): $(OBJ_FILES) $(SQLITE_OBJ_FILES) $(UTILS_OBJ_FILES)
	$(CXX) $^ -o $(BUILD_DIR)/$@ $(LDFLAGS)

debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(DEBUG_BUILD_DIR) $(DEBUG_OUT)

$(DEBUG_BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(DEBUG_BUILD_DIR)/%.o: $(SQLITE_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(DEBUG_BUILD_DIR)/%.o: $(UTILS_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(DEBUG_OUT): $(DEBUG_OBJ_FILES) $(DEBUG_SQLITE_OBJ_FILES) $(DEBUG_UTILS_OBJ_FILES)
	$(CXX) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(DEBUG_BUILD_DIR)
	rm -rf $(BUILD_DIR)

