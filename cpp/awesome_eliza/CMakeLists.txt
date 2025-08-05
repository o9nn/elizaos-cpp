# Stage 3 - Application-specific - awesome eliza resources management
add_library(elizaos-awesome_eliza STATIC
    src/awesome_eliza.cpp
)

target_include_directories(elizaos-awesome_eliza PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(elizaos-awesome_eliza 
    elizaos-core
)
