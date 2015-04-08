cmake_minimum_required(VERSION 2.8)

################################## PROJECT ##################################

project(walker-simulator-2015)
set(WS2015_VERSION_MAJOR 0)
set(WS2015_VERSION_MINOR 9)
set(WS2015_VERSION_PATCH 0)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -march=native")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Realease)
    message(STATUS "Release mode active")
endif()

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
file(MAKE_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}")

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
file(MAKE_DIRECTORY "${LIBRARY_OUTPUT_PATH}")

set(AUTOGEN_OUTPUT_PATH "${CMAKE_BINARY_DIR}/autogen")
file(MAKE_DIRECTORY "${AUTOGEN_OUTPUT_PATH}")

################################## PACKAGES ##################################

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules")

find_package(LibRT REQUIRED)

################################ CONFIG FILES ################################

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

################################## COMPILER ##################################

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -O3 -march=native")

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG -Wall -Wextra -pedantic -Wfloat-equal -Wswitch-default -Winit-self -Wshadow -Wbad-function-cast -Wcast-align -Wconversion -Wlogical-op -Wstrict-prototypes -Wnested-externs")
endif()

################################# EXECUTABLE #################################

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
add_executable(walker-simulator-2015
    "${CMAKE_CURRENT_SOURCE_DIR}/src/walker/main.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/walker/request.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/walker/active.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/walker/tools.c"
    )
target_link_libraries(walker-simulator-2015 "${LibRT_LIBRARIES}")
target_link_libraries(walker-simulator-2015 "-lreadline")

################################### DOXYGEN ###################################

find_package(Doxygen QUIET)

if(DOXYGEN_FOUND)
    message(STATUS "Doxygen found, doc target available")
    set(DOXYGEN_DOXYFILE "${AUTOGEN_OUTPUT_PATH}/Doxyfile")
    set(DOXYGEN_OUTPUT_DIR "${CMAKE_BINARY_DIR}/doc")
    set(DOXYGEN_INPUT_DIRS
        "${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/include")
    set(DOXYGEN_OUTPUT_FILES
        "${DOXYGEN_OUTPUT_DIR}/html/index.xhtml"
        "${DOXYGEN_OUTPUT_DIR}/latex/Makefile"
        )

    if(CMAKE_BUILD_TYPE MATCHES Release)
        set(DOXYGEN_WARNINGS "NO")
    else()
        set(DOXYGEN_WARNINGS "YES")
    endif()

    file(MAKE_DIRECTORY "${DOXYGEN_OUTPUT_DIR}")

    if(DOXYGEN_DOT_FOUND)
        message(STATUS "DOT found, generating Doxygen DOT graphics")
        set(DOXYGEN_HAVE_DOT "YES")
    else()
        set(DOXYGEN_HAVE_DOT "NO")
    endif()

    configure_file(
        "${PROJECT_SOURCE_DIR}/config/Doxyfile.in"
        "${AUTOGEN_OUTPUT_PATH}/Doxyfile"
        IMMEDIATE @ONLY
        )

    add_custom_command(OUTPUT ${DOXYGEN_OUTPUT_FILES}
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_DOXYFILE}
        MAIN_DEPENDENCY ${DOXYGEN_DOXYFILE}
        COMMENT "Generating Doxygen doc"
        )
    add_custom_target(doc DEPENDS ${DOXYGEN_OUTPUT_FILES})

else()
    message(STATUS "Doxygen not found, doc target not available")
endif()

################################### INSTALL ###################################

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/man/man1/walker-simulator-2015.1"
    DESTINATION "share/man/man1"
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
    )

install(TARGETS walker-simulator-2015
    DESTINATION "bin"
    )

################################## UNINSTALL ##################################

configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY
    )

add_custom_target(uninstall
    COMMAND "${CMAKE_COMMAND}" -P
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
