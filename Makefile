SHELL := $(shell which bash)
PROJECT_VERSION := $(shell ./$(notdir ranhttp/versioning.sh) || true)
LAST_BUILD_NUMBER := $(shell command -v git &>/dev/null && (git tag -l "build-*" 2>/dev/null | sort -V | tail -n 1 | sed -E 's/^build-//; s/[^0-9]+$$//'))
CURRENT_BUILD_NUMBER := $(or $(LAST_BUILD_NUMBER), 1)
BUILD_INFO := $(shell ./$(notdir ranhttp/buildnumbering.sh) $(CURRENT_BUILD_NUMBER) || true)
SHA1SUM_TAG := $(word 1, $(BUILD_INFO))
BUILD_NUMBER := $(word 2, $(BUILD_INFO))

# Commit Args
commit_args := add m nvc note chore test refactor \
    refact doc docs document readme license sub subject \
    defix major demajor deminor depatch defeat defeatue \
    change dechange break debreak minor feat feature fix patch

args = $(foreach a,$($(subst -,_,$1)_args),$(if $(value $a),$a="$($a)"))


# Compiler and flags
CC := clang
CXX := clang++
MACROS := -DBUILD_TYPE=3 -DBUILD_NUMBER=$(BUILD_NUMBER) -DPROJ_TYPE=\"c\" -DPROJ_VERSION=\"$(PROJECT_VERSION)\"
CFLAGS := -Wall -Wextra -std=c17 -Iheaders $(MACROS)
CXXFLAGS := -Wall -Wextra -std=c++17 -Iheaders $(MACROS)
PROJ_TYPE := c
PROJ_NAME := ranhttp
BUILD_TYPE := 3

# Directories
SRC_DIR := src
OBJ_DIR := objects
BUILD_DIR := build
TEST_DIR := test
TEST_RESULT_DIR := $(TEST_DIR)/results
DEBUG_DIR := $(BUILD_DIR)/debug
RELEASE_DIR := $(BUILD_DIR)/release
BIN_DEBUG_DIR := $(DEBUG_DIR)/bin
LIB_DEBUG_DIR := $(DEBUG_DIR)/lib
BIN_RELEASE_DIR := $(RELEASE_DIR)/bin
LIB_RELEASE_DIR := $(RELEASE_DIR)/lib


SYSTEM := $(shell uname 2>/dev/null)
MINGW_MATCH := $(findstring MINGW,$(SYSTEM))
MSYS_MATCH := $(findstring MSYS,$(SYSTEM))
CYGWIN_MATCH := $(findstring CYGWIN,$(SYSTEM))

ifeq ($(findstring Linux, $(SYSTEM)), Linux)
    PLATFORM := LINUX
else ifeq ($(findstring Darwin, $(SYSTEM)), Darwin)
    PLATFORM := MACOS
else ifneq (,$(or $(MINGW_MATCH),$(MSYS_MATCH),$(CYGWIN_MATCH)))
    PLATFORM := WIN32
else
    $(error Unsupported platform detected!)
endif

STATIC_LIB_EXT := a

ifeq ($(PLATFORM), LINUX)
    DYNAMIC_LIB_EXT := so
else ifeq ($(PLATFORM), MACOS)
    DYNAMIC_LIB_EXT := dylib
else ifeq ($(PLATFORM), WIN32)
    DYNAMIC_LIB_EXT := dll
endif


ifeq ($(PROJ_TYPE), c)
    COMPILER = $(CC)
    TFLAGS = $(CFLAGS)
else
    COMPILER = $(CXX)
    TFLAGS = $(CXXFLAGS)
endif

