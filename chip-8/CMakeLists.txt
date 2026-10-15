cmake_minimum_required(VERSION 3.20)

project(chip8_final CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 CONFIG REQUIRED)

add_executable(chip8
    src/Main.cpp
    src/Platform.cpp
    src/Chip8.cpp
)

if(TARGET SDL2::SDL2main)
    target_link_libraries(chip8 PRIVATE SDL2::SDL2main)
endif()

target_link_libraries(chip8 PRIVATE SDL2::SDL2)