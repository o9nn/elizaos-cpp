# Stage 3 - Application-specific - Eliza's World Virtual Environment System
add_library(elizaos-elizas_world STATIC
    src/elizas_world.cpp
)

target_include_directories(elizaos-elizas_world PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(elizaos-elizas_world 
    elizaos-core
)
