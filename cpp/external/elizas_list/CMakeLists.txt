# Stage 3 - Application-specific - Eliza's List local project management
add_library(elizaos-elizas_list STATIC
    src/elizas_list.cpp
)

target_include_directories(elizaos-elizas_list PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
target_compile_features(elizaos-elizas_list PUBLIC cxx_std_17)

if(MSVC)
    target_compile_options(elizaos-elizas_list PRIVATE /W4 /WX)
else()
    target_compile_options(elizaos-elizas_list PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_link_libraries(elizaos-elizas_list PUBLIC
    elizaos-core
    nlohmann_json::nlohmann_json
)
