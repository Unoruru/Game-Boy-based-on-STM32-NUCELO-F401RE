/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications - Final Version
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "joystick.h"
#include "gameobjects.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
typedef enum {
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAME_OVER
} GameState_t;

/* Private variables ---------------------------------------------------------*/

// --- 1. SHARED RESOURCES ---
extern volatile uint16_t adcValues[2]; // From DMA
volatile JoystickData_t public_joy_data = {2048, 2048, 1}; // Protected by Mutex
volatile GameState_t current_state = STATE_PLAYING;

// --- 2. RTOS OBJECTS ---
// Mutex (For Data Protection)
osMutexId_t DataMutexHandle;
const osMutexAttr_t DataMutex_attributes = { .name = "DataMutex" };

// Semaphore (For Button Signaling)
osSemaphoreId_t ButtonSemHandle;
const osSemaphoreAttr_t ButtonSem_attributes = { .name = "ButtonSem" };

// Queue (For LED Events)
osMessageQueueId_t LedQueueHandle;
const osMessageQueueAttr_t LedQueue_attributes = { .name = "LedQueue" };

// Tasks
osThreadId_t InputTaskHandle;
osThreadId_t GameTaskHandle;
osThreadId_t LedTaskHandle; // NEW TASK

// Task Attributes
const osThreadAttr_t InputTask_attributes = {
  .name = "InputTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
const osThreadAttr_t GameTask_attributes = {
  .name = "GameTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
const osThreadAttr_t LedTask_attributes = {
  .name = "LedTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow, // Low priority background worker
};

/* Function Prototypes */
void StartInputTask(void *argument);
void StartGameTask(void *argument);
void StartLedTask(void *argument); // NEW

void MX_FREERTOS_Init(void);

/**
  * @brief  FreeRTOS initialization
  */
void MX_FREERTOS_Init(void) {
  // Create Mutex
  DataMutexHandle = osMutexNew(&DataMutex_attributes);

  // Create Semaphore (Binary, starts at 0)
  ButtonSemHandle = osSemaphoreNew(1, 0, &ButtonSem_attributes);

  // Create Queue (Depth 5, Size = sizeof(uint8_t))
  LedQueueHandle = osMessageQueueNew(5, sizeof(uint8_t), &LedQueue_attributes);

  // Create Tasks
  InputTaskHandle = osThreadNew(StartInputTask, NULL, &InputTask_attributes);
  GameTaskHandle = osThreadNew(StartGameTask, NULL, &GameTask_attributes);
  LedTaskHandle = osThreadNew(StartLedTask, NULL, &LedTask_attributes);
}

/* -------------------------------------------------------------------------
   TASK 1: INPUT (High Priority)
   Uses: Mutex (Write), Semaphore (Release)
   ------------------------------------------------------------------------- */
void StartInputTask(void *argument)
{
  Joystick_Init();
  uint8_t last_btn_state = Joystick_GetButton;

  for(;;)
  {
    uint16_t raw_x = adcValues[0];
    uint16_t raw_y = adcValues[1];
    uint8_t raw_btn = Joystick_GetButton;

    // 1. MUTEX WRITE: Protect global data
    if (osMutexAcquire(DataMutexHandle, 10) == osOK) {
        public_joy_data.x = raw_x;
        public_joy_data.y = raw_y;
        public_joy_data.button = raw_btn;
        osMutexRelease(DataMutexHandle);
    }

    // 2. SEMAPHORE SIGNAL: Detect button press edge
    if (raw_btn == 0 && last_btn_state == 1) {
        // "Give" the semaphore to signal an event happened.
        // We don't change state here anymore! We just announce the event.
        osSemaphoreRelease(ButtonSemHandle);
    }

    last_btn_state = raw_btn;
    osDelay(20);
  }
}

/* -------------------------------------------------------------------------
   TASK 2: GAME ENGINE (Normal Priority)
   Uses: Mutex (Read), Semaphore (Acquire), Queue (Send)
   ------------------------------------------------------------------------- */
void StartGameTask(void *argument)
{
  ssd1306_Init();
  ssd1306_Fill(Black);
  ssd1306_UpdateScreen();

  Basket_t player;
  FallingObject_t fruit;
  Game_Init(&player, &fruit);

  int score = 0;
  char strBuf[16];
  GameState_t last_loop_state = STATE_PLAYING;

  for(;;)
  {
    // --- 1. CHECK SEMAPHORE (Button Event) ---
    // Check if InputTask signaled a button press (Don't wait, return immediately)
    if (osSemaphoreAcquire(ButtonSemHandle, 0) == osOK) {
        // Semaphore taken! A press occurred. Toggle State.
        if (current_state == STATE_PLAYING) current_state = STATE_PAUSED;
        else if (current_state == STATE_PAUSED) current_state = STATE_PLAYING;
        else if (current_state == STATE_GAME_OVER) current_state = STATE_PLAYING;
    }

    // --- 2. GET INPUT (Mutex) ---
    JoystickData_t input = {2048, 2048, 1};
    if (osMutexAcquire(DataMutexHandle, 10) == osOK) {
        input = public_joy_data;
        osMutexRelease(DataMutexHandle);
    }

    // --- 3. GAME LOGIC ---
    ssd1306_Fill(Black);

    switch (current_state) {
        case STATE_PLAYING:
            if (last_loop_state == STATE_GAME_OVER) {
                score = 0;
                Game_Init(&player, &fruit);
            }

            Basket_Update(&player, input);
            Object_Update(&fruit);

            if (Check_Collision(&player, &fruit)) {
                uint8_t msg;
                if (fruit.type == 2) { // Bomb
                    current_state = STATE_GAME_OVER;
                    msg = 2; // Event Type 2 = Bomb
                } else {
                    score++;
                    Object_Spawn(&fruit);
                    msg = 1; // Event Type 1 = Fruit Catch
                }

                // --- QUEUE SEND: Notify LED Task ---
                osMessageQueuePut(LedQueueHandle, &msg, 0, 0);
            }

            if (fruit.active == 0) Object_Spawn(&fruit);
            Game_Draw(&player, &fruit);

            sprintf(strBuf, "%d", score);
            ssd1306_SetCursor(0, 0);
            ssd1306_WriteString(strBuf, Font_7x10, White);
            break;

        case STATE_PAUSED:
            Game_Draw(&player, &fruit);
            ssd1306_SetCursor(40, 25);
            ssd1306_WriteString("PAUSED", Font_7x10, White);
            break;

        case STATE_GAME_OVER:
            ssd1306_SetCursor(30, 20);
            ssd1306_WriteString("GAME OVER", Font_7x10, White);
            sprintf(strBuf, "Score: %d", score);
            ssd1306_SetCursor(35, 40);
            ssd1306_WriteString(strBuf, Font_7x10, White);
            break;
    }

    last_loop_state = current_state;
    ssd1306_UpdateScreen();
    osDelay(33);
  }
}

/* -------------------------------------------------------------------------
   TASK 3: LED FEEDBACK (Low Priority)
   Uses: Queue (Receive)
   ------------------------------------------------------------------------- */
void StartLedTask(void *argument)
{
  uint8_t eventMsg;

  for(;;)
  {
      // Wait FOREVER for a message in the queue
      // This task consumes 0% CPU while waiting.
      if (osMessageQueueGet(LedQueueHandle, &eventMsg, NULL, osWaitForever) == osOK)
      {
          if (eventMsg == 1) {
              // FRUIT CAUGHT: Quick Blink
              HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
              osDelay(100);
              HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
          }
          else if (eventMsg == 2) {
              // BOMB HIT: 3 Fast Blinks
              for(int i=0; i<3; i++) {
                  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
                  osDelay(100);
                  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
                  osDelay(100);
              }
          }
      }
  }
}

