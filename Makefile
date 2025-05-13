# Makefile — Debug build with console

# Compiler
CXX := g++

# Debug flags: symbols, no optimization
DEBUGFLAGS := -g -O0 -pipe

# Compile flags (include SDL2/FFmpeg headers)
CXXFLAGS := $(DEBUGFLAGS) \
            -Iinclude -I/mingw64/include \
            $(shell pkg-config --cflags sdl2 libavcodec libavformat libswscale libswresample libavutil glib-2.0)

PKG_LIBS := $(shell pkg-config --libs sdl2 SDL2_image SDL2_mixer libavcodec libavformat libswscale libswresample libavutil glib-2.0 gobject-2.0)
# strip out the -ldl that Windows doesn’t have
PKG_LIBS := $(filter-out -ldl,$(PKG_LIBS))

# Linker flags: static libs + console subsystem
LDFLAGS := \
    -L/mingw64/lib \
    $(PKG_LIBS) \
    -liconv \
    -lws2_32 -lgdi32 -luser32 -lkernel32 -lopengl32 \
    -lole32 -loleaut32 -luuid -lsetupapi -lwinmm -lbcrypt \
    -Wl,-subsystem,console

# Sources: main.cpp + everything in src/
SRC := main.cpp $(wildcard src/*.cpp)
OBJ := $(patsubst %.cpp,obj/%.o,$(SRC))

# Default target: build game.exe (incremental)
.PHONY: all
all: bin/game.exe

# Link the executable (with console)
bin/game.exe: $(OBJ)
	mkdir -p bin
	$(CXX) $(OBJ) $(LDFLAGS) -o $@

# Compile rules
obj/%.o: src/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

obj/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
.PHONY: clean
clean:
	rm -rf obj bin
