# Village Module — group dynamics, antikythera coupling, event bus, elizad daemon
# Included as subdirectory from the main project CMakeLists.txt

find_package(CURL REQUIRED)

# Include paths
set(ELIZAOS_VILLAGE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

# Village Event Bus library
add_library(village_event_bus STATIC village_event_bus.cpp)
target_include_directories(village_event_bus PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ELIZAOS_VILLAGE_INCLUDE}
)
target_link_libraries(village_event_bus PUBLIC CURL::libcurl nlohmann_json::nlohmann_json)

# Village Group Dynamics library
add_library(village_group_dynamics STATIC village_group_dynamics.cpp)
target_include_directories(village_group_dynamics PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ELIZAOS_VILLAGE_INCLUDE}
)
target_link_libraries(village_group_dynamics PUBLIC nlohmann_json::nlohmann_json)

# Antikythera Temporal Coupling library
add_library(antikythera_coupling STATIC antikythera_coupling.cpp)
target_include_directories(antikythera_coupling PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ELIZAOS_VILLAGE_INCLUDE}
)
target_link_libraries(antikythera_coupling PUBLIC nlohmann_json::nlohmann_json)

# elizad daemon — links against main project targets
add_executable(elizad elizad.cpp)
target_include_directories(elizad PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ELIZAOS_VILLAGE_INCLUDE}
)
target_link_libraries(elizad PRIVATE
    village_event_bus
    village_group_dynamics
    antikythera_coupling
    elizaos-autonomous_starter
    elizaos-endocrine
    elizaos-core
    elizaos-agentloop
    elizaos-agentaction
    elizaos-agentagenda
    elizaos-agentmemory
    elizaos-persistence
    elizaos-cognitive_curriculum
    elizaos-agentshell
    elizaos-agentlogger
    CURL::libcurl
    nlohmann_json::nlohmann_json
    pthread
    sqlite3
)

install(TARGETS elizad RUNTIME DESTINATION bin)
install(TARGETS village_event_bus village_group_dynamics antikythera_coupling ARCHIVE DESTINATION lib)
install(FILES village_event_bus.hpp village_group_dynamics.hpp antikythera_coupling.hpp
              village_ksm_transfer.hpp village_agnai_bridge.hpp
        DESTINATION include/elizaos)
