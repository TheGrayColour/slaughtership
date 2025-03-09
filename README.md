# Project: Hotline Miami-Style SDL2 Game

## Overview

This project is a 2D top-down shooter inspired by Hotline Miami, built using C/C++ and the SDL2 library. It features a player-controlled character, a tile-based level system, and a main menu interface.

## Folder Structure

- `src/` - Contains all source files (`.cpp`).
- `assets/` - Stores game assets such as sprites, maps, and sounds.
- `include/` - Stores used SDL2 libraries, json and the src header files.
- `lib/` - Stores dlls used to compile.
- `obj/` - Compiled object files.
- `bin/` - Compiled .exe file.
- `main.cpp` - The main game script.
- `Makefile` - For compiling the project using `make`.

## Dependencies

- **SDL2** (Simple DirectMedia Layer)
- **SDL2_image** (For handling PNG images)
- **SDL2_ttf** (For rendering text, if needed)
- **C++17 or later**

## Compilation & Execution

### Linux/macOS

```sh
make
./game
```

### Windows (MinGW)

```sh
make
./game.exe
```

## Housekeeping Guidelines

### Code Organization

- **Header files (`.h`)** contain class declarations.
- **Source files (`.cpp`)** contain the implementations.
- **Global variables** should be minimized; encapsulation should be used instead.
- **Memory management** is done using `new` and `delete`. Avoid leaks by cleaning up allocated memory in destructors.

### Game Flow

1. The game starts in the **Main Menu** (`Menu` class).
2. The player can start the game, which transitions to the **Game Loop** (`Game` class).
3. The game features **tile-based levels**, loaded from a JSON file (`Level` class).
4. The player can **move, aim, and shoot**, with the camera following the player (`Player` class).
5. The game loop runs until the player quits, returning to the main menu or exiting entirely.

### Memory Management & Cleanup

- `Game::clean()` ensures proper deletion of dynamically allocated objects (`menu`, `player`, `level`).
- **Always free resources** (textures, fonts, maps) before shutting down SDL.

### Key Features Implemented

- **Menu System** (handled by `Menu` class)
- **Player Controls** (movement, aiming, shooting)
- **Camera System** (follows the player)
- **Tile-Based Level System** (using JSON and Tiled)
- **Smooth Animation** (idle and running sprites)

### Future Improvements

- Implement enemy AI.
- Add more levels and weapons.
- Improve UI/UX for better gameplay experience.
- Optimize performance and memory usage.

## Controls

- **WASD** - Move player
- **Mouse** - Aim
- **Left Click** - Shoot

## Author

This project is being developed by NPCoders.
