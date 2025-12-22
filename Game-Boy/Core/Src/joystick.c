/*
 * joystick.c
 *
 *  Created on: Nov 23, 2025
 *      Author: zjzac
 */

#include "joystick.h"

// --- HARDWARE DEFINITIONS ---
// We explicitly hardcode these to prevent "Macro Confusion"
#define BUTTON_PORT   GPIOA
#define BUTTON_PIN    GPIO_PIN_9

// --- EXTERNAL VARIABLES ---
// We need access to the DMA buffer defined in main.c
extern volatile uint16_t adcValues[2];

// --- FUNCTIONS ---

void Joystick_Init(void) {
    // Hardware initialization is handled in main.c (DMA Start & GPIO Init)
    // We can keep this empty, or use it for calibration logic later.
}

// Function 1: Returns ONLY the X-Axis value
uint16_t Joystick_GetX(void) {
    // Safe read from the DMA buffer
    return adcValues[0];
}

// Function 2: Returns ONLY the Button state
uint8_t Joystick_GetButton(void) {
    // Safe read from the specific GPIO Pin
    return HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_PIN);
}
