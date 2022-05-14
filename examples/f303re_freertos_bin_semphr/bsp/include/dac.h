/**
 * @file dac.h
 * @author DFlubacher
 * @brief Simple DAC
 * @version 0.1
 * @date 2022-04-28
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef BSP_INCLUDE_DAC_H_
#define BSP_INCLUDE_DAC_H_

#include <stdint.h>

/**
 * @brief Digital to Analog Output.
 * PA4 -> Arduino Connector A2, Morpho Connector CN7-32.
 *
 */
void BspDacInit(void);

#endif /* BSP_INCLUDE_DAC_H_ */