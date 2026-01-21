cmake_minimum_required(VERSION 3.16)
project(elizaos-goal_manager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/goal_manager.cpp
)

# Create library
add_library(elizaos-goal_manager STATIC ${SOURCES})

# Include directories
target_include_directories(elizaos-goal_manager PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# Link dependencies
target_link_libraries(elizaos-goal_manager PUBLIC
    Threads::Threads
)
