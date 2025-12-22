/*
 * joystick.h
 *
 *  Created on: Nov 23, 2025
 *      Author: zjzac
 */

#ifndef JOYSTICK_H
#define JOYSTICK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h" // For uint16_t, uint8_t types

// --- 1. DATA STRUCTURE (Restored) ---
// This is needed by freertos.c and game_objects.h to pass data around
typedef struct {
    uint16_t x;      // 0-4095
    uint16_t y;      // 0-4095
    uint8_t button;  // 0 = Pressed, 1 = Released
} JoystickData_t;

// --- 2. FUNCTION PROTOTYPES (Separate) ---

// Initialization
void Joystick_Init(void);

// Get X-Axis Value ONLY (Reads from DMA Memory)
uint16_t Joystick_GetX(void);

// Get Button State ONLY (Reads from Hardware Pin)
uint8_t Joystick_GetButton(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_JOYSTICK_H_ */
