/**
 * @file comms.c
 * @author DFlubacher
 * @brief I2C, SPI
 * @version 0.1
 * @date 2022-05-01
 *
 */

#include "comms.h"

#include <stdint.h>

#include "stm32f303xe.h"
#include "stm32f3xx.h"

void BspI2C1_Init(void) {
  // Enable GPIOB:
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

  // Configure PB8 and PB9 for alternate function mode (0b10).
  GPIOB->MODER &= ~(GPIO_MODER_MODER8_Msk | GPIO_MODER_MODER9_Msk);
  GPIOB->MODER |=
      (0x02 << GPIO_MODER_MODER8_Pos | 0x02 << GPIO_MODER_MODER9_Pos);

  // Configure AF4 for PB8 and PB9 (I2C1_SCL and I2C1_SDA).
  GPIOB->AFR[1] &= ~(GPIO_AFRH_AFRH0_Msk | GPIO_AFRH_AFRH1_Msk);
  GPIOB->AFR[1] |= (0x04 << GPIO_AFRH_AFRH0_Pos | 0x04 << GPIO_AFRH_AFRH1_Pos);

  // Setup open-drain output configuration for PB8 and PB9.
  GPIOB->OTYPER |= (GPIO_OTYPER_OT_8 | GPIO_OTYPER_OT_9);

  // I2C can be clocked by SYSCLK or external clock, see figure 13. Select
  // SYSCLK.
  RCC->CFGR3 |= RCC_CFGR3_I2C1SW_SYSCLK;

  // Make sure I2C1 is disabled.
  I2C1->CR1 &= ~I2C_CR1_PE;

  // Enable I2C1 clock.
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

  // Reset I2C configuration and timing registers.
  I2C1->CR1 = 0x00000000;
  I2C1->CR2 = 0x00000000;
  I2C1->TIMINGR = 0x00000000;

  // Set prescaler to /4 to get 12 MHz. Refer to 28.7.5, PRESC + 1 will be
  // used.
  I2C1->TIMINGR |= (0x03 << I2C_TIMINGR_PRESC_Pos);

  // 100 kHz --> 10 us, i.e. 5 us LOW, 5 us HIGH.
  // Or just simple: 12 MHz is 120 x 100 kHz, 60 high, 60 low.
  // t_PRESC = (PRESC + 1) * t_I2CCLK.
  I2C1->TIMINGR |=
      (0x3C << I2C_TIMINGR_SCLL_Pos | 0x3C << I2C_TIMINGR_SCLH_Pos);

  // Enable I2C1.
  I2C1->CR1 |= I2C_CR1_PE;
}

