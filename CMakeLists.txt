cmake_minimum_required(VERSION 3.16)
project(CarDodgingGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(CarDodgingGame src/main.cpp)
target_link_libraries(CarDodgingGame PRIVATE sfml-graphics sfml-window sfml-system)
