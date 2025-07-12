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

# Configure output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# === CORE LIBRARY ===
add_library(Core
    "MCO1/Core/Config.cpp"
    "MCO1/Core/CPUTimer.cpp"
)

target_include_directories(Core PUBLIC
    "MCO1/Core"
)

# === UTILITIES LIBRARY ===
add_library(Utilities
    "MCO1/Utilities/ReportGenerator.cpp"
)

target_include_directories(Utilities PUBLIC
    "MCO1/Utilities"
)

# === SCREEN LIBRARY ===
add_library(Screen
    "MCO1/Screen/Screen.cpp"
    "MCO1/Screen/ScreenManager.cpp"
)

target_include_directories(Screen PUBLIC
    "MCO1/Screen"
)

# === SCHEDULER LIBRARY ===
add_library(Scheduler
    "MCO1/Scheduler/OSProcess.cpp"
    "MCO1/Scheduler/FCFSScheduler.cpp"
    "MCO1/Scheduler/RRScheduler.cpp"
)

target_include_directories(Scheduler PUBLIC
    "MCO1/Scheduler"
)

# === MAIN EXECUTABLE ===
add_executable(csopesy_emulator
    "MCO1/main.cpp"
    "MCO1/CLI/CLI.cpp"
)

# === LINK ALL LIBRARIES TO MAIN EXECUTABLE ===
target_link_libraries(csopesy_emulator PRIVATE
    Core
    Utilities
    Screen
    Scheduler
)

# === INCLUDE DIRECTORIES FOR CLI ===
target_include_directories(csopesy_emulator PRIVATE
    "MCO1/CLI"
)
