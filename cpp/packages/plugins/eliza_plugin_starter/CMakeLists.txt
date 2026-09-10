# ElizaOS Plugin Development Starter
# Provides plugin interfaces, registry, and lifecycle management
add_library(elizaos-eliza_plugin_starter STATIC
    src/eliza_plugin_starter.cpp
)
target_compile_features(elizaos-eliza_plugin_starter PUBLIC cxx_std_17)

target_include_directories(elizaos-eliza_plugin_starter PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/cpp/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(elizaos-eliza_plugin_starter PUBLIC
    elizaos-core
    Threads::Threads
)
