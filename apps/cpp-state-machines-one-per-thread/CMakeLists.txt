cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(my_zephyr_app)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

target_sources(app PRIVATE src/App.cpp)
target_sources(app PRIVATE src/main.cpp)
target_sources(app PRIVATE src/LedSm.cpp)
target_sources(app PRIVATE src/StateMachineLibrary/StateMachine.cpp)
target_sources(app PRIVATE src/StateMachineLibrary/StateMachineController.cpp)
target_sources(app PRIVATE src/StateMachineLibrary/Timer.cpp)
target_sources(app PRIVATE src/MasterSm.cpp)
target_sources(app PRIVATE src/SecondSm.cpp)
target_sources(app PRIVATE src/Gpio/GpioMock.cpp)