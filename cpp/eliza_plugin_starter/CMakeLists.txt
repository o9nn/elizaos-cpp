# ElizaOS Plugin Development Starter
# Provides plugin interfaces, registry, and lifecycle management
add_library(elizaos-eliza_plugin_starter STATIC
    src/eliza_plugin_starter.cpp
)

target_include_directories(elizaos-eliza_plugin_starter PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(elizaos-eliza_plugin_starter
    elizaos-core
)
