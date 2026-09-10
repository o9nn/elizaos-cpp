# Persistence Module - SQLite-backed storage for ElizaOS agents
find_package(SQLite3 QUIET)
if(TARGET SQLite::SQLite3)
    set(ELIZAOS_SQLITE_TARGET SQLite::SQLite3)
else()
    find_package(unofficial-sqlite3 CONFIG REQUIRED)
    set(ELIZAOS_SQLITE_TARGET unofficial::sqlite3::sqlite3)
endif()

add_library(elizaos-persistence STATIC
    src/persistence.cpp
)

target_include_directories(elizaos-persistence PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)

target_link_libraries(elizaos-persistence PUBLIC
    elizaos-core
    ${ELIZAOS_SQLITE_TARGET}
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
