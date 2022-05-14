/**
 * @file bsp_timers.h
 * @author DFlubacher
 * @brief Routines for interfacing timers.
 * @version 0.1
 * @date 2022-04-25
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef BSP_INCLUDE_BSP_TIMERS_H_
#define BSP_INCLUDE_BSP_TIMERS_H_

#include <stdint.h>

// ////////////////////////////////////////////////////////////////////////////
// Timers
// ----------------------------------------------------------------------------

/**
 * @brief Timer 6 Init, clocked at 48MHz (PCLK1, APB1 prescaler is /2,
 * therefore it is multiplied by 2 again, see Fig. 13.
 * Mode: Counter with auto-reload.
 *
 * @param prescaler
 * @param interval
 */
void BspTimer6TimeBaseInit(uint16_t prescaler, uint16_t interval);

/**
 * @brief Timer 6 Init, clocked at 48MHz (PCLK1, APB1 prescaler is /2,
 * therefore it is multiplied by 2 again, see Fig. 13.
 * Mode: Busy delay (bad practice, only for demo).
 * Prescaler: 48000 --> resolution: 1ms.
 * Auto-reload set to max.
 */
void BspTimer6DelayInit(void);

/**
 * @brief Delay with busy waiting (bad practice, only for demo).
 *
 * @param delay_ms
 */
void BspTimer6Delay(uint16_t delay_ms);

/**
 * @brief Timer 3 Init, clocked at 48MHz (PCLK1, APB1 prescaler is /2,
 * therefore it is multiplied by 2 again, see Fig. 13.
 * Mode: Input capture, single falling edge.
 * Pin: PB4 (AF2, ref. table 14 datasheet), TIM3_CH1.
 * PB4 is connected to Morpho Connector pin 27 (CN10-27), labelled D5.
 * Prescaler: 48000 --> resolution: 1ms.
 */
void BspTimer3InputCaptureSingleEdgeInit(void);

/**
 * @brief Timer 3 Init, clocked at 48MHz (PCLK1, APB1 prescaler is /2,
 * therefore it is multiplied by 2 again, see Fig. 13.
 * Mode: Input capture, falling edge to rising edge timing.
 * Pin: PB4 (AF2, ref. table 14 datasheet), TIM3_CH1.
 * PB4 is connected to Morpho Connector pin 27 (CN10-27), labelled D5.
 * Prescaler: 48000 --> resolution: 1ms.
 */
void BspTimer3InputCaptureDoubleEdgeInit(void);

/**
 * @brief Timer 3 Init, clocked at 48MHz (PCLK1, APB1 prescaler is /2,
 * therefore it is multiplied by 2 again, see Fig. 13.
 * Mode: Input capture, falling edge to rising edge timing. Falling edge resets
 * timer.
 * Pin: PB4 (AF2, ref. table 14 datasheet), TIM3_CH1.
 * PB4 is connected to Morpho Connector pin 27 (CN10-27), labelled D5.
 * Prescaler: 48000 --> resolution: 1ms.
 * @note:
 *    - maximum 65536 ms.
 *    - timer 3 is a 16-bit counter with 32-bit registers.
 */
void BspTimer3InputCaptureTimingInit(void);

/**
 * @brief PWM signal generation for timer 8 (16 bit).
 * Clocked by internal clock according clock tree in reference manual, but the
 * enable bits are in the APB2 RCC. Either way 48 MHz.
 * On STM32F303RE this uses the following pins:
 *   - TIM8_CH1 -> PA15 (AF2) -> Morpho Connector (CN7)-17.
 *   - TIM8_CH2 -> PA14 (AF5) -> Morpho Connector (CN7)-15.
 * Prescaler: 48 --> resolution: 1us.
 * Period: 11 ms.
 * @param period_ms
 *
 */
void BspTimer8PwmInit(uint16_t period_ms);

#endif /* BSP_INCLUDE_BSP_TIMERS_H_ */