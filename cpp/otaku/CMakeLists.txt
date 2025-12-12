add_library(elizaos-otaku STATIC
    src/placeholder.cpp
)

target_include_directories(elizaos-otaku PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(elizaos-otaku
    elizaos-core
    elizaos-agentlogger
    Threads::Threads
)
