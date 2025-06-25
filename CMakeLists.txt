cmake_minimum_required(VERSION 3.10)
project(CSOPESY_Emulator)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows-specific settings
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)  # Windows 7+
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")
endif()

# Configure output
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Screen library
add_library(Screen
    "MCO1/Screen/Screen.cpp"
    "MCO1/Screen/ScreenManager.cpp"
 "MCO1/Scheduler/BaseScheduler.h" "MCO1/Core/Config.h" "MCO1/Core/Config.cpp" "MCO1/Core/CPUTimer.cpp" "MCO1/Core/CPUTimer.h" "MCO1/Scheduler/RRScheduler.h" "MCO1/Scheduler/RRScheduler.cpp" "MCO1/Utilities/ReportGenerator.h" "MCO1/Utilities/ReportGenerator.cpp")

# Scheduler library
add_library(Scheduler
    "MCO1/Scheduler/Process.cpp"
    "MCO1/Scheduler/FCFSScheduler.cpp"
 "MCO1/Scheduler/BaseScheduler.h" "MCO1/Core/Config.h" "MCO1/Core/Config.cpp" "MCO1/Core/CPUTimer.cpp" "MCO1/Core/CPUTimer.h" "MCO1/Scheduler/RRScheduler.h" "MCO1/Scheduler/RRScheduler.cpp" "MCO1/Utilities/ReportGenerator.h" "MCO1/Utilities/ReportGenerator.cpp")

# Main executable
add_executable(csopesy_emulator
    "MCO1/main.cpp"
    "MCO1/CLI/CLI.cpp"
 "MCO1/Scheduler/BaseScheduler.h" "MCO1/Core/Config.h" "MCO1/Core/Config.cpp" "MCO1/Core/CPUTimer.cpp" "MCO1/Core/CPUTimer.h" "MCO1/Scheduler/RRScheduler.h" "MCO1/Scheduler/RRScheduler.cpp" "MCO1/Utilities/ReportGenerator.h" "MCO1/Utilities/ReportGenerator.cpp")

# Link dependencies
target_link_libraries(csopesy_emulator PRIVATE 
    Screen
    Scheduler
)

# Include directories
target_include_directories(csopesy_emulator PRIVATE
    "MCO1/CLI"
    "MCO1/Screen"
    "MCO1/Scheduler"
)