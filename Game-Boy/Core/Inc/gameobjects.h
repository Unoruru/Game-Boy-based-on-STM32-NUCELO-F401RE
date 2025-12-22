/*
 * gameobjects.h
 *
 *  Created on: Nov 28, 2025
 *      Author: zjzac
 */

#ifndef GAME_OBJECTS_H
#define GAME_OBJECTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "ssd1306.h"
#include "joystick.h"

// --- Game Constants ---
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64

// Dimensions
#define OBJ_SIZE      6    // Size of falling objects
#define BASKET_W      12   // Width of player
#define BASKET_H      4    // Height of player

// --- Struct Definitions ---

// The Player (Basket)
typedef struct {
    int x;
    int y;
    int width;
    int height;
    int speed;
} Basket_t;

// The Falling Item (Fruit or Bomb)
typedef struct {
    int x;
    int y;
    int type;   // 0 = Fruit (Good), 2 = Bomb (Bad)
    int active; // 0 = Inactive, 1 = Active
    int speed;
} FallingObject_t;

// --- Function Prototypes ---

// Initialization
void Game_Init(Basket_t* p, FallingObject_t* o);
void Object_Spawn(FallingObject_t* o);

// Physics Updates
void Basket_Update(Basket_t* p, JoystickData_t input);
void Object_Update(FallingObject_t* o);

// Collision (1=Hit, 0=Miss)
int Check_Collision(Basket_t* p, FallingObject_t* o);

// Rendering
void Game_Draw(Basket_t* p, FallingObject_t* o);

#ifdef __cplusplus
}
#endif

#endif /* INC_GAME_OBJECTS_H */
