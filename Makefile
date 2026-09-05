CXX ?= g++
WIN_CXX ?= i686-w64-mingw32-g++

SDK_DIR ?= 
BUILD_DIR := build
DIST_DIR := dist

COMMON_SOURCES := src/metrics_store.cpp src/metrics_server.cpp src/plugin.cpp
SDK_SOURCE := $(SDK_DIR)/amxplugin.cpp

CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -fPIC -DLINUX -I$(SDK_DIR) -I$(SDK_DIR)/amx -fvisibility=hidden
LDFLAGS := -shared -pthread

WIN_CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -DWIN32 -DHAVE_STDINT_H -include stddef.h -I$(SDK_DIR) -I$(SDK_DIR)/amx
WIN_LDFLAGS := -shared -static -lws2_32

.PHONY: all linux windows clean package

all: linux

linux: $(DIST_DIR)/pawn_metrics.so

windows: $(DIST_DIR)/pawn_metrics.dll

$(DIST_DIR):
	mkdir -p $(DIST_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(DIST_DIR)/pawn_metrics.so: $(COMMON_SOURCES) $(SDK_SOURCE) | $(DIST_DIR)
	$(CXX) $(CXXFLAGS) $(COMMON_SOURCES) $(SDK_SOURCE) $(LDFLAGS) -o $@

$(DIST_DIR)/pawn_metrics.dll: $(COMMON_SOURCES) $(SDK_SOURCE) | $(DIST_DIR)
	$(WIN_CXX) $(WIN_CXXFLAGS) $(COMMON_SOURCES) $(SDK_SOURCE) $(WIN_LDFLAGS) -o $@

package: linux
	cp include/pawn_metrics.inc $(DIST_DIR)/
	cp examples/example.pwn $(DIST_DIR)/

clean:
	rm -rf $(BUILD_DIR) $(DIST_DIR)
