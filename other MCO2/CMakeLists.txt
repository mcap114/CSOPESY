cmake_minimum_required(VERSION 3.10)

# Set the project name
project(csopesy_mco2_project)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Specify the base directory
set(MCO2_DIR "../MCO2")

# Include directories
include_directories(
    "${MCO2_DIR}/CLI"
    "${MCO2_DIR}/Screen"
    "${MCO2_DIR}/Config"
    "${MCO2_DIR}/Scheduler"
)

# Define the source files explicitly
set(SOURCES
    "${MCO2_DIR}/main.cpp"
    "${MCO2_DIR}/CLI/CLI.cpp"
    "${MCO2_DIR}/Screen/Screen.cpp"
    "${MCO2_DIR}/Screen/ScreenManager.cpp"
    "${MCO2_DIR}/Config/Config.cpp"
    "${MCO2_DIR}/Scheduler/RRScheduler.cpp"
    "${MCO2_DIR}/Scheduler/FCFSScheduler.cpp"
    "${MCO2_DIR}/Scheduler/SchedulerFactory.cpp"
)

# Define the headers (optional, helpful for IDEs)
set(HEADERS
    "${MCO2_DIR}/CLI/CLI.h"
    "${MCO2_DIR}/Screen/Screen.h"
    "${MCO2_DIR}/Screen/ScreenManager.h"
    "${MCO2_DIR}/Config/Config.h"
    "${MCO2_DIR}/Scheduler/Scheduler.h"
    "${MCO2_DIR}/Scheduler/RRScheduler.h"
    "${MCO2_DIR}/Scheduler/FCFSScheduler.h"
    "${MCO2_DIR}/Scheduler/SchedulerFactory.h"
)

# Add the executable
add_executable(csopesy_mco2 ${SOURCES} ${HEADERS})
