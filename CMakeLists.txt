cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME Sentry)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C CXX ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Add CMSIS-DSP / arm_math
set(CMSIS_DSP_ROOT ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP)

add_library(cmsis_dsp STATIC
    ${CMSIS_DSP_ROOT}/Source/BasicMathFunctions/BasicMathFunctions.c
    ${CMSIS_DSP_ROOT}/Source/FastMathFunctions/FastMathFunctions.c
    ${CMSIS_DSP_ROOT}/Source/MatrixFunctions/MatrixFunctions.c
    ${CMSIS_DSP_ROOT}/Source/StatisticsFunctions/StatisticsFunctions.c
    ${CMSIS_DSP_ROOT}/Source/SupportFunctions/SupportFunctions.c
    ${CMSIS_DSP_ROOT}/Source/ControllerFunctions/ControllerFunctions.c
    ${CMSIS_DSP_ROOT}/Source/CommonTables/CommonTables.c
)

target_include_directories(cmsis_dsp PUBLIC
    ${CMSIS_DSP_ROOT}/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include
)

target_compile_definitions(cmsis_dsp PUBLIC
    ARM_MATH_CM7
    ARM_MATH_LOOPUNROLL
)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    User_File/1_Middleware/Driver/TIM/drv_tim.cpp
    User_File/5_Task/tsk_config_and_callback.cpp
    User_File/2_Device/BSP/Buzzer/bsp_buzzer.cpp
    User_File/2_Device/BSP/Buzzer/bsp_BuzzerSongs.cpp
    User_File/2_Device/BSP/Key/bsp_arkey.cpp
    User_File/1_Middleware/Driver/UART/drv_uart.cpp
    User_File/1_Middleware/System/Timestamp/sys_timestamp.cpp
    User_File/2_Device/Plotter/Serialplot/dvc_serialplot.cpp
    User_File/1_Middleware/Algorithm/Basic/alg_basic.cpp
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    User_File/1_Middleware/Driver/TIM
    User_File/5_Task
    User_File/2_Device/BSP/Buzzer
    User_File/2_Device/BSP/Key
    User_File/1_Middleware/Driver/UART
    User_File/1_Middleware/System/Timestamp
    User_File/2_Device/Plotter/Serialplot
    User_File/1_Middleware/Algorithm/Basic
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)
list(REMOVE_ITEM CMAKE_CXX_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    cmsis_dsp
    # Add user defined libraries
)
