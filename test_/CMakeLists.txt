cmake_minimum_required(VERSION 3.17)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
 
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(SIZE arm-none-eabi-size)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
 
# project settings, use floader name (name in CubeMX) as progect name
string(REGEX MATCH "[^\/]+$" BUFF_FOR_PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR})
project(${BUFF_FOR_PROJECT_NAME} C CXX ASM)
 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)
 
# read Makefile for parsing
file(READ ${CMAKE_SOURCE_DIR}/Makefile MAKEFILE_BUFF)
 
# parsing for cortex carnel name
string(REGEX MATCH "-mcpu=[^\n]*" MCPU ${MAKEFILE_BUFF})
string(REGEX MATCH "[^=]*$" MCPU ${MCPU})
 
# Uncomment for hardware floating point
#add_compile_definitions(ARM_MATH_CM4;ARM_MATH_MATRIX_CHECK;ARM_MATH_ROUNDING)
#add_compile_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)
#add_link_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)
 
# Uncomment for software floating point
add_compile_options(-mfloat-abi=soft)
 
add_compile_options(-mcpu=${MCPU} -mthumb -mthumb-interwork)
add_compile_options(-ffunction-sections -fdata-sections -fno-common -fmessage-length=0)
 
# Uncomment to mitigate c++17 absolute addresses warnings
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-register")
 
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(STATUS "Maximum optimization for speed")
    add_compile_options(-Ofast)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    message(STATUS "Maximum optimization for speed, debug info included")
    add_compile_options(-Ofast -g)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    message(STATUS "Maximum optimization for size")
    add_compile_options(-Os)
else ()
    message(STATUS "Minimal optimization, debug info included")
    add_compile_options(-Og -g)
endif ()
 
# parsing for include dir
string(REGEX MATCHALL "-I[^\ \n]*" INCLUDE_DIRS_RAW ${MAKEFILE_BUFF})
foreach(INCLUDE_DIR ${INCLUDE_DIRS_RAW})
    string(REPLACE "-I" "${CMAKE_SOURCE_DIR}/" INCLUDE_DIR ${INCLUDE_DIR})
    string(REPLACE ";" "\n" INCLUDE_DIR ${INCLUDE_DIR})
    list(APPEND INCLUDE_DIRS ${INCLUDE_DIR})
endforeach()
 
include_directories(
    ${INCLUDE_DIRS}
    # You can add your's dir with heders
)
 
# parsing for definitions
string(REGEX MATCHALL "-D[^\ \n]*" DEFINITIONS_PARSE ${MAKEFILE_BUFF})
add_definitions(${DEFINITIONS_PARSE})
 
# parsing for source dir
file(STRINGS ${CMAKE_SOURCE_DIR}/Makefile MAKEFILE_BUFF)
foreach(A ${MAKEFILE_BUFF})
    if (A MATCHES "C_SOURCES =[^\n]*")
        string(REGEX MATCH "  .+$" A ${A})
        string(REGEX MATCHALL "[^ ]+" A ${A})
        set(SOURCE_FILES_BUF ${A})
    endif()
endforeach()
 
# find startap file
file(GLOB LINKER "startup_*.s")
message(${LINKER})
 
file(GLOB SOURCES
    ${SOURCE_FILES_BUF}
    ${LINKER}
    # You can add your's sorce file
    "Core/Src/rgb_led.c"
)
 
# find lincker script
file(GLOB LINKER_SCRIPT "STM32*.ld")
 
# this options for C++
add_link_options(-specs=nosys.specs -lstdc++)
add_link_options(-Wl,-gc-sections,--print-memory-usage,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)
add_link_options(-mcpu=${MCPU} -mthumb -mthumb-interwork)
add_link_options(-T ${LINKER_SCRIPT})
 
add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})
 
set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)
 
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}")