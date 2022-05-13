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

#include "bsp_timers.h"
#include "printf-stdarg.h"
// #include "stm32f303xe.h"
#include "stm32f3xx.h"

// ////////////////////////////////////////////////////////////////////////////
// Clock Initialization
// ----------------------------------------------------------------------------

void SystemClockConfig(void) {
  uint32_t hse_status;
  uint32_t pll_status;
  uint32_t sw_status;
  uint32_t timeout = 1000000;

  // Set HSE bypass mode, see 9.4.1 in ref manual. Bypass mode can only be set
  // with HSE being switched off.
  RCC->CR &= ~RCC_CR_HSEON_Msk;
  RCC->CR |= RCC_CR_HSEBYP;
  RCC->CR |= RCC_CR_HSEON;

  // Wait until HSE is ready or timeout has expired.
  do {
    // HSE clock ready flag, indicates that HSE input is stable.
    hse_status = RCC->CR & RCC_CR_HSERDY_Msk;
    --timeout;
  } while ((hse_status == 0) && (timeout > 0));

  // We send the HSE input through the PLL, hence select it as PLL input
  // source, see 9.4.2.
  // First reset the PLL input clock source.
  RCC->CFGR &= ~RCC_CFGR_PLLSRC_Msk;
  // Set PLL entry clock source to 0b10: HSE/PREDIV1.
  RCC->CFGR |= (0x02 << RCC_CFGR_PLLSRC_Pos);

  // Set PLL PREDIV to /1. Can only be set if PLL is disabled.
  RCC->CR &= ~RCC_CR_PLLON_Msk;
  RCC->CFGR2 = 0x00000000;

  // Now we set the PLL multiplication factor to 6 to reach 48 MHz.
  RCC->CFGR &= ~RCC_CFGR_PLLMUL_Msk;
  RCC->CFGR |= (0x04 << RCC_CFGR_PLLMUL_Pos);

  // Enable the PLL.
  RCC->CR |= RCC_CR_PLLON;

  // Wait until the PLL is ready.
  // Reset timeout to 1000000 cycles.
  timeout = 1000000;
  do {
    pll_status = (RCC->CR & RCC_CR_PLLRDY_Msk);
    --timeout;
  } while ((pll_status == 0) && (timeout > 0));

  // Reset prescalers.
  RCC->CFGR &= ~(RCC_CFGR_HPRE_Msk | RCC_CFGR_PPRE1_Msk | RCC_CFGR_PPRE2_Msk);

  // Set AHB prescaler to /1.
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

  // Set APB1 prescaler to /2, APB1 is limited to 36 MHz (see 3.1.5 and 9.2).
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

  // Set APB2 prescaler to /1, high-speed APB..
  RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;

  // Enable Flash Prefetch Buffer and set Flash latency.
  // See here for explanation: https://electronics.stackexchange.com/a/125139
  FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;

  // Select the PLL as the system clock source, 0x10: PLL selected as SYSCLK.
  RCC->CFGR &= ~RCC_CFGR_SW_Msk;
  RCC->CFGR |= RCC_CFGR_SW_PLL;

  // Wait until system clock switch shows desired status.
  timeout = 1000000;
  do {
    sw_status = RCC->CFGR & RCC_CFGR_SWS_Msk;
    --timeout;
  } while ((sw_status != RCC_CFGR_SWS_PLL) && (timeout > 0));

  /* The microcontroller clock output capability allows the clock to be output
     onto the external MCO pin. There are different clock sources available
    for MCO (9.2.13). Set PA8 as MCO at 48 MHz/16 = 3 MHz. Basically to use
    with an oscilloscope. See datasheet table 14, GPIOA has MCO as SYS_AF
    (AF0) */

  // Select MCO source to be SYSCLK (48 MHz).
  RCC->CFGR &= ~RCC_CFGR_MCO_Msk;
  RCC->CFGR |= RCC_CFGR_MCO_SYSCLK;

  // Set MCO prescaler to /16 (3 MHz).
  RCC->CFGR &= ~RCC_CFGR_MCOPRE_Msk;
  RCC->CFGR |= RCC_CFGR_MCOPRE_DIV16;

  // Enable GPIOA port.
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

  // There is only one pin that has MCO as an alternate function.
  // Select alternate function mode (0b10) for PA8. GPIOA8 is in the high
  // register
  GPIOA->MODER &= ~GPIO_MODER_MODER8_Msk;
  GPIOA->MODER |= (0x02 << GPIO_MODER_MODER8_Pos);

  // Select AF0 for PA8 (MCO output).
  GPIOA->AFR[1] &= ~GPIO_AFRH_AFRH0_Msk;
  GPIOA->AFR[1] |= (0x00 << GPIO_AFRH_AFRH0_Pos);

  // Update System Core Clock Update. Defined in system_stm32f0xx.c. Updates
  // the SystemCoreClock variable according to Clock Register Values.
  SystemCoreClockUpdate();

  // Check variable SystemCoreClock to verify SYSCLK setting.
}

