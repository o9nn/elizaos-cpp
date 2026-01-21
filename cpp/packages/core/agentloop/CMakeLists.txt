add_library(elizaos-agentloop STATIC
    src/agentloop.cpp
)

target_include_directories(elizaos-agentloop PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(elizaos-agentloop 
    elizaos-core
    Threads::Threads
)