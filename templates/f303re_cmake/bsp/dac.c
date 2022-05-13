/**
 * @file dac.c
 * @author DFlubacher
 * @date 2022-04-28
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "dac.h"

#include "stm32f303xe.h"
#include "stm32f3xx.h"

void BspDacInit(void) {
  // Enable GPIOA port clock for PA4.
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

  // Configure PA 4 as in analog mode (0b11).
  GPIOA->MODER &= ~(GPIO_MODER_MODER4_Msk);
  GPIOA->MODER |= (0x03 << GPIO_MODER_MODER4_Pos);

  // Enable DAC clock.
  RCC->APB1ENR |= RCC_APB1ENR_DAC1EN;

  // Enable DAC Channel 1.
  DAC1->CR &= 0x00000000;
  DAC1->CR |= DAC_CR_EN1;
}
