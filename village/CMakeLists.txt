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

# Village Event Bus library
add_library(village_event_bus STATIC village_event_bus.cpp)
target_include_directories(village_event_bus PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)
target_link_libraries(village_event_bus PUBLIC CURL::libcurl nlohmann_json::nlohmann_json)

# elizad daemon
add_executable(elizad elizad.cpp)
target_include_directories(elizad PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)
target_link_libraries(elizad PRIVATE village_event_bus CURL::libcurl nlohmann_json::nlohmann_json)

# Link against elizaos-cpp libraries from parent build
if(TARGET elizaos-autonomous_starter)
    target_link_libraries(elizad PRIVATE
        elizaos-autonomous_starter
        elizaos-endocrine
        elizaos-cognitive_bridge
        elizaos-core
    )
endif()

install(TARGETS elizad RUNTIME DESTINATION bin)
install(TARGETS village_event_bus ARCHIVE DESTINATION lib)
install(FILES village_event_bus.hpp DESTINATION include/elizaos)
