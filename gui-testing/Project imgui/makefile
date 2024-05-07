# Compiler

CXX             = clang++
DEBUG_FLAGS     = --debug

# Directories
VEN_DIR         = vendor/imgui
BACKENDS_DIR    = vendor/imgui/backends
BUILD_DIR       = build
DEBUG_BUILD_DIR = build/debug

# Source Files
SRC_FILES1      = $(wildcard /*.cpp)
SRC_FILES2      = $(wildcard $(VEN_DIR) /*.cpp)
SRC_FILES3      = $(wildcard $(BACKENDS_DIR) /*.cpp)

# Object Files
OBJ_FILES1      = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES1))
OBJ_FILES2      = $(patsubst $(VEN_DIR) /%.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES2))
OBJ_FILES3      = $(patsubst $(VEN_DIR) /%.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES3))

DEBUG_OBJ_FILES1= $(patsubst %.cpp,$(DEBUG_BUILD_DIR)/%.o,$(SRC_FILES1))
DEBUG_OBJ_FILES2= $(patsubst $(VEN_DIR) /%.cpp,$(DEBUG_BUILD_DIR)/%.o,$(SRC_FILES2))
DEBUG_OBJ_FILES3= $(patsubst $(VEN_DIR) /%.cpp,$(DEBUG_BUILD_DIR)/%.o,$(SRC_FILES3))


# Executable
OUT             = seyed_app.out
DEBUG_OUT       = $(DEBUG_BUILD_DIR)/$(OUT)

#Flags
CXXFLAGS        = -I$(BUILD_DIR) $(OUT) -wall -wextra --std=c++17

.PHONY : all clean debug

all : $(BUILD_DIR) $(OUT)

$(BUILD_DIR) :
	mkdir -p $(BUILD_DIR)

$(DEBUG_BUILD_DIR) :
	mkdir -p $(DEBUG_BUILD_DIR)

$(BUILD_DIR)/%.o : $(SRC_FILES1)/%.CPP
	$(CXX) -c $< -o $@

$(BUILD_DIR)/%.o : $(SRC_FILES2)/%.CPP
	$(CXX) -c $< -o $@

$(BUILD_DIR)/%.o : $(SRC_FILES3)/%.CPP
	$(CXX) -c $< -o $@

$(OUT) : $(OBJ_FILES1) $(OBJ_FILES1)
	$(CXX) $(CXXFLAGS) $^ -o $(BUILD_DIR)/$@ $(LDFLAGS)

debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(DEBUG_BUILD_DIR) $(DEBUG_OUT)


$(DEBUG_BUILD_DIR)/%.o: $(VEN_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(DEBUG_BUILD_DIR)/%.o: $(BACKENDS_DIR)/%.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

$(DEBUG_OUT): $(DEBUG_OBJ_FILES1) $(DEBUG_OBJ_FILES2) $(DEBUG_OBJ_FILES3)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(DEBUG_BUILD_DIR)
	rm -rf $(BUILD_DIR)	