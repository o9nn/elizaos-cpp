# Village Module — group dynamics, antikythera coupling, event bus, elizad daemon
# Included as subdirectory from the main project CMakeLists.txt

find_package(CURL REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(OpenSSL REQUIRED)

# Include paths
set(ELIZAOS_VILLAGE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

# Village Event Bus library
add_library(village_event_bus STATIC village_event_bus.cpp)
target_include_directories(village_event_bus PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ELIZAOS_VILLAGE_INCLUDE}
)
target_link_libraries(village_event_bus PUBLIC CURL::libcurl nlohmann_json::nlohmann_json)

add_executable(village_event_bus_test village_event_bus_test.cpp)
target_link_libraries(village_event_bus_test PRIVATE village_event_bus)
add_test(NAME village_event_bus_test COMMAND village_event_bus_test)

# Resident generation provenance, validation, and append-only evidence journal
add_library(response_authenticity STATIC response_authenticity.cpp)
target_include_directories(response_authenticity PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ELIZAOS_VILLAGE_INCLUDE}
)
target_link_libraries(response_authenticity PUBLIC OpenSSL::Crypto nlohmann_json::nlohmann_json)

add_executable(response_authenticity_test response_authenticity_test.cpp)
target_link_libraries(response_authenticity_test PRIVATE response_authenticity)
add_test(NAME response_authenticity_test COMMAND response_authenticity_test)

add_library(response_development STATIC response_development.cpp)
target_include_directories(response_development PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ELIZAOS_VILLAGE_INCLUDE}
)
target_link_libraries(response_development PUBLIC response_authenticity nlohmann_json::nlohmann_json)

add_executable(response_development_test response_development_test.cpp)
target_link_libraries(response_development_test PRIVATE response_development)
add_test(NAME response_development_test COMMAND response_development_test)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ELIZAOS_VILLAGE_INCLUDE}
)
target_link_libraries(elizad PRIVATE
    village_event_bus
    response_authenticity
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
    Threads::Threads
    SQLite::SQLite3
)

if(WIN32)
    target_link_libraries(elizad PRIVATE ws2_32)
endif()

install(TARGETS elizad RUNTIME DESTINATION bin)
install(TARGETS village_event_bus response_authenticity response_development village_group_dynamics antikythera_coupling ARCHIVE DESTINATION lib)
install(FILES village_event_bus.hpp village_group_dynamics.hpp antikythera_coupling.hpp
              village_ksm_transfer.hpp village_agnai_bridge.hpp village_atomspace.hpp
              response_authenticity.hpp
              response_development.hpp
        DESTINATION include/elizaos)