uint8_t BspI2C1_Read(uint8_t device_address, uint8_t register_address,
                     uint8_t* buffer, uint8_t nbytes) {
  // Flag no response received.
  uint32_t timeout = 100000;

  // Loop counter.
  uint8_t n = nbytes;

  // Set device address. 7bit aligned left in 8-bit of SADD.
  I2C1->CR2 &= ~I2C_CR2_SADD_Msk;
  I2C1->CR2 |= ((device_address << 1U) << I2C_CR2_SADD_Pos);

  // Set I2C1 to write mode.
  I2C1->CR2 &= ~I2C_CR2_RD_WRN;

  // Configure for transfer of nbytes = 1, with AUTOEND = 0.
  I2C1->CR2 &= ~I2C_CR2_NBYTES_Msk;
  I2C1->CR2 |= (0x01 << I2C_CR2_NBYTES_Pos);
  I2C1->CR2 &= ~I2C_CR2_AUTOEND;

  // Start transmission. Reset by hardware.
  I2C1->CR2 |= I2C_CR2_START;

  // ===   REQUEST   ==========================================================
  // Check if the Transfer Interrupt Status is empty before proceeding.
  while ((I2C1->ISR & I2C_ISR_TXIS_Msk) != I2C_ISR_TXIS) {
    // Check if time is up.
    if (timeout-- == 0) return 1;
  }
  timeout = 100000;

  // Send register address.
  I2C1->TXDR = register_address;

  // Wait until transmission is completed.
  while ((I2C1->ISR & I2C_ISR_TC_Msk) != I2C_ISR_TC) {
    // Check if time is up.
    if (timeout-- == 0) return 2;
  }
  timeout = 1000000;
  // ===   Request completed   ================================================

  // Enter Read mode.
  I2C1->CR2 |= I2C_CR2_RD_WRN;

  // Receive nbytes, AUTOEND = 0.
  I2C1->CR2 &= ~I2C_CR2_NBYTES_Msk;
  I2C1->CR2 |= (nbytes << I2C_CR2_NBYTES_Pos);
  I2C1->CR2 &= ~I2C_CR2_AUTOEND;

  // Start transmission. Reset by hardware.
  I2C1->CR2 |= I2C_CR2_START;

  while (n > 0) {
    // Wait for 'Receive data register not empty' flag.
    while ((I2C1->ISR & I2C_ISR_RXNE_Msk) != I2C_ISR_RXNE) {
      // Check if time is up.
      if (timeout-- == 0) return (240 + n);
    }
    timeout = 100000;

    // Store data in buffer. Reading RXDR clears the RXNE flag.
    *buffer = I2C1->RXDR;
    ++buffer;
    --n;
  }

  // ===   Reception completed   ==============================================

  // Generate STOP condition.
  I2C1->CR2 |= I2C_CR2_STOP;

  // Wait for 'Stop detection flag'.
  while ((I2C1->ISR & I2C_ISR_STOPF_Msk) != I2C_ISR_STOPF) {
    // Check if time is up.
    if (timeout-- == 0) return 4;
  }

  // Return success.
  return 0;
}

uint8_t BspI2C1_Write(uint8_t device_address, uint8_t register_address,
                      uint8_t* buffer, uint8_t nbytes) {
  // Flag no response received.
  uint32_t timeout = 100000;

  // Loop counter.
  uint8_t n = nbytes;

  // Set device address. 7bit aligned left in 8-bit of SADD.
  I2C1->CR2 &= ~I2C_CR2_SADD_Msk;
  I2C1->CR2 |= ((device_address << 1U) << I2C_CR2_SADD_Pos);

  // Set I2C1 to write mode.
  I2C1->CR2 &= ~I2C_CR2_RD_WRN;

  // Configure for transfer of nbytes + address, with AUTOEND = 0.
  I2C1->CR2 &= ~I2C_CR2_NBYTES_Msk;
  I2C1->CR2 |= ((nbytes + 1) << I2C_CR2_NBYTES_Pos);
  I2C1->CR2 |= I2C_CR2_AUTOEND;

  // Clear STOPF flag.
  I2C1->ICR &= ~I2C_ICR_STOPCF_Msk;

  // Start transmission. Reset by hardware.
  I2C1->CR2 |= I2C_CR2_START;

  // Check if the Transfer Interrupt Status is empty before proceeding.
  while ((I2C1->ISR & I2C_ISR_TXIS_Msk) != I2C_ISR_TXIS) {
    // Check if time is up.
    if (timeout-- == 0) return 1;
  }
  timeout = 100000;

  // Send register address.
  I2C1->TXDR = register_address;

  while (n > 0) {
    // Wait for 'Receive data register not empty' flag.
    while ((I2C1->ISR & I2C_ISR_TXIS_Msk) != I2C_ISR_TXIS) {
      // Check if time is up.
      if (timeout-- == 0) return 2;
    }
    timeout = 100000;

    // Send data.
    I2C1->TXDR = *buffer;
    ++buffer;
    --n;
  }

  // ===   Transmission completed ==============================================

  // Wait for 'Stop detection flag'.
  while ((I2C1->ISR & I2C_ISR_STOPF_Msk) != I2C_ISR_STOPF) {
    // Check if time is up.
    if (timeout-- == 0) return 3;
  }

  // Return success.
  return 0;
}

