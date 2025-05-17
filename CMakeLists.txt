cmake_minimum_required(VERSION 3.10)
project(CSOPESY_Emulator)  # Change to your project name

# Set C++ standard (17 recommended for modern C++)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable (main target)
add_executable(csopesy_emulator
    "Week 2 - OS Emulator Setup/main.cpp"
    "Week 2 - OS Emulator Setup/CLI/CLI.cpp"
    # Add future files here:
    # "Week 6 - Scheduler/FCFS.cpp"
 "Week 3 - Designing Console Layouts/Screen/Screen.h" "Week 3 - Designing Console Layouts/Screen/Screen.cpp" "Week 3 - Designing Console Layouts/Screen/ScreenManager.h" "Week 3 - Designing Console Layouts/Screen/ScreenManager.cpp")

# Include directories (for headers)
target_include_directories(csopesy_emulator PRIVATE
    "Week 2 - OS Emulator Setup/CLI"
    # Add future include paths:
    # "Week 6 - Scheduler"
)

# Platform-specific settings (optional)
if(WIN32)
    target_compile_definitions(csopesy_emulator PRIVATE _WIN32)
endif()
