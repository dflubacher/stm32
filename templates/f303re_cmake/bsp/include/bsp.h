/**
 * @file bsp.h
 * @author DFlubacher
 * @brief Routines for interfacing LEDs, push-buttons, serial comms.
 * @version 0.1
 * @date 2022-04-25
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef BSP_INCLUDE_BSP_H_
#define BSP_INCLUDE_BSP_H_

#include <stdint.h>

// ////////////////////////////////////////////////////////////////////////////
// Console
// ----------------------------------------------------------------------------

/**
 * @brief U(S)ART2 for console communication (USART2_CK not used).
 * According schematics 'MCU' (p.2):
 *   - PA2 --> USART_TX
 *   - PA3 --> USART_RX
 * Refer to datasheet, table 14:
 *   - PA2, USART2_TX is alternate function 7.
 *   - PA3, USART2_RX is alternate function 7.
 */
void BspConsoleInit(void);

/**
 * @brief Write single byte over Console (USART2).
 *
 * @param char_to_send
 * @return uint8_t
 */
uint8_t BspConsolePutChar(char char_to_send);

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
 * PC13 is connected to Morpho Connector pin 23 (CN7-23), no label.
 */
void BspPbInit(void);

uint8_t BspPbGetState(void);

/**
 * @brief Setup Nested Vector Interrupt Controller for selected interrupts.
 *
 */
void BspNvicInit(void);

#endif /* BSP_INCLUDE_BSP_H_ */