void BspSPI1_Init(void) {
  // Ref: https://pomad.cnfm.fr/PoMAD_2021/node/23
  // ===   Configure PB6 as Chip Select pin   =================================

  // Enable GPIOB clock.
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

  // Configure PB6 as a general purpose output mode (0b01).
  GPIOB->MODER &= ~GPIO_MODER_MODER6_Msk;
  GPIOB->MODER |= (0x01 << GPIO_MODER_MODER6_Pos);

  // Configure PB6 as a push-pull output (0).
  GPIOB->OTYPER &= ~GPIO_OTYPER_OT_6;

  // Disable pull-up/pull-down resistors.
  GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR6_Msk;

  // Configure PB6 as high-speed output (0b11).
  GPIOB->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR6_Msk;
  GPIOB->OSPEEDR |= (0x03 << GPIO_OSPEEDER_OSPEEDR6_Pos);

  // The Chip Select is Normally HIGH (active low), which disconnects the
  // peripheral from the SPI bus.
  // https://learn.sparkfun.com/tutorials/serial-peripheral-interface-spi/chip-select-cs
  GPIOB->BSRR = GPIO_BSRR_BS_6;

  // ===   Configure PA5, PA6 and PA7   =======================================
  // Enable GPIOA clock.
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

  // Configure PA5, PA6, and PA7 in alternate function mode (0b10).
  GPIOA->MODER &=
      ~(GPIO_MODER_MODER5_Msk | GPIO_MODER_MODER6_Msk | GPIO_MODER_MODER7_Msk);

  GPIOA->MODER |=
      ((0x02 << GPIO_MODER_MODER5_Pos) | (0x02 << GPIO_MODER_MODER6_Pos) |
       (0x02 << GPIO_MODER_MODER7_Pos));

  // Configure PA5, PA6, and PA7 to use alternate function 5.
  GPIOA->AFR[0] &=
      ~(GPIO_AFRL_AFRL5_Msk | GPIO_AFRL_AFRL6_Msk | GPIO_AFRL_AFRL7_Msk);
  GPIOA->AFR[0] |=
      ((0x05 << GPIO_AFRL_AFRL5_Pos) | (0x05 << GPIO_AFRL_AFRL6_Pos) |
       (0x05 << GPIO_AFRL_AFRL7_Pos));

  // Enable SPI1 clock.
  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

  // Reset SPI configuration.
  // Results in:
  // - 4-wire, (MISO/MOSI on separate lines) --> full-duplex.
  // - no CRC, MSB first.
  // - CPHA = 0, CPOL = 0 --> SPI mode 0 (first clock rising edge). CK to 0
  //   when idle. Matches u-blox Neo-M9N data sheet.
  SPI1->CR1 = 0x0000;
  SPI1->CR2 = 0x0000;

  // Configure SPI clock. SPI1 is on APB2 clock --> 48 MHz.
  // Baud rate f_CLK/128 (0b110), APB2 --> 375 kHz.
  SPI1->CR1 |= (0x06 << SPI_CR1_BR_Pos);

  // Configure SPI transfer size to 8 bits (data size 0b111).
  SPI1->CR2 |= (0x07 << SPI_CR2_DS_Pos);

  // Set as master (SSI must be high), with software managed NSS.
  // SSI (internal slave select).
  // SSM (software slave management).
  // The CS pin could be used for multi-master management, where the CS/NSS is
  // configured as input (SPI_CR1).
  SPI1->CR1 |= (SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI);

  // Enable SPI1.
  SPI1->CR1 |= SPI_CR1_SPE;
}

