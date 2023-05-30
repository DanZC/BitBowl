ifndef WASI_SDK_PATH
$(error Download the WASI SDK (https://github.com/WebAssembly/wasi-sdk) and set $$WASI_SDK_PATH)
endif

CC = "$(WASI_SDK_PATH)/bin/clang" --sysroot="$(WASI_SDK_PATH)/share/wasi-sysroot"
CXX = "$(WASI_SDK_PATH)/bin/clang++" --sysroot="$(WASI_SDK_PATH)/share/wasi-sysroot"

# Optional dependency from binaryen for smaller builds
WASM_OPT = wasm-opt
WASM_OPT_FLAGS = -Oz --zero-filled-memory --strip-producers

PNG2SRC = w4 png2src --c
BIN2SRC = python tools/bin2src.py
AREALD  = python tools/areald.py

# Whether to build for debugging instead of release
DEBUG = 0

# Compilation flags
CFLAGS = -W -Wall -Wextra -Werror -Wno-unused -Wconversion -Wsign-conversion -MMD -MP -fno-exceptions
ifeq ($(DEBUG), 1)
	CFLAGS += -DDEBUG -O0 -g
else
	CFLAGS += -DNDEBUG -Oz -flto
endif

PCGCFLAGS = -W -Wall -Wextra -Wno-unused -Wconversion -Wsign-conversion -MMD -MP -fno-exceptions -DNDEBUG -Oz -flto

# Linker flags
LDFLAGS = -Wl,-zstack-size=14752,--no-entry,--import-memory -mexec-model=reactor \
	-Wl,--initial-memory=65536,--max-memory=65536,--stack-first \
	-lm -Wl,-Map,$@.map
ifeq ($(DEBUG), 1)
	LDFLAGS += -Wl,--export-all,--no-gc-sections
else
	LDFLAGS += -Wl,--strip-all,--gc-sections,--lto-O3 -Oz
endif

METAS = $(wildcard data/metatilesets/*.bin)

OBJECTS = $(patsubst src/%.c, build/%.o, $(wildcard src/*.c))
OBJECTS += $(patsubst src/%.cpp, build/%.o, $(wildcard src/*.cpp))
OBJECTS += $(patsubst src/pcg/%.c, build/pcg/%.o, $(wildcard src/pcg/*.c))
OBJECTS += $(patsubst src/data/areas/%.cpp, build/data/areas/%.o, $(wildcard src/data/areas/*.cpp))
OBJECTS += $(patsubst src/data/metatilesets/%.cpp, build/data/metatilesets/%.o, $(wildcard src/data/metatilesets/*.cpp))
OBJECTS += build/data/graphics.o
DEPS = $(OBJECTS:.o=.d)
PNGS = $(wildcard assets/images/*.png)

ifeq '$(findstring ;,$(PATH))' ';'
    DETECTED_OS := Windows
else
    DETECTED_OS := $(shell uname 2>/dev/null || echo Unknown)
    DETECTED_OS := $(patsubst CYGWIN%,Cygwin,$(DETECTED_OS))
    DETECTED_OS := $(patsubst MSYS%,MSYS,$(DETECTED_OS))
    DETECTED_OS := $(patsubst MINGW%,MSYS,$(DETECTED_OS))
endif

ifeq ($(DETECTED_OS), Windows)
	MKDIR_BUILD = if not exist build md build
	RMDIR = rd /s /q
else
	MKDIR_BUILD = mkdir -p build
	RMDIR = rm -rf
endif

all: build/cart.wasm

# Link cart.wasm from all object files and run wasm-opt
build/cart.wasm: assets $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS)
ifneq ($(DEBUG), 1)
ifeq (, $(shell command -v $(WASM_OPT)))
	@echo Tip: $(WASM_OPT) was not found. Install it from binaryen for smaller builds!
else
	$(WASM_OPT) $(WASM_OPT_FLAGS) $@ -o $@
endif
endif

windows: build/cart.wasm
	w4 bundle build/cart.wasm --title "Bit Bowl" --windows build/cart-windows.exe

linux: build/cart.wasm
	w4 bundle build/cart.wasm --title "Bit Bowl" --linux build/cart-linux

mac: build/cart.wasm
	w4 bundle build/cart.wasm --title "Bit Bowl" --mac build/cart-mac

html: build/cart.wasm
	w4 bundle build/cart.wasm --title "Bit Bowl" --html build/html/index.html

dist: windows linux mac html

# Compile C sources
build/%.o: src/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Compile C++ sources
build/%.o: src/%.cpp
	$(CXX) -c $< -o $@ --std=c++17 $(CFLAGS)

build/pcg/%.o: src/pcg/%.c
	$(CC) -c $< -o $@ $(PCGCFLAGS)

src/data/graphics.cpp: src/data/graphics.hpp $(PNGS)
	$(PNG2SRC) -o $@ --template templates/graphics_src.mustache $(PNGS)

src/data/graphics.hpp: $(PNGS)
	$(PNG2SRC) -o $@ --template templates/graphics_header.mustache $(PNGS)

# Compile areas
build/data/areas/%.o: src/data/areas/%.cpp
	$(CXX) -c $< -o $@ --std=c++17 $(CFLAGS)

# Compile C++ sources
build/data/metatilesets/%.o: src/data/metatilesets/%.cpp
	$(CXX) -c $< -o $@ --std=c++17 $(CFLAGS)

assets: src/data/graphics.cpp

.PHONY: clean
clean:
	$(RMDIR) build/*.o
	$(RMDIR) build/*.d 
	$(RMDIR) build/pcg/*.o
	$(RMDIR) build/pcg/*.d
	$(RMDIR) build/cart.wasm
	$(RMDIR) build/cart.wasm.map

clean_dist:
	$(RMDIR) build/cart-windows.exe
	$(RMDIR) build/cart-linux
	$(RMDIR) build/html/index.html

-include $(DEPS)
