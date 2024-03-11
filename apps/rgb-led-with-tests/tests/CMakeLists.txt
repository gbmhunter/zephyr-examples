cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(integration)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include all source files in the main application
# except main.c, which we will filter out
FILE(GLOB appSources ../src/*.c)
list(FILTER appSources EXCLUDE REGEX ".*main\\.c$")

# Include all source files in the test application
FILE(GLOB testAppSources src/*.c)
target_sources(app PRIVATE ${testAppSources} ${appSources})

# Add the main application directory to the include path
target_include_directories(app PRIVATE "../src/")
