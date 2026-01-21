# Stage 2 implementation - agentcomms with basic networking infrastructure
add_library(elizaos-agentcomms STATIC
    src/agentcomms.cpp
)

target_include_directories(elizaos-agentcomms PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(elizaos-agentcomms 
    elizaos-core
)
