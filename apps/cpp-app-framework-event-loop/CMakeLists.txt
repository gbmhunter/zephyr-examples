cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(my_zephyr_app)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

Include(FetchContent)

FetchContent_Declare(
  etl
  GIT_REPOSITORY https://github.com/ETLCPP/etl
  GIT_TAG        20.39.4
)

FetchContent_MakeAvailable(etl)

target_sources(app PRIVATE src/main.cpp src/EventLoop.cpp src/Timer.cpp src/Led.cpp)
target_link_libraries(app PRIVATE etl::etl)