# =============================================================================
# STM32 CMake
# Author: DF
# -----------------------------------------------------------------------------

# ##   Initialize the project   ###############################################
cmake_minimum_required(VERSION 3.22)

project(f303re_cmake LANGUAGES C CXX ASM)
set(PROJECT_PATH                    ${CMAKE_CURRENT_SOURCE_DIR})
message(">>> BUILD TYPE: "          ${CMAKE_BUILD_TYPE})

# ##   Enable C and ASM using C11 standard   ##################################
enable_language(C CXX ASM)

set(CMAKE_C_STANDARD                11)
set(CMAKE_C_STANDARD_REQUIRED       ON)
set(CMAKE_C_EXTENSIONS              OFF)
set(CMAKE_CXX_STANDARD              17)
set(CMAKE_CXX_STANDARD_REQUIRED     ON)
set(CMAKE_CXX_EXTENSIONS            OFF)

# ##   MCU parameters   #######################################################
# STM32F303RET6
# https://www.mikrocontroller.net/articles/ARM_GCC#Compiler_&_Linker_Flags
set(CFLAGS_MCU
      # Makefile: CFLAGS, mcu
      -mthumb
      # Makefile: CFLAGS, cpu
      -mcpu=cortex-m4
      # Makefile: CFLAGS, fpu
      -mfpu=fpv4-sp-d16
      # Makefile: CFLAGS, float-abi
      -mfloat-abi=hard
      # -mfloat-abi=soft
)

# Set Linker script.
set(LDSCRIPT                        "${PROJECT_PATH}/STM32F303RETX_FLASH.ld")

# ##   Create an executable with above defined list   #########################
set(EXECUTABLE ${PROJECT_NAME})

# ##   Create a variable holding the list of source files   ###################
# Relative to project path.
set(SRC_FILES
      app/main.c
      app/stm32f3xx_it.c

      cmsis/device/system_stm32f3xx.c

      cmsis/device/startup_stm32f303xe.s
)

# ##   Create a variable holding the list of include directories   ############
set(INCLUDE_DIRS
      app/include
      bsp/include
      cmsis/core
      cmsis/device/include
)

# ##   Create a variable holding the list of macro definitions for gcc  #######
set(C_DEFS
      "DEBUG"
      # "USE_HAL_DRIVER"
      "STM32F303xx"
      # External oscillator value
      "HSE_VALUE=8000000"
      # "ARM_MATH_CM4"
      # "ARM_MATH_MATRIX_CHECK"
      # "ARM_MATH_ROUNDING"
)

# ##   Define executable   ####################################################
add_executable(${EXECUTABLE} ${SRC_FILES})

# ##   Configure the compiler's include path   ################################
target_include_directories(${EXECUTABLE} PRIVATE ${INCLUDE_DIRS})

# ##   Add macro definitions   ################################################
target_compile_definitions(${EXECUTABLE} PRIVATE ${C_DEFS})

# ##   Set compiler options (CFlags)   ########################################
target_compile_options(
      ${EXECUTABLE} PRIVATE
      ${CFLAGS_MCU}
      # Makefile: compile gcc flags
      -Wall
      -fdata-sections
      -ffunction-sections
      # OPT
      -Og
      $<$<CONFIG:Debug>:-g -gdwarf-2>
      # Generate dependency information
      # -MMD -MP -MF"$(@:%.o=%.d)"
      # -Wextra
      # -Wpedantic
      # -Wno-unused-parameter

)

# ##   Set the linker's options   #############################################
# Makefile: LDFLAGS
target_link_options(
      ${EXECUTABLE} PRIVATE
      ${CFLAGS_MCU}
      -specs=nano.specs
      # Linker script:
      -T${LDSCRIPT}
      # libraries, LIBS
      -lc
      -lm
      -lnosys
      -Wl,-Map=${PROJECT_NAME}.map,--cref
      -Wl,--gc-sections
)

# ##   Custom commands   ######################################################
# Print executable size
add_custom_command(
      TARGET ${EXECUTABLE}
      POST_BUILD
      COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${EXECUTABLE}>
)

# Create hex file.
add_custom_command(
      TARGET ${EXECUTABLE}
      POST_BUILD
      COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${EXECUTABLE}> ${EXECUTABLE}.hex
      COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${EXECUTABLE}> ${EXECUTABLE}.bin
)

