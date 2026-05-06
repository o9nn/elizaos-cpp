# Stage 3 - Application-specific placeholder - implementation planned
add_library(elizaos-eliza_nextjs_starter STATIC
    src/eliza_nextjs_starter.cpp
)

target_include_directories(elizaos-eliza_nextjs_starter PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(elizaos-eliza_nextjs_starter 
    elizaos-core
)