uint8_t BspSPI1_SendReceive(uint8_t tx_byte) {
  // uint16_t BspSPI1_SendReceive(uint16_t tx_byte) {
  // Flag no response received.
  uint32_t timeout = 1000000;
  uint8_t rx_byte;
  // uint16_t rx_byte;

  // Make sure the TXE flag is set before writing to the data register.
  // SR: status register.
  while ((SPI1->SR & SPI_SR_TXE_Msk) != SPI_SR_TXE) {
    if (timeout-- == 0) return 1;
  }
  timeout = 1000000;

  // Transmit the byte.
  *(__IO uint8_t*)&SPI1->DR = tx_byte;
  // SPI1->DR = tx_byte;

  // Wait until incoming data has arrived.
  while ((SPI1->SR & SPI_SR_RXNE_Msk) != SPI_SR_RXNE) {
    if (timeout-- == 0) return 2;
  }

  // Read data sent by slave.
  rx_byte = *(__IO uint8_t*)&SPI1->DR;
  // rx_byte = SPI1->DR;

  return rx_byte;
}

uint16_t BspSPI1_SendReceive16(uint16_t tx_byte) {
  // Flag no response received.
  uint32_t timeout = 100000;
  uint16_t rx_byte;

  // Make sure the TXE flag is set before writing to the data register.
  // SR: status register.
  while ((SPI1->SR & SPI_SR_TXE_Msk) != SPI_SR_TXE) {
    if (timeout-- == 0) return 1;
  }
  timeout = 100000;

  // Transmit the byte.
  // *(__IO uint8_t*)&SPI1->DR = tx_byte;
  SPI1->DR = tx_byte;

  // Wait until incoming data has arrived.
  while ((SPI1->SR & SPI_SR_RXNE_Msk) != SPI_SR_RXNE) {
    if (timeout-- == 0) return 2;
  }

  // Read data sent by slave.
  // rx_byte = *(__IO uint8_t*)&SPI1->DR;
  rx_byte = SPI1->DR;

  return rx_byte;
}

uint8_t BspSPI1_BMP390_Read(uint8_t register_address, uint8_t length,
                            uint8_t* response) {
  uint32_t timeout = 100000;
  uint8_t n = length;
  // Set FIFO reception threshold to 1.
  // 1: RXNE event is generated if the FIFO level is greater than or equal to
  // 1/4 (8-bit).
  SPI1->CR2 |= SPI_CR2_FRXTH;

  // Select the slave (CS --> low).
  GPIOB->BSRR = GPIO_BSRR_BR_6;

  // Make sure the TXE flag is set before writing to the data register.
  // SR: status register.
  while ((SPI1->SR & SPI_SR_TXE_Msk) != SPI_SR_TXE) {
    if (timeout-- == 0) return 1;
  }
  timeout = 1000000;

  // Transmit the byte, add the READ bit.
  *(__IO uint8_t*)&SPI1->DR = (register_address | 0x80);
  // SPI1->DR = tx_byte;
  while ((SPI1->SR & SPI_SR_TXE_Msk) != SPI_SR_TXE) {
    if (timeout-- == 0) return 1;
  }
  timeout = 1000000;

  // // Transmit dummy byte.
  *(__IO uint8_t*)&SPI1->DR = 0x42;
  while ((SPI1->SR & SPI_SR_TXE_Msk) != SPI_SR_TXE) {
    if (timeout-- == 0) return 1;
  }
  timeout = 1000000;
  *(__IO uint8_t*)&SPI1->DR = 0x42;

  while (n-- > 0) {
    // Wait until incoming data has arrived.
    while ((SPI1->SR & SPI_SR_RXNE_Msk) != SPI_SR_RXNE) {
      if (timeout-- == 0) return 2;
    }
    timeout = 1000000;
    *response = *(__IO uint8_t*)&SPI1->DR;
    ++response;
  }

  // Release slave (CS --> high).
  GPIOB->BSRR = GPIO_BSRR_BS_6;

  return 0;
}
