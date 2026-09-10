# Persistence Module - SQLite-backed storage for ElizaOS agents
find_package(SQLite3 REQUIRED)

add_library(elizaos-persistence STATIC
    src/persistence.cpp
)

target_include_directories(elizaos-persistence PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)

target_link_libraries(elizaos-persistence PUBLIC
    elizaos-core
    SQLite::SQLite3
    Threads::Threads
)

set_target_properties(elizaos-persistence PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

install(TARGETS elizaos-persistence
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(FILES
    ${CMAKE_SOURCE_DIR}/include/elizaos/persistence.hpp
    DESTINATION include/elizaos
)
