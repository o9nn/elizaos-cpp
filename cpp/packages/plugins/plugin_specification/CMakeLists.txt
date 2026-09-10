# Stage 3 - Application-specific placeholder - implementation planned
add_library(elizaos-plugin_specification STATIC
    src/plugin_specification.cpp
)

target_compile_features(elizaos-plugin_specification PUBLIC cxx_std_17)

target_include_directories(elizaos-plugin_specification PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)

target_link_libraries(elizaos-plugin_specification
    PUBLIC elizaos-core
    PRIVATE ${CMAKE_DL_LIBS}
)
