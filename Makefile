# Compiler
CC          = clang
CXX         = clang++
DEBUG_FLAGS = --debug
ARCH       ?=
BUILD_GUI  ?= yes

# Directories
SRC_DIR          = src
BUILD_DIR        = build
DEBUG_BUILD_DIR  = build/debug
SQLITE_DIR       = $(SRC_DIR)/sqlite
UTILS_DIR        = $(SRC_DIR)/utils
DATABASE_SRC_DIR = $(SRC_DIR)/database
GUI_DIR          = $(SRC_DIR)/gui
IMGUI_DIR        = $(SRC_DIR)/gui/imgui
MISC_DIR         = misc

# Source files
SRC_FILES          = $(wildcard $(SRC_DIR)/*.cpp)
SQLITE_SRC_FILES   = $(wildcard $(SQLITE_DIR)/*.c)
UTILS_SRC_FILES    = $(wildcard $(UTILS_DIR)/*.cpp)
DATABASE_SRC_FILES = $(wildcard $(DATABASE_SRC_DIR)/*.cpp)
GUI_SRC_FILES      = $(wildcard $(GUI_DIR)/*.cpp)
IMGUI_SRC_FILES    = $(wildcard $(IMGUI_DIR)/*.cpp)
MISC_FILES         = $(wildcard $(MISC_DIR)/*.*)

# Object files
OBJ_FILES          = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES))
SQLITE_OBJ_FILES   = $(patsubst $(SQLITE_DIR)/%.c,$(BUILD_DIR)/%.o,$(SQLITE_SRC_FILES))
UTILS_OBJ_FILES    = $(patsubst $(UTILS_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(UTILS_SRC_FILES))
DATABASE_OBJ_FILES = $(patsubst $(DATABASE_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(DATABASE_SRC_FILES))
GUI_OBJ_FILES      = $(patsubst $(GUI_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(GUI_SRC_FILES))
IMGUI_OBJ_FILES    = $(patsubst $(IMGUI_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(IMGUI_SRC_FILES))
BUILD_MISC_FILES   = $(patsubst $(MISC_DIR)/%,$(BUILD_DIR)/$(MISC_DIR)/%,$(MISC_FILES))

DEBUG_OBJ_FILES          = $(patsubst $(SRC_DIR)/%.cpp,$(DEBUG_BUILD_DIR)/%.o,$(SRC_FILES))
DEBUG_SQLITE_OBJ_FILES   = $(patsubst $(SQLITE_DIR)/%.c,$(DEBUG_BUILD_DIR)/%.o,$(SQLITE_SRC_FILES))
DEBUG_UTILS_OBJ_FILES    = $(patsubst $(UTILS_DIR)/%.cpp,$(DEBUG_BUILD_DIR)/%.o,$(UTILS_SRC_FILES))
DEBUG_DATABASE_OBJ_FILES = $(patsubst $(DATABASE_DIR)/%.cpp,$(DEBUG_BUILD_DIR)/%.o,$(DATABASE_SRC_FILES))
DEBUG_GUI_OBJ_FILES      = $(patsubst $(GUI_DIR)/%.cpp,$(DEBUG_BUILD_DIR)/%.o,$(GUI_SRC_FILES))
DEBUG_IMGUI_OBJ_FILES    = $(patsubst $(IMGUI_DIR)/%.cpp,$(DEBUG_BUILD_DIR)/%.o,$(IMGUI_SRC_FILES))
DEBUG_BUILD_MISC_FILES   = $(patsubst $(MISC_DIR)/%,$(DEBUG_BUILD_DIR)/$(MISC_DIR)/%,$(MISC_FILES))

# Executable
OUT = seyed_app.out
DEBUG_OUT = $(DEBUG_BUILD_DIR)/$(OUT)

# Flags
CFLAGS   = -I$(SRC_DIR)
CXXFLAGS = -I$(SRC_DIR) -Wall -Wextra --std=c++20
LDFLAGS  =

ifeq ($(BUILD_GUI),yes)
	LDFLAGS  := -luser32 -ld3d9
	CXXFLAGS := $(CXXFLAGS) -D GUI
endif

ifeq ($(ARCH), "")
	CXXFLAGS := $(CXXFLAGS)
else
	CXXFLAGS := $(CXXFLAGS) -march=$(ARCH)
endif

.PHONY: all clean debug

all: $(BUILD_DIR) $(BUILD_MISC_FILES) $(OUT) #gui.o main

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

$(BUILD_DIR)/%.o: $(DATABASE_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(GUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

$(BUILD_MISC_FILES): $(MISC_FILES)
	cp $(MISC_DIR) $(BUILD_DIR)/$(MISC_DIR) -r

ifeq ($(BUILD_GUI),yes)
$(OUT): $(OBJ_FILES) $(SQLITE_OBJ_FILES) $(UTILS_OBJ_FILES) $(DATABASE_OBJ_FILES) $(IMGUI_OBJ_FILES) $(GUI_OBJ_FILES)
	$(CXX) $(CXXFLAGS) $^ -o $(BUILD_DIR)/$@ $(LDFLAGS)
else
$(OUT): $(OBJ_FILES) $(SQLITE_OBJ_FILES) $(UTILS_OBJ_FILES) $(DATABASE_OBJ_FILES)
	$(CXX) $(CXXFLAGS) $^ -o $(BUILD_DIR)/$@ $(LDFLAGS)
endif

debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(DEBUG_BUILD_DIR) $(DEBUG_BUILD_MISC_FILES) $(DEBUG_OUT)

$(DEBUG_BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(DEBUG_BUILD_DIR)/%.o: $(SQLITE_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(DEBUG_BUILD_DIR)/%.o: $(UTILS_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(DEBUG_BUILD_DIR)/%.o: $(DATABASE_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(DEBUG_BUILD_DIR)/%.o: $(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

$(DEBUG_BUILD_DIR)/%.o: $(GUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(DEBUG_BUILD_MISC_FILES): $(MISC_FILES)
	cp $(MISC_DIR) $(DEBUG_BUILD_DIR)/$(MISC_DIR) -r

ifeq ($(BUILD_GUI),yes)
$(DEBUG_OUT): $(DEBUG_OBJ_FILES) $(DEBUG_SQLITE_OBJ_FILES) $(DEBUG_UTILS_OBJ_FILES) $(DEBUG_DATABASE_OBJ_FILES) $(DEBUG_IMGUI_OBJ_FILES) $(DEBUG_GUI_OBJ_FILES)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
else
$(DEBUG_OUT): $(DEBUG_OBJ_FILES) $(DEBUG_SQLITE_OBJ_FILES) $(DEBUG_UTILS_OBJ_FILES) $(DEBUG_DATABASE_OBJ_FILES)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
endif

clean:
	rm -rf $(DEBUG_BUILD_DIR)
	rm -rf $(BUILD_DIR)

