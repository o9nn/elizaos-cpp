# Stage 5 - Web and Documentation - ElizaOS GitHub.io module
add_library(elizaos-elizaos_github_io STATIC
    src/elizaos_github_io.cpp
)

target_include_directories(elizaos-elizaos_github_io PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(elizaos-elizaos_github_io 
    elizaos-core
    elizaos-agentlogger
)
