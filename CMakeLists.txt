cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")
project(tetris VERSION 0.1.0)

add_executable(tetris src/main.cpp)

file(GLOB SOURCES src/*.cpp)
target_sources(tetris PRIVATE ${SOURCES})

find_package(ftxui CONFIG REQUIRED)
target_link_libraries(tetris PRIVATE ftxui::dom ftxui::screen ftxui::component)
