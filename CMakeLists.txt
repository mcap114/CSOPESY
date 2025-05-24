cmake_minimum_required(VERSION 3.10)
project(CSOPESY_Emulator)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configure output
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Screen module (fixed source paths)
add_library(Screen
    "Week 3 - Designing Console Layouts/Screen/Screen.cpp"
    "Week 3 - Designing Console Layouts/Screen/ScreenManager.cpp"
)

# Main executable
add_executable(csopesy_emulator
    "Week 3 - Designing Console Layouts/main.cpp"
    "Week 3 - Designing Console Layouts/CLI/CLI.cpp"
)

# Link dependencies
target_link_libraries(csopesy_emulator PRIVATE Screen)

# Include directories
target_include_directories(csopesy_emulator PRIVATE
    "Week 3 - Designing Console Layouts/CLI"
    "Week 3 - Designing Console Layouts/Screen"
)