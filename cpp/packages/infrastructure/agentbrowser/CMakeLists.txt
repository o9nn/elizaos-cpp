# AgentBrowser - Web automation interface for ElizaOS agents

find_package(Threads REQUIRED)
if(NOT TARGET CURL::libcurl)
    find_package(CURL REQUIRED)
endif()
find_package(PkgConfig QUIET)
option(ELIZAOS_FORCE_BUNDLED_GUMBO "Build the pinned Gumbo fallback even when a system package is available" OFF)

set(ELIZAOS_GUMBO_TARGET "")
if(PkgConfig_FOUND AND NOT ELIZAOS_FORCE_BUNDLED_GUMBO)
    pkg_check_modules(GUMBO QUIET IMPORTED_TARGET gumbo)
    if(TARGET PkgConfig::GUMBO)
        set(ELIZAOS_GUMBO_TARGET PkgConfig::GUMBO)
    endif()
endif()

if(NOT ELIZAOS_GUMBO_TARGET)
    if(NOT CMAKE_C_COMPILER_LOADED)
        enable_language(C)
    endif()
    include(FetchContent)
    FetchContent_Declare(
        gumbo_parser
        URL https://github.com/google/gumbo-parser/archive/refs/tags/v0.10.1.tar.gz
        URL_HASH SHA256=28463053d44a5dfbc4b77bcf49c8cee119338ffa636cc17fc3378421d714efad
    )
    FetchContent_GetProperties(gumbo_parser)
    if(NOT gumbo_parser_POPULATED)
        FetchContent_Populate(gumbo_parser)
        add_library(elizaos-gumbo STATIC
            ${gumbo_parser_SOURCE_DIR}/src/attribute.c
            ${gumbo_parser_SOURCE_DIR}/src/char_ref.c
            ${gumbo_parser_SOURCE_DIR}/src/error.c
            ${gumbo_parser_SOURCE_DIR}/src/parser.c
            ${gumbo_parser_SOURCE_DIR}/src/string_buffer.c
            ${gumbo_parser_SOURCE_DIR}/src/string_piece.c
            ${gumbo_parser_SOURCE_DIR}/src/tag.c
            ${gumbo_parser_SOURCE_DIR}/src/tokenizer.c
            ${gumbo_parser_SOURCE_DIR}/src/utf8.c
            ${gumbo_parser_SOURCE_DIR}/src/util.c
            ${gumbo_parser_SOURCE_DIR}/src/vector.c
        )
        target_include_directories(elizaos-gumbo PUBLIC
            ${gumbo_parser_SOURCE_DIR}/src
        )
        set_target_properties(elizaos-gumbo PROPERTIES
            C_STANDARD 99
            C_STANDARD_REQUIRED ON
            C_EXTENSIONS ON
            POSITION_INDEPENDENT_CODE ON
        )
        if(MSVC)
            target_include_directories(elizaos-gumbo PRIVATE
                ${gumbo_parser_SOURCE_DIR}/visualc/include
            )
            target_compile_definitions(elizaos-gumbo PRIVATE
                _CRT_SECURE_NO_WARNINGS
                _CRT_NONSTDC_NO_DEPRECATE
            )
        endif()
    endif()
    set(ELIZAOS_GUMBO_TARGET elizaos-gumbo)
endif()

add_library(elizaos-agentbrowser STATIC
    src/agentbrowser.cpp
)

target_include_directories(elizaos-agentbrowser PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)

target_include_directories(elizaos-agentbrowser PRIVATE
    ${CURL_INCLUDE_DIRS}
)

target_link_libraries(elizaos-agentbrowser
    elizaos-core
    elizaos-agentlogger
    elizaos-agentmemory
    Threads::Threads
    CURL::libcurl
    ${ELIZAOS_GUMBO_TARGET}
)

target_compile_definitions(elizaos-agentbrowser PRIVATE
    ELIZAOS_AGENTBROWSER_REAL_HTTP=1
)
