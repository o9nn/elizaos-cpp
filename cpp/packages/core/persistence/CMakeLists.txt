# Persistence Module - SQLite-backed storage for ElizaOS agents

option(ELIZAOS_FORCE_BUNDLED_SQLITE "Build the pinned SQLite fallback even when a system package is available" OFF)

if(NOT ELIZAOS_FORCE_BUNDLED_SQLITE)
    find_package(SQLite3 QUIET)
endif()

if(NOT TARGET SQLite::SQLite3)
    if(NOT CMAKE_C_COMPILER_LOADED)
        enable_language(C)
    endif()
    include(FetchContent)
    FetchContent_Declare(
        sqlite_amalgamation
        URL https://www.sqlite.org/2024/sqlite-amalgamation-3450100.zip
        URL_HASH SHA256=5592243caf28b2cdef41e6ab58d25d653dfc53deded8450eb66072c929f030c4
    )
    FetchContent_GetProperties(sqlite_amalgamation)
    if(NOT sqlite_amalgamation_POPULATED)
        FetchContent_Populate(sqlite_amalgamation)
        add_library(elizaos-sqlite3 STATIC
            ${sqlite_amalgamation_SOURCE_DIR}/sqlite3.c
        )
        add_library(SQLite::SQLite3 ALIAS elizaos-sqlite3)
        target_include_directories(elizaos-sqlite3 PUBLIC
            ${sqlite_amalgamation_SOURCE_DIR}
        )
        target_compile_definitions(elizaos-sqlite3 PRIVATE
            SQLITE_THREADSAFE=1
        )
        set_target_properties(elizaos-sqlite3 PROPERTIES
            C_STANDARD 99
            C_STANDARD_REQUIRED ON
            C_EXTENSIONS ON
            POSITION_INDEPENDENT_CODE ON
        )
        if(CMAKE_DL_LIBS)
            target_link_libraries(elizaos-sqlite3 PUBLIC ${CMAKE_DL_LIBS})
        endif()
    endif()
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
