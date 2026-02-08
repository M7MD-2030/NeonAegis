cmake_minimum_required(VERSION 3.21)

# Project Name (Matches the game code I gave you)
project(NeonAegis)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- DEPENDENCY HANDLING ---
# This block makes the build robust. 
# It tries to find installed Raylib. If not found, it downloads it automatically.

include(FetchContent)

find_package(raylib QUIET)

if (NOT raylib_FOUND)
    message(STATUS "Raylib not found locally. Downloading from GitHub...")
    FetchContent_Declare(
        raylib
        URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
    )
    FetchContent_MakeAvailable(raylib)
endif()

# --- SOURCE FILES ---
# Defines where your code lives
set(SOURCES src/main.cpp)

# --- EXECUTABLE ---
add_executable(${PROJECT_NAME} ${SOURCES})

# --- LINKING ---
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# --- OS SPECIFIC TWEAKS ---

# macOS: Often requires specific frameworks even if Raylib is linked
if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework OpenGL")
endif()

# Windows: Logic to hide the console window if you want a release build
if(WIN32)
    # Uncomment the next line if you don't want the black command prompt appearing behind the game
    # set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE ON)
endif()

# Copy assets folder to build directory (if you add images/sounds later)
# file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})