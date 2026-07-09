cmake_minimum_required(VERSION 3.20)
project(elizad VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)
find_package(nlohmann_json QUIET)

if(NOT nlohmann_json_FOUND)
    include(FetchContent)
    FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
    )
    FetchContent_MakeAvailable(json)
endif()

# Include paths
set(ELIZAOS_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
set(ELIZAOS_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../build)

# Village Event Bus library
add_library(village_event_bus STATIC village_event_bus.cpp)
target_include_directories(village_event_bus PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ELIZAOS_INCLUDE}
)
target_link_libraries(village_event_bus PUBLIC CURL::libcurl nlohmann_json::nlohmann_json)

# elizad daemon — link against pre-built elizaos libraries
add_executable(elizad elizad.cpp)
target_include_directories(elizad PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ELIZAOS_INCLUDE}
)

# Link all required libraries from the main build
target_link_libraries(elizad PRIVATE
    village_event_bus
    village_group_dynamics
    antikythera_coupling
    ${ELIZAOS_BUILD_DIR}/cpp/packages/integration/autonomous_starter/libelizaos-autonomous_starter.a
    ${ELIZAOS_BUILD_DIR}/cpp/packages/core/endocrine/libelizaos-endocrine.a
    ${ELIZAOS_BUILD_DIR}/cpp/packages/core/core/libelizaos-core.a
    ${ELIZAOS_BUILD_DIR}/cpp/packages/core/agentloop/libelizaos-agentloop.a
    ${ELIZAOS_BUILD_DIR}/cpp/packages/core/agentaction/libelizaos-agentaction.a
    ${ELIZAOS_BUILD_DIR}/cpp/packages/core/agentagenda/libelizaos-agentagenda.a
    ${ELIZAOS_BUILD_DIR}/cpp/packages/core/agentmemory/libelizaos-agentmemory.a
    ${ELIZAOS_BUILD_DIR}/cpp/packages/core/persistence/libelizaos-persistence.a
    ${ELIZAOS_BUILD_DIR}/cpp/packages/integration/cognitive_curriculum/libelizaos-cognitive_curriculum.a
    ${ELIZAOS_BUILD_DIR}/cpp/packages/infrastructure/agentshell/libelizaos-agentshell.a
    ${ELIZAOS_BUILD_DIR}/cpp/packages/infrastructure/agentlogger/libelizaos-agentlogger.a
    CURL::libcurl
    nlohmann_json::nlohmann_json
    pthread
    sqlite3
)

install(TARGETS elizad RUNTIME DESTINATION bin)
install(TARGETS village_event_bus ARCHIVE DESTINATION lib)
install(FILES village_event_bus.hpp DESTINATION include/elizaos)

# Village Group Dynamics library
add_library(village_group_dynamics STATIC village_group_dynamics.cpp)
target_include_directories(village_group_dynamics PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ELIZAOS_INCLUDE}
)
target_link_libraries(village_group_dynamics PUBLIC nlohmann_json::nlohmann_json)

# Antikythera Temporal Coupling library
add_library(antikythera_coupling STATIC antikythera_coupling.cpp)
target_include_directories(antikythera_coupling PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ELIZAOS_INCLUDE}
)
target_link_libraries(antikythera_coupling PUBLIC nlohmann_json::nlohmann_json)
