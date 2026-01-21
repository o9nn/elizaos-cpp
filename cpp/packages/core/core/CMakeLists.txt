add_library(elizaos-core STATIC
    src/core.cpp
)

target_include_directories(elizaos-core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(elizaos-core 
    Threads::Threads
)