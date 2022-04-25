# STM32F303RE
## Configuration
- [STM32F3 MCU package, v1.11.3](https://github.com/STMicroelectronics/STM32CubeF3/archive/refs/tags/v1.11.3.tar.gz)

## Copy files
| `<extracted package> folder` | File(s) | project folder |
|------------------------------|---------|------------------|
| /Drivers/CMSIS/Include/ | *.h     | /cmsis/core/ |
| /Drivers/CMSIS/Device/ST/STM32F3xx/Include/ | stm32f3xx.h, stm32f303xe.h, system_stm32f3xx.h |  /cmsis/device/include/ |
| /Drivers/CMSIS/Device/ST/STM32F3xx/Source/Templates/gcc/ | startup_stm32f303xe.s  | /cmsis/device/startup/ |
| /Projects/STM32F303RE-Nucleo/Templates/Src | system_stm32f3xx.c | /cmsis/device/ |
| /Projects/STM32F303RE-Nucleo/Templates/Inc | stm32f3xx_it.h, main.h | /app/inc/ |
| /Projects/STM32F303RE-Nucleo/Templates/Src | stm32f3xx_it.c | /app/src/ |

## Modifications
1. app/stm32f3xx_it.c
    * Comment out `HAL_IncTick()` in the `SysTick_Handler`, as HAL is not part of this template.
2. app/include/main.h
    * Replace `#include "stm32f3xx_hal.h"` with `#include "stm32f3xx.h"`
3. cmsis/device/include/stm32f3xx.h
    * You need to select the actual MCU, therefore uncomment the corresponding line, i.e. `#define STM32F303xE`

## Manual commands when using Makefiles directly
In project folder:
- `make` to build the project
- `compiledb make -j8` to build the project including compile_commands.json
- see tasks.json for convenience