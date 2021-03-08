cmake_minimum_required(VERSION 3.8)

set(CMAKE_TOOLCHAIN_FILE "common/intros/cmake/config-stm32f4discovery-gcc.cmake")
set(CMAKE_BUILD_TYPE     "Release")

option(__FLASH   "Build and flash" OFF)
option(__QEMU    "Build and emulate with qemu software" OFF)
option(__MONITOR "Build, flash and monitor with hardware by semihosting" OFF)

project(test)

find_package(startup REQUIRED)
find_package(device  REQUIRED)
find_package(intros  REQUIRED)

add_executable(test
	src/main.c
)

target_include_directories(test
	PRIVATE
	include
)

target_link_libraries(test
	PRIVATE
	startup
	device::nosys
	intros::kernel
)

setup_target(test)
