# Stage 2 - Infrastructure: AgentShell Implementation
add_library(elizaos-agentshell STATIC
    src/agentshell.cpp
)

target_include_directories(elizaos-agentshell PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)

target_link_libraries(elizaos-agentshell 
    elizaos-core
    elizaos-agentlogger
)
