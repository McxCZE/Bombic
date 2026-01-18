cmake_minimum_required(VERSION 3.16)

project(Bombic
    VERSION 2.0.0
    DESCRIPTION "Bombic - Classic Bomberman-style game"
    LANGUAGES CXX
)

# C++11 standard (minimum for nullptr, override, etc.)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build type - default to Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find required SDL2 packages
find_package(PkgConfig REQUIRED)

pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)
pkg_check_modules(SDL2_NET SDL2_net)

# Check if SDL2_net was found
if(SDL2_NET_FOUND)
    set(HAVE_SDL2_NET ON)
    message(STATUS "SDL2_net found - LAN multiplayer enabled")
else()
    set(HAVE_SDL2_NET OFF)
    message(STATUS "SDL2_net NOT found - LAN multiplayer disabled")
    message(STATUS "  To enable LAN multiplayer, install SDL2_net:")
    message(STATUS "    Arch/Manjaro: sudo pacman -S sdl2_net")
    message(STATUS "    Debian/Ubuntu: sudo apt install libsdl2-net-dev")
    message(STATUS "    Fedora: sudo dnf install SDL2_net-devel")
endif()

# Generate config.h
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)

# Add source directory
add_subdirectory(src)

# Install data files
install(DIRECTORY data/
    DESTINATION share/bombic
    FILES_MATCHING
    PATTERN "*.bmp"
    PATTERN "*.jpg"
    PATTERN "*.wav"
    PATTERN "*.ble"
    PATTERN "*.txt"
    PATTERN "*.ttf"
    PATTERN "Makefile*" EXCLUDE
)

# Install desktop file and icon (optional)
if(UNIX AND NOT APPLE)
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/cmake/bombic.desktop"
        DESTINATION share/applications
        OPTIONAL
    )
endif()

# Print configuration summary
message(STATUS "")
message(STATUS "Bombic ${PROJECT_VERSION} configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "")
