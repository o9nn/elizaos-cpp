add_library(elizaos-trust_scoreboard
    src/trust_scoreboard.cpp
)

target_include_directories(elizaos-trust_scoreboard PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)

target_link_libraries(elizaos-trust_scoreboard
    elizaos-core
    elizaos-agentmemory
    elizaos-agentcomms
    elizaos-agentlogger
    nlohmann_json::nlohmann_json
)
