/**
 * @file printf-stdarg.h
 * @author DFlubacher
 * @brief interface to custom printf, sprintf functions
 * @version 0.1
 * @date 2022-04-26
 *
 */

#ifndef BSP_INCLUDE_PRINTF_STDARG_H_
#define BSP_INCLUDE_PRINTF_STDARG_H_

#include <stdarg.h>

int stm32_printf(const char* format, ...);

int stm32_sprintf(char* out, const char* format, ...);

#endif /* BSP_INCLUDE_PRINTF_STDARG_H_ */