// ////////////////////////////////////////////////////////////////////////////
// Console
// ----------------------------------------------------------------------------

void BspConsoleInit(void) {
  // Enable GPIOA clock.
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

  // Configure PA2 and PA3 as Alternative Function.
  // Clear Mode fields for PA2 and PA3 first.
  GPIOA->MODER &= ~(GPIO_MODER_MODER2_Msk | GPIO_MODER_MODER3_Msk);

  // Set 0b10: Alternate function mode.
  GPIOA->MODER |=
      ((0x02 << GPIO_MODER_MODER2_Pos) | (0x02 << GPIO_MODER_MODER3_Pos));

  // Select AF7 for both PA2 and PA3 (AFR low register).
  GPIOA->AFR[0] &= ~(GPIO_AFRL_AFRL2_Msk | GPIO_AFRL_AFRL3_Msk);
  GPIOA->AFR[0] |=
      ((0x07 << GPIO_AFRL_AFRL2_Pos) | (0x07 << GPIO_AFRL_AFRL3_Pos));

  // Per default, the USART2 uses the PCLK (9.4.13), which is the APB1 Clock.
  // APB1 is limited to 36 MHz. USART2SW to (0b01) to select SYSCLK as source.
  RCC->CFGR3 &= ~RCC_CFGR3_USART2SW_Msk;
  // RCC->CFGR3 |= RCC_CFGR3_USART2SW_SYSCLK;
  RCC->CFGR3 |= RCC_CFGR3_USART2SW_PCLK;

  // Enable the USART2 clock.
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

  // Reset USART2 configuration registers.
  // Equivalent to 8-bit, 1 start, 1 stop bit, CTS/RTS disabled.
  USART2->CR1 = 0x00000000;
  USART2->CR1 = 0x00000000;
  USART2->CR1 = 0x00000000;

  /**
   * Baud rate: 115200
   * section 29.5.4:
   * Given:
   *   f_CK = 24 MHz (APB1)
   *   OVER8 = 0 (oversampling by 16)
   *   Baud_rate_desired = 115200
   *   --> USARTDIV = 24e6 / 115200 = 208.333 -> BRR = 208
   *       BRR = 208 --> Baud Rate = 115384.615, 0.16% error.
   *
   * Given:
   *   f_CK = 24 MHz
   *   OVER8 = 1 (oversampling by 8)
   *   Baud_rate_desired = 115200
   *   --> USARTDIV = 2 * 24e6 / 115200 = 416.666 -> BRR = 417
   *       BRR = 417 --> Baud Rate = 115107.914, 0.08% error.
   */
  USART2->CR1 |= USART_CR1_OVER8;
  USART2->BRR = 417;

  // Enable both Transmitter and Receiver.
  USART2->CR1 |= (USART_CR1_TE | USART_CR1_RE);

  // Enable USART2.
  USART2->CR1 |= USART_CR1_UE;
}

uint8_t BspConsolePutChar(char char_to_send) {
  // A character could be written into the transmit data register when the flag
  // TXE is 1. But for single byte use the TC transmit complete flag.
  // Wait until the transmission is idle or complete before writing data.
  while ((USART2->ISR & USART_ISR_TC_Msk) != USART_ISR_TC)
    ;

  // Write data into register to be written to bus.
  USART2->TDR = (uint32_t)char_to_send;

  return 0;
}

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

// ////////////////////////////////////////////////////////////////////////////
// Nested Vector Interrupt Controller setup
// ----------------------------------------------------------------------------

void BspNvicInit(void) {
  // Enable TIM6 interrupt.
  NVIC_EnableIRQ(TIM6_DAC_IRQn);

  // Enable USART2 global interrupt (name defined in stm32f091xc.h line 106).
  NVIC_EnableIRQ(USART2_IRQn);

  // Set priority level for TIM6.
  NVIC_SetPriority(TIM6_DAC_IRQn, 1);

  // Set USART2 global interrupt priority.
  NVIC_SetPriority(USART2_IRQn, 2);
}