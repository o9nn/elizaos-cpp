# Stage 5 - Web and Documentation - Website module
add_library(elizaos-website STATIC
    src/placeholder.cpp
)

target_include_directories(elizaos-website PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(elizaos-website 
    elizaos-core
    elizaos-agentlogger
)
