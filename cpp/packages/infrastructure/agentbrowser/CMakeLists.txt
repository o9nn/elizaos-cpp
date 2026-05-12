# AgentBrowser - Web automation interface for ElizaOS agents

find_package(Threads REQUIRED)
find_package(CURL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GUMBO REQUIRED gumbo)

add_library(elizaos-agentbrowser STATIC
    src/agentbrowser.cpp
)

target_include_directories(elizaos-agentbrowser PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)

target_include_directories(elizaos-agentbrowser PRIVATE
    ${CURL_INCLUDE_DIRS}
    ${GUMBO_INCLUDE_DIRS}
)

target_link_directories(elizaos-agentbrowser PRIVATE
    ${GUMBO_LIBRARY_DIRS}
)

target_link_libraries(elizaos-agentbrowser
    elizaos-core
    elizaos-agentlogger
    elizaos-agentmemory
    Threads::Threads
    CURL::libcurl
    ${GUMBO_LIBRARIES}
)

target_compile_definitions(elizaos-agentbrowser PRIVATE
    ELIZAOS_AGENTBROWSER_REAL_HTTP=1
)
