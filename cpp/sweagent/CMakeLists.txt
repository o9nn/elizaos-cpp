# SWEAgent Module - Autonomous Software Engineer

add_library(elizaos-sweagent STATIC
    src/placeholder.cpp
)

target_include_directories(elizaos-sweagent PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(elizaos-sweagent
    elizaos-core
    elizaos-agentloop
    elizaos-agentmemory
    elizaos-agentaction
    elizaos-agentlogger
    Threads::Threads
)
