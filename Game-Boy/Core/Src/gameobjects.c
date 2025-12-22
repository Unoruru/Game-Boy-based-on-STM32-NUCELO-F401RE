/*
 * gameobjects.c
 *
 *  Created on: Nov 28, 2025
 *      Author: zjzac
 */

#include "gameobjects.h"
#include <stdlib.h> // For rand()

// Private Helper: Draw Filled Rectangle
static void DrawRect(int x, int y, int w, int h, SSD1306_COLOR color) {
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            ssd1306_DrawPixel(x + i, y + j, color);
        }
    }
}

// --- Logic ---

void Object_Spawn(FallingObject_t* o) {
    o->x = rand() % (SCREEN_WIDTH - OBJ_SIZE);
    o->y = -OBJ_SIZE; // Start above screen
    o->active = 1;
    o->speed = 1 + (rand() % 3); // Random speed 1-3

    // 70% Fruit (0), 30% Bomb (2)
    if ((rand() % 10) < 7) o->type = 0;
    else o->type = 2;
}

void Game_Init(Basket_t* p, FallingObject_t* o) {
    p->width = BASKET_W;
    p->height = BASKET_H;
    p->x = (SCREEN_WIDTH / 2) - (BASKET_W / 2);
    p->y = SCREEN_HEIGHT - (BASKET_H + 2);
    p->speed = 3;

    Object_Spawn(o);
}

void Basket_Update(Basket_t* p, JoystickData_t input) {
    // 1. Move Left
    if (input.x < 1000) {
        p->x -= p->speed;
    }
    // 2. Move Right
    else if (input.x > 3000) {
        p->x += p->speed;
    }

    // 3. Keep inside screen
    if (p->x < 0) p->x = 0;
    if (p->x > SCREEN_WIDTH - p->width) p->x = SCREEN_WIDTH - p->width;
}

void Object_Update(FallingObject_t* o) {
    if (o->active == 0) return;

    o->y += o->speed;

    // Check floor
    if (o->y > SCREEN_HEIGHT) {
        o->active = 0;
    }
}

int Check_Collision(Basket_t* p, FallingObject_t* o) {
    if (o->active == 0) return 0;

    // AABB Collision
    if (o->x < p->x + p->width &&
        o->x + OBJ_SIZE > p->x &&
        o->y < p->y + p->height &&
        o->y + OBJ_SIZE > p->y) {
        return 1;
    }
    return 0;
}

void Game_Draw(Basket_t* p, FallingObject_t* o) {
    // Draw Basket
    DrawRect(p->x, p->y, p->width, p->height, White);

    // Draw Object
    if (o->active) {
        if (o->type == 0) {
            // Fruit: Box
            DrawRect(o->x, o->y, OBJ_SIZE, OBJ_SIZE, White);
        } else {
            // Bomb: X Shape
            for (int k = 0; k < OBJ_SIZE; k++) {
                ssd1306_DrawPixel(o->x + k, o->y + k, White);
                ssd1306_DrawPixel(o->x + OBJ_SIZE - 1 - k, o->y + k, White);
            }
        }
    }
}
