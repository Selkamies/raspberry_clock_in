cmake_minimum_required(VERSION 3.0)

project(MyRaspberryPiProject C)

set(CMAKE_C_STANDARD 11)
#set(CMAKE_VERBOSE_MAKEFILE ON)

# Set the build type to Debug if not explicitly set
#if(NOT CMAKE_BUILD_TYPE)
#    set(CMAKE_BUILD_TYPE Debug)
#endif()

# Add your source files
#file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.c")
set(SOURCES
    src/main.c
    src/config_handler.c
    src/gpio_init.c
    src/keypad.c
    src/gpio_functions.c
    src/leds.c
    src/sounds.c
    src/timer.c
    src/database.c
)

# List all header files
#file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "include/*.h")
#set(HEADERS
#    src/gpio_handler.h
#    src/pins.h
#    src/keypad/keypad.h
#    src/keypad/keypad_gpio.h
#)


# Exclude specific files or directories if needed
#list(FILTER SOURCES EXCLUDE REGEX "src/excluded_file.c")

# Add your include directory
#include_directories(include)
#include_directories(${CMAKE_SOURCE_DIR}/include)

# Add subdirectories. These all need their own CMakeLists.txt.
#add_subdirectory(include/keypad)

# Set the output directory for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Create the executable
add_executable(clock_in ${SOURCES})

# Find SDL2
find_package(SDL2 REQUIRED)
target_include_directories(clock_in PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(clock_in ${SDL2_LIBRARIES})

# Find SDL2_mixer
find_package(SDL2_mixer REQUIRED)
target_include_directories(clock_in PRIVATE ${SDL2_MIXER_INCLUDE_DIRS})
#target_link_libraries(clock_in ${SDL2_MIXER_LIBRARIES})
target_link_libraries(clock_in SDL2_mixer)

# Find SQLite3.
find_package(SQLite3 REQUIRED)
target_include_directories(clock_in PRIVATE ${SQLite3_INCLUDE_DIRS})
target_link_libraries(clock_in ${SQLite3_LIBRARIES})

# Add any external libraries
# target_link_libraries(your_target_name external_lib)
target_link_libraries(clock_in pigpio)

# Specify include directories for the target
target_include_directories(clock_in PRIVATE include)

# Print the build type for verification
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")