# Find all C and C++ source files
SRC_C := $(shell find $(SRC_DIR) -name '*.c') $(wildcard ./*.c)
SRC_CPP := $(shell find $(SRC_DIR) -name '*.cpp') $(wildcard ./*.cpp)
SRC := $(SRC_C) $(SRC_CPP)

# Object files
OBJ_C := $(patsubst %.c, $(OBJ_DIR)/%.c.o, $(SRC_C))
OBJ_CPP := $(patsubst %.cpp, $(OBJ_DIR)/%.cpp.o, $(SRC_CPP))
OBJ := $(OBJ_C) $(OBJ_CPP)

# Get object files of root sources
ROOT_OBJ_C := $(patsubst %.c, $(OBJ_DIR)/%.c.o, $(wildcard ./*.c))
ROOT_OBJ_CPP := $(patsubst %.cpp, $(OBJ_DIR)/%.cpp.o, $(wildcard ./*.cpp))
ROOT_OBJ := $(ROOT_OBJ_C) $(ROOT_OBJ_CPP)

# Exclude object files from root folder
OBJ_SRC := $(filter-out $(ROOT_OBJ), $(OBJ))

# Test files
TEST_SRC_C := $(wildcard $(TEST_DIR)/*.c)
TEST_SRC_CPP := $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJ_C := $(patsubst $(TEST_DIR)/%.c, $(OBJ_DIR)/$(TEST_DIR)/%.o, $(TEST_SRC_C))
TEST_OBJ_CPP := $(patsubst $(TEST_DIR)/%.cpp, $(OBJ_DIR)/$(TEST_DIR)/%.o, $(TEST_SRC_CPP))
TEST_OBJ := $(TEST_OBJ_C) $(TEST_OBJ_CPP)

# Modules
DEBUG_MODULES := $(foreach MOD, $(notdir $(wildcard $(SRC_DIR)/**)), $(LIB_DEBUG_DIR)/lib$(MOD))
RELEASE_MODULES := $(foreach MOD, $(notdir $(wildcard $(SRC_DIR)/**)), $(LIB_RELEASE_DIR)/lib$(MOD))
LINKING_LIBS := $(foreach MOD, $(notdir $(wildcard $(SRC_DIR)/**)), -l$(MOD))

# Targets
DEBUG_TARGET := $(BIN_DEBUG_DIR)/$(PROJ_NAME)
DEBUG_DEPENDENCY := $(ROOT_OBJ) $(DEBUG_MODULES)
RELEASE_TARGET := $(BIN_RELEASE_DIR)/$(PROJ_NAME)
RELEASE_DEPENDENCY := $(ROOT_OBJ) $(RELEASE_MODULES)

ifeq ($(PLATFORM), MACOS)
    XFLAGS_SO_DEBUG := -dynamiclib
    XFLAGS_SO_RELEASE := -dynamiclib -s
else
    XFLAGS_SO_DEBUG := -shared
    XFLAGS_SO_RELEASE := -shared -s
endif

ifeq ($(BUILD_TYPE), 3)
    XFLAGS_EXEC_DEBUG := $(ROOT_OBJ) -L$(LIB_DEBUG_DIR) $(LINKING_LIBS)
    XFLAGS_EXEC_RELEASE := $(ROOT_OBJ) -L$(LIB_RELEASE_DIR) $(LINKING_LIBS)
    ifneq ($(PLATFORM), WIN32)
        XFLAGS_O := -fPIC
        XFLAGS_EXEC_DEBUG += -Wl,-rpath=$(LIB_DEBUG_DIR)
        XFLAGS_EXEC_RELEASE += -Wl,-rpath=$(LIB_RELEASE_DIR)
    else
        XFLAGS_DLL_SPEC = -Wl,--out-implib=$@.dll.a 
    endif
else ifeq ($(BUILD_TYPE), 2)
    XFLAGS_EXEC_DEBUG = $(ROOT_OBJ) $(shell find "$(LIB_DEBUG_DIR)" -name '*.$(STATIC_LIB_EXT)')
    XFLAGS_EXEC_RELEASE = $(ROOT_OBJ) $(shell find "$(LIB_RELEASE_DIR)" -name '*.$(STATIC_LIB_EXT)')
else
    XFLAGS_EXEC_DEBUG := $(DEBUG_DEPENDENCY)
    XFLAGS_EXEC_RELEASE := $(RELEASE_DEPENDENCY)
endif


# Test executables
TEST_TARGETS := $(patsubst $(OBJ_DIR)/$(TEST_DIR)/%.o, $(TEST_RESULT_DIR)/%, $(TEST_OBJ))

# Default build
all: release

# Debug build
debug: CFLAGS += -g -DDEBUG_MODE=1
debug: CXXFLAGS += -g -DDEBUG_MODE=1
debug: $(OBJ_SRC) $(DEBUG_TARGET) add_debug_build_tag

# Release build
release: CFLAGS += -O2
release: CXXFLAGS += -O2
release: $(OBJ_SRC) $(RELEASE_TARGET) add_release_build_tag

# Test executables builds
mocks: TFLAGS += -DUNIT_TESTING -DDEBUG_MODE=0
mocks: CFLAGS += -DDEBUG_MODE=0
mocks: CXXFLAGS += -DDEBUG_MODE=0
mocks: $(TEST_TARGETS)

add_debug_build_tag:
	command -v git &>/dev/null && git tag "build-$(BUILD_NUMBER)D" 2>/dev/null && git tag "$(SHA1SUM_TAG)" 2>/dev/null || true

# Link main executable
$(DEBUG_TARGET): $(DEBUG_DEPENDENCY)
	@mkdir -p $(BIN_DEBUG_DIR)
	$(COMPILER) $(TFLAGS) $(XFLAGS_EXEC_DEBUG) -o $(DEBUG_TARGET)
	@if [[ "$(PLATFORM)" == "WIN32" && "$(BUILD_TYPE)" == "3" ]]; then cp -a $(LIB_DEBUG_DIR)/*.dll $(BIN_DEBUG_DIR); fi

add_release_build_tag:
	command -v git &>/dev/null && git tag "build-$(BUILD_NUMBER)R" 2>/dev/null && git tag "$(SHA1SUM_TAG)" 2>/dev/null || true

$(RELEASE_TARGET): $(RELEASE_DEPENDENCY)
	@mkdir -p $(BIN_RELEASE_DIR)
	$(COMPILER) $(TFLAGS) -s $(XFLAGS_EXEC_RELEASE) -o $(RELEASE_TARGET)
	@if [[ "$(PLATFORM)" == "WIN32" && "$(BUILD_TYPE)" == "3" ]]; then cp -a $(LIB_RELEASE_DIR)/*.dll $(BIN_RELEASE_DIR); fi

# Create dynamic libraries
$(LIB_DEBUG_DIR)/lib%:
	@mkdir -p $(LIB_DEBUG_DIR)
	$(COMPILER) $(TFLAGS) $(XFLAGS_SO_DEBUG) $(XFLAGS_DLL_SPEC) $(shell find "$(OBJ_DIR)/$(SRC_DIR)/$*" -name '*.o') -o $@.$(DYNAMIC_LIB_EXT)

$(LIB_RELEASE_DIR)/lib%:
	@mkdir -p $(LIB_RELEASE_DIR)
	$(COMPILER) $(TFLAGS) $(XFLAGS_SO_RELEASE) $(XFLAGS_DLL_SPEC) $(shell find "$(OBJ_DIR)/$(SRC_DIR)/$*" -name '*.o') -o $@.$(DYNAMIC_LIB_EXT)


# Link test executables
$(TEST_RESULT_DIR)/%: $(OBJ_DIR)/$(TEST_DIR)/%.o $(OBJ_SRC)
	@mkdir -p $(TEST_RESULT_DIR)
	$(COMPILER) $(TFLAGS) $^ -o $@

# Compile C source files into object files
$(OBJ_DIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(XFLAGS_O) -c $< -o $@

$(OBJ_DIR)/$(SRC_DIR)/%.c.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(XFLAGS_O) -c $< -o $@

# Compile C++ source files into object files
$(OBJ_DIR)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(XFLAGS_O) -c $< -o $@

$(OBJ_DIR)/$(SRC_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(XFLAGS_O) -c $< -o $@

# Compile test C/C++ files
$(OBJ_DIR)/$(TEST_DIR)/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -DUNIT_TESTING -c $< -o $@

$(OBJ_DIR)/$(TEST_DIR)/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -DUNIT_TESTING -c $< -o $@

test:
	@echo "Running tests in $(TEST_RESULT_DIR)..."
	@TOTAL=0; PASSED=0; FAILED=0; 	for TEST_BIN in $(TEST_RESULT_DIR)/test__*; do 		if [ -x $$TEST_BIN ]; then 			TOTAL=$$((TOTAL + 1)); 			echo "Testing: $$TEST_BIN"; 			$$TEST_BIN; 			EXIT_CODE=$$?; 			if [ $$EXIT_CODE -eq 0 ]; then 				echo "✅ Test passed"; 				PASSED=$$((PASSED + 1)); 			else 				echo "❌ Test failed"; 				FAILED=$$((FAILED + 1)); 			fi; 		fi; 	done; 	echo "------------------------------------"; 	echo "Total tests   : $$TOTAL"; 	echo "Passed        : $$PASSED"; 	echo "Failed        : $$FAILED"; 	echo "------------------------------------"; 	if [ $$FAILED -ne 0 ]; then exit 1; fi

# Bump version
bump:
	command -v git &>/dev/null && git tag -a "$(PROJECT_VERSION)" -m "$(PROJ_NAME) $(PROJECT_VERSION) build $(BUILD_NUMBER)" >/dev/null 2>&1 || true

# Commit Changes
commit:
	@bash $(notdir ranhttp/commithelper.sh) $(call args,$@)

# Clean build files
clean:
	rm -rf $(OBJ_DIR) $(BUILD_DIR) $(TEST_RESULT_DIR)

print-%:
	@echo "$* = $($*)"

.PHONY: all debug release clean bump commit mocks test
