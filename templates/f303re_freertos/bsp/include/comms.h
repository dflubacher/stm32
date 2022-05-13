/**
 * @file comms.h
 * @author DFlubacher
 * @brief API for I2c, SPI
 * @version 0.1
 * @date 2022-05-01
 * References:
 * - https://pomad.cnfm.fr/PoMAD_2021/node/22
 *
 */

#ifndef BSP_INCLUDE_COMMS_H_
#define BSP_INCLUDE_COMMS_H_

#include <stdint.h>

/**
 * @brief I2C initialization.
 * Pins: PB8, PB9, open-drain configuration
 * Board: PB8 -> CN10-3 and CN5-10 (label SCL/D15).
 *        PB9 -> CN10-5 and CN5-9  (label SDA/D14).
 * source CLK: SYSCLK 48 MHz.
 * I2C CLK: standard mode, 100kHz.
 */
void BspI2C1_Init(void);

/**
 * @brief Read `nbytes` of register `register_address` from device with address
 *        `device_address` and store it in `buffer`.
 *
 *        Returns 1 if device address fails. Check wiring and receiver address.
 *        Returns 2 if register address fails.
 *        Returns 3 if reception fails.
 *        Returns 4 if Stop detection fails.
 *
 * @param device_address
 * @param register_address
 * @param buffer
 * @param nbytes
 * @return uint8_t
 */
uint8_t BspI2C1_Read(uint8_t device_address, uint8_t register_address,
                     uint8_t* buffer, uint8_t nbytes);

/**
 * @brief Write `nbytes` from `buffer` to the register `register_address` of
 *        device with address `device_address`.
 *
 * @param device_address
 * @param register_address
 * @param buffer
 * @param nbytes
 * @return uint8_t
 */
uint8_t BspI2C1_Write(uint8_t device_address, uint8_t register_address,
                      uint8_t* buffer, uint8_t nbytes);

/**
 * @brief SPI initialization.
 * Board:
 * - SCK:  D13 --> PA5 (AF5, SPI1_SCK)
 * - MISO: D12 --> PA6 (AF5, SPI1_MISO)
 * - MOSI: D11 --> PA7 (AF5, SPI1_MOSI)
 * - CS:   D10 --> PB6 (GPO)
 */
void BspSPI1_Init(void);

/**
 * @brief Read Transaction
 * Tell the slave what operation to perform. First byte with:
 * - slave register address (7-bit right aligned).
 * - Nature of operation READ/WRITE (bit 7).
 * - optional bits depending on slave-specific protocol.
 *
 * All SPI data transactions pass through the 32-bit embedded FIFOs.
 * For receiving, check RXNE, indicating if FIFO buffer is not empty (defined by
 * threshold). For transmitting, check TXE, indicating if FIFO buffer is
 * considered as full (defined by FRXTH).
 * TXE: TX Empty, you can add data.
 * RXNE: RX Not Empty, you can read data.
 * @param tx_byte
 * @return byte received
 */
uint8_t BspSPI1_SendReceive(uint8_t tx_byte);

uint8_t BspSPI1_BMP390_Read(uint8_t register_address, uint8_t length,
                            uint8_t* response);

uint16_t BspSPI1_SendReceive16(uint16_t tx_byte);

#endif /* BSP_INCLUDE_COMMS_H_ */