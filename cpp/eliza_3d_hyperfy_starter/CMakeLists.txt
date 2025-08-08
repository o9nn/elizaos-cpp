# Stage 3 - Application-specific implementation for eliza_3d_hyperfy_starter
add_library(elizaos-eliza_3d_hyperfy_starter STATIC
    src/placeholder.cpp
)

target_include_directories(elizaos-eliza_3d_hyperfy_starter PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(elizaos-eliza_3d_hyperfy_starter 
    elizaos-core
    elizaos-agentlogger
    Threads::Threads
)
