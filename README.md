# Project: Hotline Miami-Style SDL2 Game

## Overview

This project is a 2D top-down shooter inspired by Hotline Miami. Built in C++ with the SDL2 library, the game now features a modular, well-organized design with dynamic resource management, refined collision detection, and a robust weapon system. In addition to a player character with advanced weapon handling (pickup, drop, and throw mechanics), the game now includes multiple enemy types (including a boss enemy), each with their own AI and weapon behavior.

## Folder Structure

- **src/** – All source (`.cpp`) and header (`.h`) files.
- **assets/** – Game assets (sprites, maps, weapons, sounds).
- **build/** – Compiled binaries and object files.
- **docs/** – Project documentation.
- **Makefile** – Build instructions for compiling the project.

## Dependencies

- **SDL2** – Main multimedia library.
- **SDL2_image** – For handling PNG images.
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

## Design & Implementation

### 1. Modular Code Architecture

- **Separation of Concerns:**  
  The code is organized into separate classes:
  - **Game:** The central game loop, state management, and high-level object updates.
  - **Renderer:** Encapsulates SDL_Renderer functions.
  - **InputManager:** Handles all player input.
  - **Level:** Manages a tile-based level loaded from JSON, supporting collision layers.
  - **Menu:** Implements the main menu system.
  - **Player:** Manages the player’s movement, animations, and weapon handling.
  - **Enemy & BossEnemy:** Enemy AI (with a dedicated BossEnemy subclass that uses boss-specific textures).
  - **Weapon System:** An abstract weapon interface (`AbstractWeapon`) with derived classes for projectile weapons and melee weapons. Each weapon type handles its own animations, fire rate, and damage logic.
  - **Bullet:** Handles bullet behavior and collision detection.

- **Resource Management:**  
  A `ResourceManager` caches and provides texture resources to prevent redundant loading and ensure proper cleanup.

- **Smart Pointers:**  
  Usage of `std::unique_ptr` throughout the codebase ensures safe memory management and cleanup of game objects.

### 2. Player & Weapon System

- **Player Movement & Animation:**  
  The player can move using WASD and aim using the mouse. Depending on whether a weapon is held, the player’s animation switches between normal and “attached” modes (e.g., `player_run.png` vs. `player_run_attached.png`).

- **Weapon Handling:**  
  The weapon system distinguishes between:
  - **ProjectileWeapon:** For guns (pistol, shotgun, SMG, etc.) that spawn bullets with defined speed, fire rate, and ammo limits.
  - **MeleeWeapon:** For close-combat weapons (baseball bat, knife) that trigger an attack animation and apply damage on collision.
  - **Barefist:** If the player is unarmed, they can still punch to damage enemies.

- **Weapon Pickup/Drop/Throw Mechanics:**  
  - **Right-Click:**  
    - If the player holds a weapon, right‑click will drop (throw) it at a calculated position.
    - If the player is unarmed, right‑click near a dropped weapon will pick it up.
  - Dropped weapons are rendered using their own (often smaller) images and remain on the ground until picked up.
  - The player's weapon system only allows one weapon at a time.

### 3. Enemy & Boss Systems

- **Standard Enemies:**  
  Enemies patrol the level and, when the player is within a certain range, switch to an attacking state. They fire bullets from their center using their assigned weapon. Upon death, they play a death animation and drop their weapon for pickup.

- **BossEnemy:**  
  A subclass of `Enemy` that overrides the textures with boss-specific images (`boss_idle.png`, `boss_run.png`, `boss_dead.png`). The boss behaves similarly to regular enemies but is visually distinct and may have enhanced parameters.

- **Bullet and Collision:**  
  Bullets (both from the player and enemies) update using a fixed timestep and check for collisions with walls and targets. Enemy bullets now properly collide with walls and are deactivated, preventing them from flying through obstacles.

### 4. Collision, Health, and Game Restart

- **Collision Detection:**  
  Collision is consistently handled via SDL_Rects with a dedicated `CollisionHandler` class.

- **Health & Death Animations:**  
  Both the player and enemies have health values. When health drops to zero, a death animation plays and the entity is considered dead. The player’s death animation remains on screen until the player presses “R” to restart the game.

- **Game Restart:**  
  Pressing the "R" key when the player is dead reinitializes all game objects, clears old enemy and bullet containers, and reloads resources, ensuring a clean state.

### 5. Game Loop & Rendering

- **Main Menu & Game States:**  
  The game starts in a main menu (handled by the `Menu` class) and transitions into the game loop upon starting. The game loop handles events, updates game objects (player, enemies, bullets), and renders the scene.

- **Camera System:**  
  A camera follows the player, converting world coordinates to on-screen coordinates for rendering.

- **Rendering Order:**  
  The level is rendered first, followed by enemies, then the player. Dropped weapons and bullets are rendered on top so that they are visible and interactable.

### 6. Input Handling

- **Player Controls:**  
  - **WASD:** Move the player.
  - **Mouse:** Aim.
  - **Left Click:** Shoot or attack.
  - **Right Click:** Pick up or drop/throw a weapon (pickup/drop only occurs on right‑click, not automatically).
  - **R Key:** Restart the game when the player is dead.

---

## Future Improvements

- **Advanced Enemy AI:**  
  Implement more sophisticated enemy behaviors and pathfinding.
- **Expanded Weapon System:**  
  Introduce additional weapon types with unique properties.
- **Sound & Effects:**  
  Add sound effects, background music, and particle effects.
- **Level Optimization:**  
  Optimize rendering by drawing only visible tiles.
- **UI/UX Enhancements:**  
  Improve on-screen indicators for pickups and weapon status.

---

## Controls Recap

- **WASD:** Move the player.
- **Mouse:** Aim.
- **Left Click:** Shoot/attack.
- **Right Click:** Pick up or drop/throw a weapon.
- **R:** Restart the game when dead.

---

## Conclusion

This project demonstrates a modular and scalable design for a 2D SDL2 game. With features such as dynamic weapon handling, enemy AI (including boss enemies), collision detection, and a robust resource manager, the codebase is structured for easy debugging and future expansion. The design emphasizes separation of concerns and clean resource management, allowing for rapid iteration on gameplay features.

---

## Author

This project is developed by NPCoders.
