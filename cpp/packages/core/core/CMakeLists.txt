add_library(elizaos-core STATIC
    src/core.cpp
    src/action.cpp
    src/actions.cpp
    src/database.cpp
    src/memory.cpp
    src/runtime.cpp
)

target_include_directories(elizaos-core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(elizaos-core
    Threads::Threads
)
