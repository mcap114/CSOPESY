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
    "Week 6 - FCFS scheduler/Screen/Screen.cpp"
    "Week 6 - FCFS scheduler/Screen/ScreenManager.cpp"
)

# Scheduler library
add_library(Scheduler
    "Week 6 - FCFS scheduler/Scheduler/Process.cpp"
    "Week 6 - FCFS scheduler/Scheduler/FCFSScheduler.cpp"
)

# Main executable
add_executable(csopesy_emulator
    "Week 6 - FCFS scheduler/main.cpp"
    "Week 6 - FCFS scheduler/CLI/CLI.cpp"
)

# Link dependencies
target_link_libraries(csopesy_emulator PRIVATE 
    Screen
    Scheduler
)

# Include directories
target_include_directories(csopesy_emulator PRIVATE
    "Week 6 - FCFS scheduler/CLI"
    "Week 6 - FCFS scheduler/Screen"
    "Week 6 - FCFS scheduler/Scheduler"
)