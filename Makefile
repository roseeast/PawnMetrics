CXX ?= g++
WIN_CXX ?= i686-w64-mingw32-g++
LLVM_MINGW_DEFAULT := /home/rch/Documents/realterproject/tools/llvm-mingw-20251104-ucrt-ubuntu-22.04-x86_64/bin/i686-w64-mingw32-g++
ifeq ($(shell which $(WIN_CXX) 2>/dev/null),)
  ifneq ($(wildcard $(LLVM_MINGW_DEFAULT)),)
    WIN_CXX := $(LLVM_MINGW_DEFAULT)
  endif
endif

SDK_DIR ?= /home/rch/Documents/project-main/build-linux-x64/_deps/samp-plugin-sdk-src
OPENMP_SDK_DIR ?= /home/rch/Documents/open.mp-sdk
BUILD_DIR := build
DIST_DIR := dist

COMMON_SOURCES := src/metrics_store.cpp src/metrics_server.cpp src/natives.cpp
PLUGIN_SOURCES := $(COMMON_SOURCES) src/plugin.cpp
COMPONENT_SOURCES := $(COMMON_SOURCES) components/openmp/component.cpp
SDK_SOURCE := $(SDK_DIR)/amxplugin.cpp

DEF_FILE := exports/pawn_metrics.def
COMPONENT_DEF_FILE := exports/pawn_metrics_component.def

CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -fPIC -DLINUX -DHAVE_STDINT_H -I$(SDK_DIR) -I$(SDK_DIR)/amx -Isrc -fvisibility=hidden
LDFLAGS := -shared -pthread

OPENMP_INCLUDES := -Icomponents/openmp/compat \
	-I$(OPENMP_SDK_DIR)/include \
	-I$(OPENMP_SDK_DIR)/lib/glm \
	-I$(OPENMP_SDK_DIR)/lib/robin-hood-hashing/src/include \
	-I$(OPENMP_SDK_DIR)/lib/span-lite/include \
	-I$(OPENMP_SDK_DIR)/lib/string-view-lite/include

WIN_CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -DWIN32 -DHAVE_STDINT_H -include stddef.h -I$(SDK_DIR) -I$(SDK_DIR)/amx -Isrc
WIN_LDFLAGS := -shared -static -lws2_32

.PHONY: all linux windows clean package openmp openmp-linux openmp-windows plugin-linux plugin-windows

all: linux

linux: plugin-linux openmp-linux

windows: plugin-windows openmp-windows

plugin-linux: $(DIST_DIR)/plugins/pawn_metrics.so $(DIST_DIR)/pawn_metrics.so

plugin-windows: $(DIST_DIR)/plugins/pawn_metrics.dll $(DIST_DIR)/pawn_metrics.dll

openmp-linux: $(DIST_DIR)/components/pawn_metrics.so

openmp-windows: $(DIST_DIR)/components/pawn_metrics.dll

$(DIST_DIR):
	mkdir -p $(DIST_DIR)/plugins $(DIST_DIR)/components

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Legacy plugin Linux
$(DIST_DIR)/plugins/pawn_metrics.so: $(PLUGIN_SOURCES) $(SDK_SOURCE) | $(DIST_DIR)
	$(CXX) $(CXXFLAGS) $(PLUGIN_SOURCES) $(SDK_SOURCE) $(LDFLAGS) -o $@

$(DIST_DIR)/pawn_metrics.so: $(DIST_DIR)/plugins/pawn_metrics.so | $(DIST_DIR)
	cp $< $@

# Legacy plugin Windows
$(DIST_DIR)/plugins/pawn_metrics.dll: $(PLUGIN_SOURCES) $(SDK_SOURCE) $(DEF_FILE) | $(DIST_DIR)
	$(WIN_CXX) $(WIN_CXXFLAGS) $(PLUGIN_SOURCES) $(SDK_SOURCE) $(DEF_FILE) $(WIN_LDFLAGS) -o $@

$(DIST_DIR)/pawn_metrics.dll: $(DIST_DIR)/plugins/pawn_metrics.dll | $(DIST_DIR)
	cp $< $@

# open.mp component Linux
$(DIST_DIR)/components/pawn_metrics.so: $(COMPONENT_SOURCES) $(SDK_SOURCE) | $(DIST_DIR)
	$(CXX) $(CXXFLAGS) $(OPENMP_INCLUDES) $(COMPONENT_SOURCES) $(SDK_SOURCE) $(LDFLAGS) -o $@

# open.mp component Windows
$(DIST_DIR)/components/pawn_metrics.dll: $(COMPONENT_SOURCES) $(SDK_SOURCE) $(COMPONENT_DEF_FILE) | $(DIST_DIR)
	$(WIN_CXX) $(WIN_CXXFLAGS) $(OPENMP_INCLUDES) $(COMPONENT_SOURCES) $(SDK_SOURCE) $(COMPONENT_DEF_FILE) $(WIN_LDFLAGS) -o $@

package: all
	cp include/pawn_metrics.inc $(DIST_DIR)/
	cp examples/example.pwn $(DIST_DIR)/
	cp examples/pawn_metrics.cfg $(DIST_DIR)/

clean:
	rm -rf $(BUILD_DIR) $(DIST_DIR)
