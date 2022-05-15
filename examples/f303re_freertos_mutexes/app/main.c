/**
 * @file main.c
 * @author DFlubacher
 * @brief FreeRTOS: Mutex example.
 * @version 0.1
 * @date 2022-05-15
 *
 */

#include "main.h"

#include <stdint.h>

// Device header.
#include "portmacro.h"
#include "projdefs.h"
#include "stm32f3xx.h"

// BSP functions.
#include "bsp.h"

// FreeRTOS headers.
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "event_groups.h"
#include "queue.h"
#include "semphr.h"
#include "stream_buffer.h"
#include "task.h"
#include "timers.h"

// #include "bsp_timers.h"
// #include "comms.h"
// #include "dac.h"
#include "printf-stdarg.h"

// Forward declarations, FreeRTOS tasks.
void vTask1(void* pv_parameters);
void vTask2(void* pv_parameters);

// Kernel objects: Mutex.
xSemaphoreHandle x_console_mutex;

/**
 * @brief Bare metal programming with FreeRTOS example.
 * - Mutex example. The console output is shared between task 1 and task 2. In
 *   order to protect it and give exclusive access, a mutex is introduced.
 * @return int
 */
int main(void) {
  // Configure the System Clock.
  SystemClockConfig();

  // Initialize LED and user button.
  BspLedInit();
  BspPbInit();

  // Initialize Console.
  BspConsoleInit();
  stm32_printf("\r\n\n------------------------\r\n");
  stm32_printf("Console initialized!\r\n");
  stm32_printf("SYSCLK: %d Hz\r\n", SystemCoreClock);
  stm32_printf("------------------------\r\n\n");

  // Create a mutex to access the console (initialization).
  x_console_mutex = xSemaphoreCreateMutex();

  // Create FreeRTOS tasks. Task 2 has a higher priority than Task 1.
  xTaskCreate(vTask1, "Task_1", 256, NULL, 1, NULL);
  xTaskCreate(vTask2, "Task_2", 256, NULL, 2, NULL);

  // Start the FreeRTOS scheduler.
  vTaskStartScheduler();

  /* Loop forever */
  while (1) {
    // The program should never reach this section.
  }
}

/**
 * @brief Task 1 prints out a message every 100 ms.
 *
 * @param pv_parameters
 */
void vTask1(void* pv_parameters) {
  // Initialize timing.
  portTickType time_last_wakeup = xTaskGetTickCount();
  uint16_t count = 0;

  while (1) {
    // Take mutex to print message
    xSemaphoreTake(x_console_mutex, portMAX_DELAY);
    stm32_printf("FreeRTOS Task 1, mutex example, message count: %d\r\n",
                 count);
    // Release mutex.
    xSemaphoreGive(x_console_mutex);

    ++count;

    // Wait for remainder of period.
    vTaskDelayUntil(&time_last_wakeup, (100 / portTICK_PERIOD_MS));
  }
}

/**
 * @brief Task 2 write out a message every 20 ms.
 *
 * @param pv_parameters
 */
void vTask2(void* pv_parameters) {
  // Initialize timing.
  portTickType time_last_wakeup = xTaskGetTickCount();
  uint16_t count = 0;

  while (1) {
    // Take mutex for console.
    xSemaphoreTake(x_console_mutex, portMAX_DELAY);

    // Send message to console.
    stm32_printf("#");

    // Release mutex.
    xSemaphoreGive(x_console_mutex);

    vTaskDelayUntil(&time_last_wakeup, (20 / portTICK_PERIOD_MS));
  }
}