# STM32F303RE
STM32 template project setup:
- bare (no HAL or LL)
- CMSIS headers available
- System clock setup
- CMake meta-build system with Ninja and ARM cross toolchain
## Configuration package
- [STM32F3 MCU package, v1.11.3](https://github.com/STMicroelectronics/STM32CubeF3/archive/refs/tags/v1.11.3.tar.gz)

## Copy files
mkdir cmsis

| `<extracted package> folder` | File(s) | project folder |
|------------------------------|---------|------------------|
| /Drivers/CMSIS/Include/ | *.h     | /cmsis/core/ |
| /Drivers/CMSIS/Device/ST/STM32F3xx/Include/ | stm32f3xx.h, stm32f303xe.h, system_stm32f3xx.h |  /cmsis/device/include/ |
| /Drivers/CMSIS/Device/ST/STM32F3xx/Source/Templates/gcc/ | startup_stm32f303xe.s  | /cmsis/device/ |
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

## CMake configure and build
There are two ways to use CMake in VS code:
- code-workspace settings and cmake-kits.json are defined to use the VS code toolbar (CMake-Tools)
- tasks.json and launch.json are configured to use CLI `cmake` without VS code extensions.

## CMake setup
- [Best practices](https://indico.jlab.org/event/420/contributions/7961/attachments/6507/8734/CMakeSandCroundtable.slides.pdf):
- In project folder:
    ```sh
    # Configure CMake.
    cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=/workspaces/stm32/.devcontainer/custom-scripts/gcc-arm-none-eabi.cmake -G Ninja -DTOOLCHAIN_PATH:STRING=/opt/gcc-arm-11.2-2022.02-x86_64-arm-none-eabi -DTARGET_TOOLCHAIN:STRING=arm-none-eabi

    # Build project.
    cmake --build build
    ```
- See tasks.json for convenience.