cmake_minimum_required(VERSION 2.8)
project(walker-simulator-2015)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules")

set(WS2015_VERSION_MAJOR 0)
set(WS2015_VERSION_MINOR 0)
set(WS2015_VERSION_PATCH 1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -march=native")

if(CMAKE_BUILD_TYPE MATCHES Debug)
    # cmake adds -g automatically
    set(DEBUG ON)
    message(STATUS "Debug mode active")
    #set(WS2015_DEBUG_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/debug)
    #file(MAKE_DIRECTORY ${WS2015_DEBUG_DIRECTORY})
else()
    set(DEBUG OFF)
endif()

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
file(MAKE_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}")

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
file(MAKE_DIRECTORY "${LIBRARY_OUTPUT_PATH}")

set(AUTOGEN_OUTPUT_PATH "${CMAKE_BINARY_DIR}/autogen")
file(MAKE_DIRECTORY "${AUTOGEN_OUTPUT_PATH}")
# Search for librt

find_package(LibRT REQUIRED)

# config file

find_program(WHOAMI_PROG whoami)
find_program(HOSTNAME_PROG hostname)
find_program(DATE_PROG date)

if(NOT DEFINED USERNAME AND EXISTS "${WHOAMI_PROG}")
    execute_process(COMMAND "${WHOAMI_PROG}"
        OUTPUT_STRIP_TRAILING_WHITESPACE
        OUTPUT_VARIABLE USERNAME)
else()
    set(USERNAME "H&M")
endif()
if(NOT DEFINED HOSTNAME AND EXISTS "${HOSTNAME_PROG}")
    execute_process(COMMAND "${HOSTNAME_PROG}"
        OUTPUT_STRIP_TRAILING_WHITESPACE
        OUTPUT_VARIABLE HOSTNAME)
else()
    set(HOSTNAME "WS-2015")
endif()

if(NOT DEFINED DATE AND EXISTS "${DATE_PROG}")
    execute_process(COMMAND "${DATE_PROG}"
        OUTPUT_STRIP_TRAILING_WHITESPACE
        OUTPUT_VARIABLE DATE)
else()
    set(DATE "1-1-1970")
endif()

configure_file(
    "${PROJECT_SOURCE_DIR}/config/version.h.in"
    "${AUTOGEN_OUTPUT_PATH}/include/walker/version.h")


include_directories("${AUTOGEN_OUTPUT_PATH}/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

add_definitions(-std=gnu11)

if(DEBUG)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG -Wall -Wextra -pedantic -Wfloat-equal -Wswitch-default -Winit-self -Wshadow -Wbad-function-cast -Wcast-align -Wconversion -Wlogical-op -Wstrict-prototypes -Wnested-externs")
endif()

# Executable

add_executable(walker-simulator-2015
    "${CMAKE_CURRENT_SOURCE_DIR}/src/walker/main.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/walker/request.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/walker/active.c"
    )

target_link_libraries(walker-simulator-2015 "${LibRT_LIBRARIES}")

# Install
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/man/man1/walker-simulator-2015.1"
    DESTINATION "share/man/man1"
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
    )

install(TARGETS walker-simulator-2015
    DESTINATION "bin"
    )

# Uninstall

configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY
    )

add_custom_target(uninstall
    COMMAND "${CMAKE_COMMAND}" -P
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
