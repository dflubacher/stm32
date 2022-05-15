/**
 * @file main.c
 * @author DFlubacher
 * @brief FreeRTOS: Binary Semaphore example.
 * @version 0.1
 * @date 2022-05-13
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

// Kernel objects: Binary semaphore.
xSemaphoreHandle x_semaphore;

/**
 * @brief Bare metal programming with FreeRTOS example.
 * - Binary semaphore example. As long as push button is pressed, the LED is
 *   toggled by task 1 and a Semaphore is given. Semaphore is taken by task 2
 *   or times out after 2 seconds with a corresponding message.
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

  // Create a semaphore object (initialization).
  x_semaphore = xSemaphoreCreateBinary();

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
 * @brief Task 1 toggles the LED every 100 ms.
 *
 * @param pv_parameters
 */
void vTask1(void* pv_parameters) {
  uint16_t count = 0;
  // Initialize timing.
  portTickType time_last_wakeup = xTaskGetTickCount();

  while (1) {
    // Toggle LED only if user push button is pressed:
    if (BspPbGetState()) {
      BspLedToggle();
      ++count;
    }

    // Release semaphore every 10th count.
    if (count == 10) {
      // 'Give' semaphore, i.e. trigger Task 2. This yields to equal or higher
      // priority tasks.
      xSemaphoreGive(x_semaphore);

      // Reset count (probably more efficient than using modulo).
      count = 0;
    }
    // Wait for remainder of period.
    vTaskDelayUntil(&time_last_wakeup, (100 / portTICK_PERIOD_MS));
  }
}

/**
 * @brief Task 2 write out a message every second.
 *
 * @param pv_parameters
 */
void vTask2(void* pv_parameters) {
  portBASE_TYPE status_semaphore;
  uint16_t count = 0;

  // Take the semaphore once to make sure it is empty.
  xSemaphoreTake(x_semaphore, 0);

  while (1) {
    // Wait for semaphore (maximum timeout). This is an OS aware waiting
    // mechanism, therefore we don't need a delay function anymore.
    // Thus is asynchronously take semaphore. After printing it immediately
    // takes the semaphore again.
    status_semaphore = xSemaphoreTake(x_semaphore, 2000);

    if (status_semaphore == pdPASS) {
      // Semaphore taken successfully.
      stm32_printf("Semaphore taken %2dth time by task 2\r\n", count);
      ++count;
    } else {
      // Semaphore timed out.
      stm32_printf("Task_2 timed out waiting for semaphore.\r\n");
    }
  }
}