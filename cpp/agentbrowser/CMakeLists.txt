# AgentBrowser - Web automation interface for ElizaOS agents
add_library(elizaos-agentbrowser STATIC
    src/agentbrowser.cpp
)

target_include_directories(elizaos-agentbrowser PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(elizaos-agentbrowser 
    elizaos-core
    elizaos-agentlogger
    elizaos-agentmemory
    Threads::Threads
)

# Add libcurl for HTTP requests
if(CURL_FOUND)
    target_link_libraries(elizaos-agentbrowser ${CURL_LIBRARIES})
    target_include_directories(elizaos-agentbrowser PRIVATE ${CURL_INCLUDE_DIRS})
endif()

# Add gumbo-parser for HTML parsing
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(GUMBO QUIET gumbo)
    if(GUMBO_FOUND)
        target_link_libraries(elizaos-agentbrowser ${GUMBO_LIBRARIES})
        target_include_directories(elizaos-agentbrowser PRIVATE ${GUMBO_INCLUDE_DIRS})
    endif()
endif()
