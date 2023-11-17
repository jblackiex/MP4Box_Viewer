# CMake version
cmake_minimum_required(VERSION 3.10)

# Project Name
project(mp4box)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Path to Qt
set(CMAKE_PREFIX_PATH "/path/to/qt")

# Source Files
add_executable(mp4box code/main.cpp code/utils.cpp)

set_target_properties(mp4box PROPERTIES
    # COMPILE_FLAGS "-g -Wall -Wextra -Werror -static-libgcc -static-libstdc++"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

set(SHELL_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/script.sh")

# Add a custom command to run the script.sh before building
add_custom_command(
    TARGET mp4box
    PRE_BUILD
    COMMAND bash "${SHELL_SCRIPT}"
    VERBATIM
)

# Find and link Qt libraries
find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)
target_link_libraries(mp4box PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
