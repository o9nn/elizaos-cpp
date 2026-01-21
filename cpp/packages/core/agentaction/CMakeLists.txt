# Stage 3 - Action orchestration and processing
add_library(elizaos-agentaction STATIC
    src/agentaction.cpp
)

target_include_directories(elizaos-agentaction PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(elizaos-agentaction 
    elizaos-core
    elizaos-agentmemory
    elizaos-agentlogger
)