# Stage 3 - Autonomous Starter implementation
add_library(elizaos-autonomous_starter STATIC
    src/autonomous_starter.cpp
)

target_include_directories(elizaos-autonomous_starter PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(elizaos-autonomous_starter 
    elizaos-core
    elizaos-agentloop
    elizaos-agentshell
    elizaos-agentlogger
    Threads::Threads
)

# Autonomous Starter Demo
add_executable(autonomous_starter_demo
    src/demo.cpp
)

target_include_directories(autonomous_starter_demo PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(autonomous_starter_demo
    elizaos-autonomous_starter
    elizaos-core
    elizaos-agentloop
    elizaos-agentshell
    elizaos-agentlogger
    Threads::Threads
)
