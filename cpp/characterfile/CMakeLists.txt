# Stage 3 - Application-specific placeholder - implementation planned
add_library(elizaos-characterfile STATIC
    src/placeholder.cpp
)

target_include_directories(elizaos-characterfile PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(elizaos-characterfile 
    elizaos-core
    elizaos-characters
    elizaos-agentlogger
)
