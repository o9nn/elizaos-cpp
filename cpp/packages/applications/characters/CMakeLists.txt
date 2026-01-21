# Stage 3 - Application-specific - Character Management System
add_library(elizaos-characters STATIC
    src/characters.cpp
    src/character_json_loader.cpp
    src/character.cpp
    src/character_evolution_evaluator.cpp
    src/character_updater.cpp
)

target_include_directories(elizaos-characters PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(elizaos-characters
    elizaos-core
    elizaos-agentmemory
    elizaos-agentlogger
    elizaos-agentaction
    nlohmann_json::nlohmann_json
    Threads::Threads
)
