# Stage 3 - Task scheduling with std::chrono
add_library(elizaos-agentagenda STATIC
    src/agentagenda.cpp
)

target_include_directories(elizaos-agentagenda PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(elizaos-agentagenda 
    elizaos-core
    elizaos-agentmemory
    elizaos-agentlogger
)