add_library(elizaos-otc_agent STATIC
    src/otc_agent.cpp
)

target_include_directories(elizaos-otc_agent PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(elizaos-otc_agent
    elizaos-core
    elizaos-agentlogger
    Threads::Threads
)
