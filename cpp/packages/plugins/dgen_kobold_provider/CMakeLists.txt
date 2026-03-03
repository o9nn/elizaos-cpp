# ElizaOS Dgen/KoboldCpp Provider Plugin
# Provides DreamGen cloud API and KoboldCpp local inference as model providers

add_library(elizaos-dgen_kobold_provider STATIC
    src/plugin.cpp
    src/dreamgen_provider.cpp
    src/koboldcpp_provider.cpp
    src/http_client.cpp
)

target_include_directories(elizaos-dgen_kobold_provider PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)

target_link_libraries(elizaos-dgen_kobold_provider
    elizaos-core
    nlohmann_json::nlohmann_json
)

# Link libcurl if available (for HTTP client)
if(CURL_FOUND)
    target_link_libraries(elizaos-dgen_kobold_provider ${CURL_LIBRARIES})
    target_include_directories(elizaos-dgen_kobold_provider PRIVATE ${CURL_INCLUDE_DIRS})
    target_compile_definitions(elizaos-dgen_kobold_provider PRIVATE HAS_LIBCURL=1)
endif()
