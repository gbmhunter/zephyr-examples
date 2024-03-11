cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr)
project(my_zephyr_app)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

target_sources(app PRIVATE src/main.cpp)
target_sources(app PRIVATE src/Led.cpp)
target_sources(app PRIVATE src/StateMachine.cpp)