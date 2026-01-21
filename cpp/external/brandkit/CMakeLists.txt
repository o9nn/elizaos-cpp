# Stage 3 - Application-specific placeholder - implementation planned
add_library(elizaos-brandkit STATIC
    src/brandkit.cpp
)

target_include_directories(elizaos-brandkit PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(elizaos-brandkit 
    elizaos-core
)
