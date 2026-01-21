# Stage 2 implementation - agentlogger with full functionality
add_library(elizaos-agentlogger STATIC
    src/agentlogger.cpp
)

target_include_directories(elizaos-agentlogger PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(elizaos-agentlogger 
    elizaos-core
)
