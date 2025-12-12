# AgentBrowser - Web automation interface for ElizaOS agents
add_library(elizaos-agentbrowser STATIC
    src/agentbrowser.cpp
)

target_include_directories(elizaos-agentbrowser PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(elizaos-agentbrowser 
    elizaos-core
    elizaos-agentlogger
    elizaos-agentmemory
    Threads::Threads
)
