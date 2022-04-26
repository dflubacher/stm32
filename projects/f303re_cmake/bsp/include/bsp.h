/**
 * @file bsp.h
 * @author DFlubacher
 * @brief Routines for interfacing timers, LEDs, serial comms.
 * @version 0.1
 * @date 2022-04-25
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef BSP_INCLUDE_H_
#define BSP_INCLUDE_H_

#include <stdint.h>

#include "stm32f3xx.h"

// ////////////////////////////////////////////////////////////////////////////
// LED driver functions
// ----------------------------------------------------------------------------
/**
 * @brief Initialize LED pin as a High-Speed Push-pull output.
 * Set LED initial state to OFF.
 * Refer to reference manual (Input/output AC characteristics). High-speed
 * can be 20-50 MHz depending on voltage etc.
 * Push-pull means V_DD when HIGH and GND when LOW.
 * PA5: see schematics 'Extension connectors' (p.4).
 */
void BspLedInit(void);

/**
 * @brief Turn on LED on PA5.
 */
void BspLedOn(void);

/**
 * @brief Turn off LED on PA5.
 */
void BspLedOff(void);

/**
 * @brief Toggle LED on PA5.
 */
void BspLedToggle(void);

// ////////////////////////////////////////////////////////////////////////////
// Pushbutton driver functions
// ----------------------------------------------------------------------------
/**
 * @brief Configure PC13 as an input without pull-up/pull-down (pull-up
 * resistor externally). See schematics 'MCU' (p.2).
 */
void BspPbInit(void);

uint8_t BspPbGetState(void);
#endif /* BSP_INCLUDE_H_ */