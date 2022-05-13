/**
 * @file bsp_timers.c
 * @author DFlubacher
 * @brief Initialization routines for timers
 * @version 0.1
 * @date 2022-04-27
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "bsp_timers.h"

#include <stdint.h>

#include "stm32f303xe.h"
#include "stm32f3xx.h"

// ////////////////////////////////////////////////////////////////////////////
// Timers
// ----------------------------------------------------------------------------

void BspTimer6TimeBaseInit(uint16_t prescaler, uint16_t interval) {
  // Enable TIM6 clock. Basically source TIM6 with APB1 clock.
  RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

  // Reset timer 6 configuration (16-bit).
  TIM6->CR1 = 0x0000;
  TIM6->CR2 = 0x0000;

  // Set TIM6 prescaler:
  // Freq: 48 MHz -> /48000 -> 1000 Hz counter frequency, i.e. 1 ms resolution.
  // As register is zero based, but prescaler is 1 per default, you need to
  // subtract 1.
  TIM6->PSC = (uint16_t)(prescaler - 1);

  // Prescaler yields resolution, interval has to be converted to zero based.
  TIM6->ARR = (uint16_t)(interval - 1);

  // Enable Auto-reload preload. AFAIK it buffers the Auto-reload register,
  // i.e. when changing the ARR, it doesn't directly write the register, but
  // only at the update event.
  TIM6->CR1 |= TIM_CR1_ARPE;

  // Enable interrupt on Update Event.
  TIM6->DIER |= TIM_DIER_UIE;

  // Set counter enable bit.
  TIM6->CR1 |= TIM_CR1_CEN;
}

void BspTimer6DelayInit(void) {
  // Enable timer 6 clock. Basically source TIM6 with APB1 clock.
  RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

  // Reset TIM6 configuration.
  TIM6->CR1 = 0x0000;
  TIM6->CR2 = 0x0000;

  // Set TIM6 prescaler:
  // Freq: 48 MHz -> /48000 -> 1000 Hz counter frequency.
  // As register is zero based, but prescaler is 1 per default, you need to
  // subtract 1.
  TIM6->PSC = (uint16_t)47999;

  // Set ARR to maximum value.
  TIM6->ARR = (uint16_t)0xFFFF;
}

void BspTimer6Delay(uint16_t delay_ms) {
  // Reset the TIM6 counter.
  TIM6->EGR = TIM_EGR_UG;

  // Start the TIM6 counter.
  TIM6->CR1 |= TIM_CR1_CEN;

  // Wait until the TIM6 counter reaches the `delay_ms` value.
  while (TIM6->CNT < delay_ms)
    ;

  // Stop the TIM6 counter.
  TIM6->CR1 &= ~TIM_CR1_CEN;
}

void BspTimer3InputCaptureSingleEdgeInit(void) {
  // 1. Configure PB4.
  // Enable port B.
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

  // Set PB4 to mode 'Alternate Function' (0b10)..
  GPIOB->MODER &= ~GPIO_MODER_MODER4_Msk;
  GPIOB->MODER |= (0x02 << GPIO_MODER_MODER4_Pos);

  // Select AF2 for PB4.
  GPIOB->AFR[0] &= ~GPIO_AFRL_AFRL4_Msk;
  GPIOB->AFR[0] |= (0x02 << GPIO_AFRL_AFRL4_Pos);

  // 2. Configure Timer (see 20.3.7 for reference).
  // Enable TIM3 clock. Basically source TIM3 with APB1 clock.
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

  // Reset the timer 3 configuration (16-bit).
  TIM3->CR1 = 0x0000;
  TIM3->CR2 = 0x0000;
  // Reset the capture/compare enable register.
  TIM3->CCER = 0x0000;
  TIM3->PSC = (uint16_t)47999;

  // Set the auto-reload value to maximum so that counter can reach 2^16 - 1;
  TIM3->ARR = 0xFFFF;

  // Associate Channel 1 with trigger input 1 of TIM3 (21.4.7).
  // Reset Capture/compare mode register 1 & 2.
  TIM3->CCMR1 = 0x0000;
  TIM3->CCMR2 = 0x0000;

  // TIM3_CH1 is the input pin, TI1: Timer input is the detected input to
  // the timer (see Figure 197 'General-purpose timer block diagram').
  // We need to first specify if it is an input or an output.
  // CC1S: Capture Compare 1 Selection.
  // 0b01: CC1 is configured as input, IC1 is mapped on TI1.
  TIM3->CCMR1 |= (0x01 << TIM_CCMR1_CC1S_Pos);

  // They make a difference between timer clock and sampling clock.
  // CK_INT is the clock used to drive the TIMER, dead-time and
  // sampling clock DTS. You could divide CK_INT to get a slower
  // DTS, but we don't do that here. But we use a debounce/filter, meaning only
  // if 8 consecutive events are captured is it considered a transition.
  TIM3->CCMR1 |= (0x03 << TIM_CCMR1_IC1F_Pos);

  // The push button is active low, i.e. shows HIGH when not pressed.
  // To start the timer when it gets pressed, we need to listen for
  // falling edges.
  // This is set in capture/compare enable register. There are two bits
  // for input capture CC1NP (bit 3) and CC1P (bit 1).
  // CC1P explains: 0b01: inverted/falling edge.
  TIM3->CCER |= ((0x00 << TIM_CCER_CC1NP_Pos) | (0x01 << TIM_CCER_CC1P_Pos));

  // Enable input capture on channel 1.
  TIM3->CCER |= (0x01 << TIM_CCER_CC1E_Pos);

  // Enable TIMER 3.
  TIM3->CR1 |= TIM_CR1_CEN;
}

void BspTimer3InputCaptureDoubleEdgeInit(void) {
  // 1. Configure PB4.
  // Enable port B.
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

  // Set PB4 to mode 'Alternate Function' (0b10)..
  GPIOB->MODER &= ~GPIO_MODER_MODER4_Msk;
  GPIOB->MODER |= (0x02 << GPIO_MODER_MODER4_Pos);

  // Select AF2 for PB4.
  GPIOB->AFR[0] &= ~GPIO_AFRL_AFRL4_Msk;
  GPIOB->AFR[0] |= (0x02 << GPIO_AFRL_AFRL4_Pos);

  // 2. Configure Timer (see 20.3.7 for reference).
  // Enable TIM3 clock. Basically source TIM3 with APB1 clock.
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

  // Reset the timer 3 configuration (16-bit).
  TIM3->CR1 = 0x0000;
  TIM3->CR2 = 0x0000;
  // Reset the capture/compare enable register.
  TIM3->CCER = 0x0000;
  TIM3->PSC = (uint16_t)47999;

  // Set the auto-reload value to maximum so that counter can reach 2^16 - 1;
  TIM3->ARR = 0xFFFF;

  // Associate Channel 1 with trigger input 1 of TIM3 (21.4.7).
  // Reset Capture/compare mode register 1 & 2.
  TIM3->CCMR1 = 0x0000;
  TIM3->CCMR2 = 0x0000;

  // The general purpose timers have two channels, which can be used
  // differently depending on input capture or output compare mode.
  // We use one channel for falling edge detection and the other for rising
  // edge detection, i.e. demux TIM3_CH1 (the physical input pin) to CC1 and
  // CC2

  // TIM3_CH1 is the input pin, TI1: Timer input is the detected input to
  // the timer (see Figure 197 'General-purpose timer block diagram').
  // We need to first specify if it is an input or an output.
  // CC1S: Capture Compare 1 Selection.
  // 0b01: CC1 channel is configured as input, IC1 is mapped on TI1.
  TIM3->CCMR1 |= (0x01 << TIM_CCMR1_CC1S_Pos);

  // 0b10: CC2 channel is configured as input, IC2 is mapped on TI1.
  TIM3->CCMR1 |= (0x02 << TIM_CCMR1_CC2S_Pos);

  // They make a difference between timer clock and sampling clock.
  // CK_INT is the clock used to drive the TIMER, dead-time and
  // sampling clock DTS. You could divide CK_INT to get a slower
  // DTS, but we don't do that here. But we use a debounce/filter, meaning only
  // if 8 consecutive events are captured is it considered a transition. Do
  // this on both channels
  TIM3->CCMR1 |= (0x03 << TIM_CCMR1_IC1F_Pos | 0x03 << TIM_CCMR1_IC2F_Pos);

  // The push button is active low, i.e. shows HIGH when not pressed.
  // To start the timer when it gets pressed, we need to listen for
  // falling edges.
  // This is set in capture/compare enable register. There are two bits
  // for input capture CC1NP (bit 3) and CC1P (bit 1).
  // CC1P explains: 0b01: inverted/falling edge.
  TIM3->CCER |= ((0x00 << TIM_CCER_CC1NP_Pos) | (0x01 << TIM_CCER_CC1P_Pos));

  // Select rising edge on channel 2 (00: noninverted/rising edge).
  TIM3->CCER |= ((0x00 << TIM_CCER_CC2NP_Pos) | (0x00 << TIM_CCER_CC2P_Pos));

  // Enable input capture on channel 1 and 2.
  TIM3->CCER |= (0x01 << TIM_CCER_CC1E_Pos | 0x01 << TIM_CCER_CC2E_Pos);

  // Enable TIMER 3.
  TIM3->CR1 |= TIM_CR1_CEN;
}

void BspTimer3InputCaptureTimingInit(void) {
  // 1. Configure PB4.
  // Enable port B.
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

  // Set PB4 to mode 'Alternate Function' (0b10)..
  GPIOB->MODER &= ~GPIO_MODER_MODER4_Msk;
  GPIOB->MODER |= (0x02 << GPIO_MODER_MODER4_Pos);

  // Select AF2 for PB4.
  GPIOB->AFR[0] &= ~GPIO_AFRL_AFRL4_Msk;
  GPIOB->AFR[0] |= (0x02 << GPIO_AFRL_AFRL4_Pos);

  // 2. Configure Timer (see 20.3.7 for reference).
  // Enable TIM3 clock. Basically source TIM3 with APB1 clock.
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

  // Reset the timer 3 configuration (16-bit).
  TIM3->CR1 = 0x0000;
  TIM3->CR2 = 0x0000;
  // Reset the capture/compare enable register.
  TIM3->CCER = 0x0000;
  TIM3->PSC = (uint16_t)47999;

  // Set the auto-reload value to maximum so that counter can reach 2^16 - 1;
  TIM3->ARR = 0xFFFF;

  // Associate Channel 1 with trigger input 1 of TIM3 (21.4.7).
  // Reset Capture/compare mode register 1 & 2.
  TIM3->CCMR1 = 0x0000;
  TIM3->CCMR2 = 0x0000;

  // TIM3_CH1 is the input pin, TI1: Timer input is the detected input to
  // the timer.
  // We need to first specify if it is an input or an output.
  // CC1S: Capture Compare 1 Selection.
  // 0b01: CC1 channel is configured as input, IC1 is mapped on TI1.
  TIM3->CCMR1 |= (0x01 << TIM_CCMR1_CC1S_Pos);

  // 0b10: CC2 channel is configured as input, IC2 is mapped on TI1.
  TIM3->CCMR1 |= (0x02 << TIM_CCMR1_CC2S_Pos);

  // They make a difference between timer clock and sampling clock.
  // CK_INT is the clock used to drive the TIMER, dead-time and
  // sampling clock DTS. You could divide CK_INT to get a slower
  // DTS, but we don't do that here. But we use a debounce/filter, meaning only
  // if 8 consecutive events are captured is it considered a transition. Do
  // this on both channels
  TIM3->CCMR1 |= (0x03 << TIM_CCMR1_IC1F_Pos | 0x03 << TIM_CCMR1_IC2F_Pos);

  // The push button is active low, i.e. shows HIGH when not pressed.
  // To start the timer when it gets pressed, we need to listen for
  // falling edges.
  // This is set in capture/compare enable register. There are two bits
  // for input capture CC1NP (bit 3) and CC1P (bit 1).
  // CC1P explains: 0b01: inverted/falling edge.
  TIM3->CCER |= ((0x00 << TIM_CCER_CC1NP_Pos) | (0x01 << TIM_CCER_CC1P_Pos));

  // Select rising edge on channel 2 (00: noninverted/rising edge).
  TIM3->CCER |= ((0x00 << TIM_CCER_CC2NP_Pos) | (0x00 << TIM_CCER_CC2P_Pos));

  // Enable reset by a trigger (slave mode control register).
  // First select trigger input. This is the filter input of channel 1
  // (TI1FP1, 0b101) .
  TIM3->SMCR &= ~TIM_SMCR_TS_Msk;
  TIM3->SMCR |= (0x05 << TIM_SMCR_TS_Pos);

  // I.e. slave mode set to 'reset mode', rising edge of trigger input resets
  // the counter.
  TIM3->SMCR &= ~TIM_SMCR_SMS_Msk;
  TIM3->SMCR |= (0x04 << TIM_SMCR_SMS_Pos);

  // Enable input capture on channel 1 and 2.
  TIM3->CCER |= (0x01 << TIM_CCER_CC1E_Pos | 0x01 << TIM_CCER_CC2E_Pos);

  // Enable TIMER 3.
  TIM3->CR1 |= TIM_CR1_CEN;
}

void BspTimer8PwmInit(uint16_t period_ms) {
  // 1. Configure PA 14/15.
  // Enable port A.
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

  // Set PA14 and PA15 to mode 'Alternate Function' (0b10)..
  GPIOA->MODER &= ~(GPIO_MODER_MODER14_Msk | GPIO_MODER_MODER15_Msk);
  GPIOA->MODER |=
      (0x02 << GPIO_MODER_MODER14_Pos | 0x02 << GPIO_MODER_MODER15_Pos);

  // Select AF5 for PA14 and AF2 for PA15.
  GPIOB->AFR[1] &= ~(GPIO_AFRH_AFRH6_Msk | GPIO_AFRH_AFRH7_Msk);
  GPIOB->AFR[1] |= (0x02 << GPIO_AFRH_AFRH6_Pos);
  GPIOB->AFR[1] |= (0x05 << GPIO_AFRH_AFRH7_Pos);

  // Enable timer 8.
  RCC->APB2ENR |= RCC_APB2ENR_TIM8EN;

  // Reset the timer 8 configuration (16-bit).
  TIM8->CR1 = 0x0000;
  TIM8->CR2 = 0x0000;
  // Reset the capture/compare enable register.
  TIM8->CCER = 0x0000;

  // Prescaler set to 48 --> 1 MHz.
  TIM8->PSC = (uint16_t)47;

  // Set the auto-reload value to the period in us.
  TIM8->ARR = (uint16_t)(period_ms * 1000 - 1);

  // Enable auto-reload preload register. When you write to the register, the
  // value is buffered and not directly written, only when there is an update
  // event. This could be useful, if the period is changed.
  TIM8->CR1 |= TIM_CR1_ARPE;

  // Reset the Capture/Compare Mode Registers.
  TIM8->CCMR1 = 0x0000;
  TIM3->CCMR2 = 0x0000;

  // Set mode two channels as output and PWM mode 1.
  // Output Compare 1 Mode 0b110: channel 1 is active as long as CNT < CCR1.
  // Enable Output Compare Preload Enable on channel 1.
  TIM8->CCMR1 |= (0x06 << TIM_CCMR1_OC1M_Pos | TIM_CCMR1_OC1PE);

  // Same for channel 2:
  TIM8->CCMR1 |= (0x06 << TIM_CCMR1_OC2M_Pos | TIM_CCMR1_OC2PE);

  // Initialize PWM duty cycle.
  TIM8->CCR1 = 1500;
  TIM8->CCR2 = 1500;

  // Enable both outputs.
  TIM8->CCER |= (TIM_CCER_CC1E | TIM_CCER_CC2E);

  // Enable main output.
  TIM8->BDTR |= TIM_BDTR_MOE;

  // Enable timer 8.
  TIM8->CR1 |= TIM_CR1_CEN;
}
