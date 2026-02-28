# Stage 2 implementation - agentlogger with full functionality
add_library(elizaos-agentlogger STATIC
    src/agentlogger.cpp
)

target_include_directories(elizaos-agentlogger PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(elizaos-agentlogger
    elizaos-core
)
