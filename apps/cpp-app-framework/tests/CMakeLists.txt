cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(integration)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include all source files in the main application
# except main.c, which we will filter out
FILE(GLOB appSources ../src/*.cpp)
list(FILTER appSources EXCLUDE REGEX ".*main\\.cpp$")

# Include all source files in the test application
FILE(GLOB testAppSources src/*.cpp)
target_sources(app PRIVATE ${testAppSources} ${appSources})

# Add the main application directory to the include path
target_include_directories(app PRIVATE "../src/")
