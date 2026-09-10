# Stage 3 - Registry system for local plugin discovery and management
add_library(elizaos-registry STATIC
    src/registry.cpp
)

target_include_directories(elizaos-registry PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
target_compile_features(elizaos-registry PUBLIC cxx_std_17)

if(MSVC)
    target_compile_options(elizaos-registry PRIVATE /W4 /WX)
else()
    target_compile_options(elizaos-registry PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_link_libraries(elizaos-registry PUBLIC
    elizaos-core
    elizaos-agentlogger
    elizaos-plugins_automation
    nlohmann_json::nlohmann_json
)

if(CURL_FOUND)
    if(TARGET CURL::libcurl)
        target_link_libraries(elizaos-registry PRIVATE CURL::libcurl)
    else()
        target_link_libraries(elizaos-registry PRIVATE ${CURL_LIBRARIES})
        target_include_directories(elizaos-registry PRIVATE ${CURL_INCLUDE_DIRS})
    endif()
    target_compile_definitions(elizaos-registry PRIVATE LIBCURL_FOUND)
endif()
