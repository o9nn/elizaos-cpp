add_library(elizaos-core STATIC
    src/core.cpp
    src/runtime.cpp
    src/actions.cpp
)

target_include_directories(elizaos-core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(elizaos-core
    Threads::Threads
)
