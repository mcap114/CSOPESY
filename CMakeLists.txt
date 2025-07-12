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

# === FILE GROUPINGS ===

file(GLOB_RECURSE CORE_SRC
    "Week 10 - First-fit memory allocator with round-robin scheduler/Core/*.cpp"
)

file(GLOB_RECURSE UTILITIES_SRC
    "Week 10 - First-fit memory allocator with round-robin scheduler/Utilities/*.cpp"
)

file(GLOB_RECURSE SCREEN_SRC
    "Week 10 - First-fit memory allocator with round-robin scheduler/Screen/*.cpp"
)

file(GLOB_RECURSE SCHEDULER_SRC
    "Week 10 - First-fit memory allocator with round-robin scheduler/Scheduler/*.cpp"
)

file(GLOB_RECURSE CLI_SRC
    "Week 10 - First-fit memory allocator with round-robin scheduler/CLI/*.cpp"
)

# === CORE LIBRARY ===
add_library(Core ${CORE_SRC} "Week 10 - First-fit memory allocator with round-robin scheduler/Memory/MemoryManager.h" "Week 10 - First-fit memory allocator with round-robin scheduler/Memory/MemoryManager.cpp")
target_include_directories(Core PUBLIC
    "Week 10 - First-fit memory allocator with round-robin scheduler/Core"
)

# === UTILITIES LIBRARY ===
add_library(Utilities ${UTILITIES_SRC} "Week 10 - First-fit memory allocator with round-robin scheduler/Memory/MemoryManager.h" "Week 10 - First-fit memory allocator with round-robin scheduler/Memory/MemoryManager.cpp")
target_include_directories(Utilities PUBLIC
    "Week 10 - First-fit memory allocator with round-robin scheduler/Utilities"
)

# === SCREEN LIBRARY ===
add_library(Screen ${SCREEN_SRC} "Week 10 - First-fit memory allocator with round-robin scheduler/Memory/MemoryManager.h" "Week 10 - First-fit memory allocator with round-robin scheduler/Memory/MemoryManager.cpp")
target_include_directories(Screen PUBLIC
    "Week 10 - First-fit memory allocator with round-robin scheduler/Screen"
)

# === SCHEDULER LIBRARY ===
add_library(Scheduler ${SCHEDULER_SRC} "Week 10 - First-fit memory allocator with round-robin scheduler/Memory/MemoryManager.h" "Week 10 - First-fit memory allocator with round-robin scheduler/Memory/MemoryManager.cpp")
target_include_directories(Scheduler PUBLIC
    "Week 10 - First-fit memory allocator with round-robin scheduler/Scheduler"
)

# === MAIN EXECUTABLE ===
add_executable(csopesy_emulator
    ${CLI_SRC}
    "Week 10 - First-fit memory allocator with round-robin scheduler/main.cpp"
 "Week 10 - First-fit memory allocator with round-robin scheduler/Memory/MemoryManager.h" "Week 10 - First-fit memory allocator with round-robin scheduler/Memory/MemoryManager.cpp")

# === LINK ALL LIBRARIES TO MAIN EXECUTABLE ===
target_link_libraries(csopesy_emulator PRIVATE
    Core
    Utilities
    Screen
    Scheduler
)

# === INCLUDE DIRECTORIES FOR CLI ===
target_include_directories(csopesy_emulator PRIVATE
    "Week 10 - First-fit memory allocator with round-robin scheduler/CLI"
)
