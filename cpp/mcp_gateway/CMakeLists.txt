# MCP Gateway Module - Model Context Protocol Gateway

add_library(elizaos-mcp_gateway STATIC
    src/placeholder.cpp
)

target_include_directories(elizaos-mcp_gateway PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(elizaos-mcp_gateway
    elizaos-core
    elizaos-agentcomms
    elizaos-agentlogger
    nlohmann_json::nlohmann_json
    Threads::Threads
)
