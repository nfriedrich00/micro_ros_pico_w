cmake_minimum_required(VERSION 3.12)

set(PICO_BOARD pico_w CACHE STRING "Board type")

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(micro_ros_pico C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


pico_sdk_init()

link_directories($ENV{PICO_MICRO_ROS_SDK_PATH}/libmicroros)
add_executable(micro_ros_pico
    pico_micro_ros.cpp
    pico_wifi_transport.c
)
target_link_libraries(micro_ros_pico
    microros
    pico_stdlib
    hardware_adc
    pico_cyw43_arch_lwip_threadsafe_background
)

target_include_directories(micro_ros_pico PUBLIC
    $ENV{PICO_MICRO_ROS_SDK_PATH}/libmicroros/include
    ${CMAKE_CURRENT_LIST_DIR}		# test if this and next lineare necessary
    ${CMAKE_CURRENT_LIST_DIR}/..
)


SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections")

# Configure Pico	#check next few lines function
pico_enable_stdio_usb(micro_ros_pico 1)
pico_enable_stdio_uart(micro_ros_pico 0)
add_compile_definitions(PICO_UART_ENABLE_CRLF_SUPPORT=0)
add_compile_definitions(PICO_STDIO_ENABLE_CRLF_SUPPORT=0)
add_compile_definitions(PICO_STDIO_DEFAULT_CRLF=0)

# Generate UF2
pico_add_extra_outputs(micro_ros_pico)

