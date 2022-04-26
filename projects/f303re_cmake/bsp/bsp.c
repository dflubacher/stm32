/**
 * @file bsp.c
 * @author DFlubacher
 * @brief
 * @version 0.1
 * @date 2022-04-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "bsp.h"

#include <stdint.h>

#include "stm32f303xe.h"

// ////////////////////////////////////////////////////////////////////////////
// LED driver functions
// ----------------------------------------------------------------------------

void BspLedInit(void) {
  // Enable GPIOA clock.
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

  // Configure PA5 as output. 0b01: General purpose output mode.
  GPIOA->MODER &= ~GPIO_MODER_MODER5_Msk;
  GPIOA->MODER |= (0x01 << GPIO_MODER_MODER5_Pos);

  // Configure PA5 as Push-Pull output. 0b00: Default.
  GPIOA->OTYPER &= ~GPIO_OTYPER_OT_5;

  // Configure PA5 as High-Speed Output. 0b11: High speed.
  GPIOA->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR5_Msk;
  GPIOA->OSPEEDR |= (0x03 << GPIO_OSPEEDER_OSPEEDR5_Pos);

  // Disable PA5 Pull-up/Pull-down.
  GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR5_Msk;

  // Set initial state to OFF.
  GPIOA->BSRR |= GPIO_BSRR_BR_5;
}

void BspLedOn(void) { GPIOA->BSRR |= GPIO_BSRR_BS_5; }

void BspLedOff(void) { GPIOA->BSRR |= GPIO_BSRR_BR_5; }

void BspLedToggle(void) { GPIOA->ODR ^= GPIO_ODR_5; }

// ////////////////////////////////////////////////////////////////////////////
// Pushbutton driver functions
// ----------------------------------------------------------------------------

void BspPbInit(void) {
  // Enable GPIOC clock.
  RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

  // Configure PC13 as input.
  GPIOC->MODER &= ~GPIO_MODER_MODER13_Msk;
  GPIOC->MODER |= (0x00 << GPIO_MODER_MODER13_Pos);

  // Configure PC13 without pull-up/pull-down.
  GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR13_Msk;
  GPIOC->PUPDR |= (0x00 << GPIO_PUPDR_PUPDR13_Pos);
}

uint8_t BspPbGetState(void) {
  uint8_t state = 0;

  // The schematics show an inverted logic, i.e. pushbutton press pulls the
  // input high. Therefore inverte the state here.
  state = (uint8_t)((GPIOC->IDR & GPIO_IDR_13) == 0);

  return state;
}

uint8_t BspPbGetState(void);