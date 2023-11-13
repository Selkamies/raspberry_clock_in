cmake_minimum_required(VERSION 3.0)

project(MyRaspberryPiProject C)

set(CMAKE_C_STANDARD 11)

# Add your source files
file(GLOB_RECURSE SOURCES "src/*.c")

# Exclude specific files or directories if needed
#list(FILTER SOURCES EXCLUDE REGEX "src/excluded_file.c")

# Add your include directory
include_directories(include)

# Add any external libraries
# target_link_libraries(your_target_name external_lib)

# Set the output directory for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Create the executable
add_executable(clock_in ${SOURCES})

# Link against the pigpio library
target_link_libraries(clock_in pigpio)