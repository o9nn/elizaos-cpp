# Stage 3 - Registry system for plugin discovery and management
add_library(elizaos-registry STATIC
    src/registry.cpp
)

target_include_directories(elizaos-registry PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)

target_link_libraries(elizaos-registry 
    elizaos-core
    elizaos-agentlogger
    elizaos-plugins_automation
    nlohmann_json::nlohmann_json
)

# Add libcurl if available for HTTP registry access
if(CURL_FOUND)
    if(TARGET CURL::libcurl)
        target_link_libraries(elizaos-registry CURL::libcurl)
    else()
        target_link_libraries(elizaos-registry ${CURL_LIBRARIES})
        target_include_directories(elizaos-registry PRIVATE ${CURL_INCLUDE_DIRS})
    endif()
    target_compile_definitions(elizaos-registry PRIVATE LIBCURL_FOUND)
endif()
