# Stage 3 - Application-specific placeholder - implementation planned
add_library(elizaos-hat STATIC
    src/hat.cpp
)

target_include_directories(elizaos-hat PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(elizaos-hat 
    elizaos-core
)
