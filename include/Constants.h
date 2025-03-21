#ifndef CONSTANTS_H
#define CONSTANTS_H

// Screen settings
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 768;

// Player settings
const int PLAYER_SPRITE_WIDTH = 54;
const int PLAYER_SPRITE_HEIGHT = 54;
const int PLAYER_COLLISION_OFFSET_X = 19; // X offset for collision box
const int PLAYER_COLLISION_OFFSET_Y = 19; // Y offset for collision box
const int PLAYER_COLLISION_WIDTH = 16;
const int PLAYER_COLLISION_HEIGHT = 16;
const float PLAYER_SPEED = 3.0f;

// Animation settings
const int ANIMATION_FRAME_SPEED = 3;
const int IDLE_FRAMES = 1;
const int RUN_FRAMES = 8;
const int ATTACK_FRAMES = 8;

// Menu settings
const int BUTTON_WIDTH = 200;
const int BUTTON_HEIGHT = 50;
const int MENU_START_Y = 250;
const int MENU_SPACING = 100;
const int MENU_CENTER_X = (SCREEN_WIDTH - BUTTON_WIDTH) / 2;

// Level settings
const int DEFAULT_TILE_WIDTH = 32;
const int DEFAULT_TILE_HEIGHT = 32;

// Weapon settings for each type
// Pistol
const int WEAPON_AMMO_PISTOL = 10;
const float WEAPON_FIRE_RATE_PISTOL = 0.5f;
const float WEAPON_BULLET_SPEED_PISTOL = 12.0f;

// Shotgun
const int WEAPON_AMMO_SHOTGUN = 5;
const float WEAPON_FIRE_RATE_SHOTGUN = 1.0f;
const float WEAPON_BULLET_SPEED_SHOTGUN = 10.0f;

// Melee weapons (Barefist, Baseball Bat, Knife)
const int WEAPON_AMMO_MELEE = -1; // -1 indicates unlimited ammo for melee
const float WEAPON_FIRE_RATE_MELEE = 0.3f;
const float WEAPON_BULLET_SPEED_MELEE = 0.0f;

// Default (for any other weapon)
const int WEAPON_AMMO_DEFAULT = 20;
const float WEAPON_FIRE_RATE_DEFAULT = 0.2f;
const float WEAPON_BULLET_SPEED_DEFAULT = 14.0f;

#endif // CONSTANTS_H
