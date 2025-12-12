add_library(elizaos-workgroups
    src/workgroups.cpp
)

target_include_directories(elizaos-workgroups PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(elizaos-workgroups
    elizaos-core
    elizaos-agentcomms
    elizaos-agentmemory
    elizaos-agentagenda
    elizaos-agentlogger
)
