# STM32F303RE with FreeRTOS
STM32 template with FreeRTOS bare project setup:
- bare (no HAL or LL)
- CMSIS headers available
- System clock setup
- CMake meta-build system with Ninja and ARM cross toolchain
- Reference: [1. First FreeRTOS project](https://pomad.cnfm.fr/PoMAD_2021/node/36)
## Configuration package
- [STM32F3 MCU package, v1.11.3](https://github.com/STMicroelectronics/STM32CubeF3/archive/refs/tags/v1.11.3.tar.gz)

## Prepare environment
- project root `CMakeLists.txt`
    * project(f303re_freertos LANGUAGES C CXX ASM)
    * If necessary, set HSE frequency.
- `launch.json`
    * adapt `"executable"` path.
- `tasks.json`
    * adapt `"'program build/f303re_freertos.elf verify reset exit'"` under the flash task.

## Download FreeRTOS
```sh
# latest seems to focus on zip, but there is a tarball_url field.
cd /tmp
curl -s https://api.github.com/repos/FreeRTOS/FreeRTOS/releases/latest | grep "tarball_url" | cut -d: -f2,3 | cut -d'"' -f2 | xargs -I {} curl -Lo /tmp/freertos.tar.gz {}


# LTS version:
curl -s https://api.github.com/repos/FreeRTOS/FreeRTOS-LTS/releases/latest | grep "tarball_url" | cut -d: -f2,3 | cut -d'"' -f2 | wget -qi -
```
Somehow the Source folder in the tarball is empty, therefore download the zip.
https://github.com/FreeRTOS/FreeRTOS/releases/latest
https://github.com/FreeRTOS/FreeRTOS/releases/download/202112.00/FreeRTOSv202112.00.zip

## Copy files
mkdir cmsis

| `<extracted package> folder` | File(s) | project folder |
|------------------------------|---------|------------------|
| /Drivers/CMSIS/Include/ | *.h     | /cmsis/core/ |
| /Drivers/CMSIS/Device/ST/STM32F3xx/Include/ | stm32f3xx.h, stm32f303xe.h, system_stm32f3xx.h |  /cmsis/device/include/ |
| /Drivers/CMSIS/Device/ST/STM32F3xx/Source/Templates/gcc/ | startup_stm32f303xe.s  | /cmsis/device/ |
| /Projects/STM32F303RE-Nucleo/Templates/Src | system_stm32f3xx.c | /cmsis/device/ |
| /Projects/STM32F303RE-Nucleo/Templates/Inc | stm32f3xx_it.h, main.h | /app/include/ |
| /Projects/STM32F303RE-Nucleo/Templates/Src | stm32f3xx_it.c | /app/src/ |
| ~Downloads/FreeRTOS-*/.../Source               | `Source/*` | /FreeRTOS/ |
| Downloads/FreeRTOS-*/.../Demo/CORTEX_M4F_STM32F407ZG-SK | FreeRTOSConfig.h | app/include/ |


## Modifications
1. app/stm32f3xx_it.c
    * Comment out `HAL_IncTick()` in the `SysTick_Handler`, as HAL is not part of this template.
2. app/include/main.h
    * Replace `#include "stm32f3xx_hal.h"` with `#include "stm32f3xx.h"`
3. cmsis/device/include/stm32f3xx.h
    * You need to select the actual MCU, therefore uncomment the corresponding line, i.e. `#define STM32F303xE`
4. FreeRTOS, delete
    * FreeRTOS/portable/readme.txt
    * FreeRTOS/include/stdint.readme
    * FreeRTOS/portable: remove everything **except**
        * FreeRTOS/portable/GCC/ARM_CM4F (the ARM_CM4_MPU stands for Memory Protection Unit, a hardware feature not present on the board)
        * FreeRTOS/portable/MemMang/heap_1.c
5. Add new includes to CMakeLists.txt
    * FreeRTOS/include
    * FreeRTOS/portable/GCC/ARM_CM4F
6. in stm32f3xx_it.c
    * Comment out `void SVC_Handler(void) {}`
    * Comment out `void PendSV_Handler(void) {}`
    * Comment out `void SysTick_Handler(void) {...}`
    These interrupt handlers are used by FreeRTOS.
7. in FreeRTOS/portable/GCC/ARM_CM4F/portmacro.h
    * `#include "FreeRTOSConfig.h"`
    * `#include "projdefs.h"`
    * `#include "stm32f3xx.h"`
    TODO: unclear if this intended.
### FreeRTOSConfig.h
```c
#define configUSE_PREEMPTION			1
#define configUSE_IDLE_HOOK				0
#define configUSE_TICK_HOOK				0
#define configCPU_CLOCK_HZ				( 48000000UL )
#define configTICK_RATE_HZ				( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES			( 5 )
#define configMINIMAL_STACK_SIZE		( ( unsigned short ) 70 )
#define configTOTAL_HEAP_SIZE			( ( size_t ) ( 25 * 1024 ) )
#define configMAX_TASK_NAME_LEN			( 10 )
#define configUSE_TRACE_FACILITY		0
#define configUSE_16_BIT_TICKS			0
#define configIDLE_SHOULD_YIELD			1
#define configUSE_MUTEXES						1
#define configQUEUE_REGISTRY_SIZE		0
#define configCHECK_FOR_STACK_OVERFLOW	0
#define configUSE_RECURSIVE_MUTEXES		0
#define configUSE_MALLOC_FAILED_HOOK	0
#define configUSE_APPLICATION_TASK_TAG	0
#define configUSE_COUNTING_SEMAPHORES	1
#define configGENERATE_RUN_TIME_STATS	0
```



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