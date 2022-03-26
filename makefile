# linux | windows
TARGET_OS ?= linux
# 32 | 64
TARGET_CPU ?= 64
# debug | release
CONFIG ?= debug

SRC_DIR ?= src
BUILD_DIR ?= bin/$(TARGET_OS)
TARGET_NAME ?= libtootlwren

GODOT_HEADER_PATH=./godot-cpp/godot-headers/
CPP_BINDING_PATH=./godot-cpp/

CPPPATH=-Isrc/ -I./src/wren/vm -I./src/wren/optional -I$(GODOT_HEADER_PATH) -I$(CPP_BINDING_PATH)include/ -I$(CPP_BINDING_PATH)include/core/ -I$(CPP_BINDING_PATH)include/gen/
LIBPATH=$(CPP_BINDING_PATH)bin/
GODOTLIB=godot-cpp.$(TARGET_OS).$(CONFIG).$(TARGET_CPU)

CXX:=g++
CXXFLAGS:=-std=c++17 -fPIC $(CPPPATH)


ifeq ($(TARGET_OS), windows)
CXX:=x86_64-w64-mingw32-g++
TARGET_NAME:=$(TARGET_NAME).dll
endif
ifeq ($(TARGET_OS), linux)
TARGET_NAME:=$(TARGET_NAME).so
endif

ifeq ($(CONFIG), debug)
CXXFLAGS:=-g $(CXXFLAGS)
endif
ifeq ($(CONFIG), release)
CXXFLAGS:=-O3 $(CXXFLAGS)
endif

CPPSRCS := $(shell find $(SRC_DIR) -name *.cpp) 
CSRCS := $(shell find $(SRC_DIR) -name *.c)
SRCS := $(CPPSRCS) $(CSRCS)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/$(TARGET_NAME): $(OBJS)
	$(MKDIR_P) $(dir $@)
	$(CXX) -o $@ -shared $(OBJS) -L$(LIBPATH) -l$(GODOTLIB)

$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	$(RM) -r $(BUILD_DIR)
MKDIR_P ?= mkdir -p
