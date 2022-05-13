/**
 * @file main.c
 * @author DFlubacher
 * @brief
 * @version 0.1
 * @date 2022-05-13
 *
 */

#include "main.h"

#include <stdint.h>

// Device header.
#include "portmacro.h"
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

/**
 * @brief Bare metal programming with FreeRTOS example.
 * -
 * @return int
 */
int main(void) {
  // Configure the System Clock.
  SystemClockConfig();

  // Initialize LED and user button.
  BspLedInit();

  // Initialize Console.
  BspConsoleInit();
  stm32_printf("\r\n\n------------------------\r\n");
  stm32_printf("Console initialized!\r\n");
  stm32_printf("SYSCLK: %d Hz\r\n", SystemCoreClock);
  stm32_printf("------------------------\r\n\n");

  // Create FreeRTOS tasks.
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
 * @brief Task 1 toggles the LED every 300 ms.
 *
 * @param pv_parameters
 */
void vTask1(void* pv_parameters) {
  while (1) {
    BspLedToggle();
    vTaskDelay(300 / portTICK_PERIOD_MS);
  }
}

/**
 * @brief Task 2 write out a message every second.
 *
 * @param pv_parameters
 */
void vTask2(void* pv_parameters) {
  uint16_t count = 0;
  while (1) {
    stm32_printf("Hello %2d from task 2\r\n", count);
    ++count;
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}