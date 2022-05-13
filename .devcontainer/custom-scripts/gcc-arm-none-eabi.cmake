# =============================================================================
# CMake cross toolchain information file.
# -----------------------------------------------------------------------------
# References:
# https://github.com/ObKo/stm32-cmake/
# https://github.com/MaJerle/stm32-cube-cmake-vscode

# This should be set automatically when specifying the system name.
# https://cmake.org/cmake/help/latest/variable/CMAKE_CROSSCOMPILING.html
set(CMAKE_CROSSCOMPILING "TRUE")

# ##   Host System Definitions   ##############################################
# https://cmake.org/cmake/help/latest/variable/CMAKE_HOST_SYSTEM.html
# Composite Name of OS CMake is being run on.
set(CMAKE_HOST_SYSTEM           "Linux-5.4.0-109-generic")
# uname -s
set(CMAKE_HOST_SYSTEM_NAME      "Linux")
# uname -r
set(CMAKE_HOST_SYSTEM_VERSION   "5.4.0-109-generic")
# uname -m
set(CMAKE_HOST_SYSTEM_PROCESSOR "x86_64")

# ##   Target System Definitions   ############################################
# https://cmake.org/cmake/help/latest/variable/CMAKE_SYSTEM.html
# Composite name of operating system CMake is compiling for.
set(CMAKE_SYSTEM                Generic)
set(CMAKE_SYSTEM_NAME           Generic)
set(CMAKE_SYSTEM_VERSION        "")
set(CMAKE_SYSTEM_PROCESSOR      arm)


# ##   Toolchain Definitions   ################################################
# set(TOOLCHAIN_PREFIX arm-none-eabi-)
set(TOOLCHAIN_SYSROOT  "${TOOLCHAIN_PATH}/${TARGET_TOOLCHAIN}")
set(TOOLCHAIN_BIN_PATH "${TOOLCHAIN_PATH}/bin")
set(TOOLCHAIN_INC_PATH "${TOOLCHAIN_PATH}/${TARGET_TOOLCHAIN}/include")
set(TOOLCHAIN_LIB_PATH "${TOOLCHAIN_PATH}/${TARGET_TOOLCHAIN}/lib")

message(STATUS "toolchain sysroot: ${TOOLCHAIN_SYSROOT}")
message(STATUS "toolchain bin: ${TOOLCHAIN_BIN_PATH}")
message(STATUS "toolchain include: ${TOOLCHAIN_INC_PATH}")
message(STATUS "toolchain lib: ${TOOLCHAIN_LIB_PATH}")

find_program(CMAKE_C_COMPILER NAMES ${TARGET_TOOLCHAIN}-gcc HINTS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_CXX_COMPILER NAMES ${TARGET_TOOLCHAIN}-g++ HINTS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_ASM_COMPILER NAMES ${TARGET_TOOLCHAIN}-gcc HINTS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_OBJCOPY NAMES ${TARGET_TOOLCHAIN}-objcopy HINTS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_OBJDUMP NAMES ${TARGET_TOOLCHAIN}-objdump HINTS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_SIZE NAMES ${TARGET_TOOLCHAIN}-size HINTS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_DEBUGGER NAMES ${TARGET_TOOLCHAIN}-gdb HINTS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_CPPFILT NAMES ${TARGET_TOOLCHAIN}-c++filt HINTS ${TOOLCHAIN_BIN_PATH})


set(CMAKE_EXECUTABLE_SUFFIX_ASM     ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C       ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX     ".elf")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)