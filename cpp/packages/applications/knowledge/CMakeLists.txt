# Stage 3 - Application-specific - Knowledge Management System
add_library(elizaos-knowledge STATIC
    src/knowledge.cpp
    src/knowledge_helpers.cpp
)

target_include_directories(elizaos-knowledge PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(elizaos-knowledge
    elizaos-core
    elizaos-agentmemory
    elizaos-agentlogger
    elizaos-agentaction
    Threads::Threads
)
