add_library(elizaos-classified STATIC
    src/classified.cpp
)

target_include_directories(elizaos-classified PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)

target_link_libraries(elizaos-classified
    elizaos-core
    elizaos-agentlogger
    Threads::Threads
)
