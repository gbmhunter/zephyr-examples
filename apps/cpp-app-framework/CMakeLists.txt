cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -std=c++14 -fno-threadsafe-statics")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++14 -fno-threadsafe-statics")


find_package(Zephyr)
zephyr_library_compile_options(-std=c++14 -fno-threadsafe-statics)
project(my_zephyr_app)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

target_sources(app PRIVATE src/main.cpp)
target_sources(app PRIVATE src/Led.cpp)
target_sources(app PRIVATE src/StateMachine.cpp)
