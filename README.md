# Project: Hotline Miami-Style SDL2 Game

## Overview

This project is a 2D top-down shooter inspired by Hotline Miami, built using C++ and the SDL2 library. It features a player-controlled character, a tile-based level system, dynamic weapon handling, a camera system, a weapon pickup system, and a main menu interface.

## Folder Structure

- `src/` - Contains all source files (`.cpp`) and header files (`.h`).
- `assets/` - Stores game assets such as sprites, maps, weapons, and sounds.
- `build/` - Compiled binary and object files.
- `docs/` - Project documentation.
- `Makefile` - For compiling the project using `make`.

## Dependencies

- **SDL2** (Simple DirectMedia Layer)
- **SDL2\_image** (For handling PNG images)
- **SDL2\_ttf** (For rendering text, if needed)
- **C++17 or later**

## Compilation & Execution

### Linux/macOS

```sh
make
./game
```

### Windows (MinGW)

```sh
mingw32-make
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
3. The game features **tile-based levels**, loaded from a JSON file (`Level` class) using multiple layers such as `floor`, `wall`, and `assets`.
4. The player can **move, aim, and shoot**, with the camera following the player (`Player` class).
5. The player can **pick up, drop, and throw weapons**, dynamically changing their attacks (`Weapon` class).
6. Each weapon has its own unique properties, including **ammo, fire rate, and attack animations**.
7. The game loop runs until the player quits, returning to the main menu or exiting entirely.

### Memory Management & Cleanup

- `Game::clean()` ensures proper deletion of dynamically allocated objects (`menu`, `player`, `level`, `weapons`).
- **Always free resources** (textures, fonts, maps) before shutting down SDL.

### Key Features Implemented

- **Menu System** (handled by `Menu` class)
- **Player Controls** (movement, aiming, shooting, melee attacks)
- **Weapon System** (pickup, drop, throw, attack with melee/guns)
  - Guns have unique ammo limits, fire rates, and bullet properties.
  - Melee weapons (bat, knife, fists) have separate attack animations.
- **Camera System** (follows the player, dynamically adjusts based on movement)
- **Tile-Based Level System** (using JSON and Tiled, supports multiple layers and non-square tiles)
- **Dynamic Animation System**
  - Player has separate animations for **idle, running, and attacking**.
  - Weapons have their own **held and dropped** sprites.
  - Guns feature a **muzzle flash effect** when fired.

### Future Improvements

- Implement enemy AI with pathfinding and attack behavior.
- Add more levels, weapons, and interactive elements.
- Improve UI/UX for better gameplay experience.
- Optimize rendering to **only draw visible tiles**.
- Implement sound effects and background music.

## Controls

- **WASD** - Move player
- **Mouse** - Aim
- **Left Click** - Shoot / Attack
- **Right Click** - Pick up / Drop / Throw weapon
- **Esc** - Open main menu

## Author

This project is being developed by NPCoders.
