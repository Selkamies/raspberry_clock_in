cmake_minimum_required(VERSION 3.0)

project(MyRaspberryPiProject C)

set(CMAKE_C_STANDARD 11)

# Add your source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.c")
#set(SOURCES
#    src/main.c
#    src/gpio_handler.c
#    src/pins.c
#    src/keypad/keypad.c
#    src/keypad/keypad_gpio.c
#)

# List all header files
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "include/*.h")
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
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add subdirectories. These all need their own CMakeLists.txt.
#add_subdirectory(include/keypad)

# Add any external libraries
# target_link_libraries(your_target_name external_lib)

# Set the output directory for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Create the executable
add_executable(clock_in ${SOURCES})

# Link against the pigpio library
target_link_libraries(clock_in pigpio)