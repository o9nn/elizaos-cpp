# ElizaOS DreamGen/KoboldCpp transport-injected provider utilities

option(ELIZAOS_DGEN_KOBOLD_ENABLE_LIBCURL
       "Build the explicit libcurl production transport adapter" ON)

add_library(elizaos-dgen_kobold_provider STATIC
    src/plugin.cpp
    src/dreamgen_provider.cpp
    src/koboldcpp_provider.cpp
    src/http_client.cpp
)

target_include_directories(elizaos-dgen_kobold_provider PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(elizaos-dgen_kobold_provider
    PUBLIC nlohmann_json::nlohmann_json
    PRIVATE elizaos-core
)

target_compile_features(elizaos-dgen_kobold_provider PUBLIC cxx_std_17)
if(MSVC)
    target_compile_options(elizaos-dgen_kobold_provider PRIVATE /W4 /WX)
else()
    target_compile_options(elizaos-dgen_kobold_provider PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Werror)
endif()

if(ELIZAOS_DGEN_KOBOLD_ENABLE_LIBCURL AND CURL_FOUND)
    if(TARGET CURL::libcurl)
        target_link_libraries(elizaos-dgen_kobold_provider PRIVATE CURL::libcurl)
    else()
        target_link_libraries(elizaos-dgen_kobold_provider PRIVATE ${CURL_LIBRARIES})
        target_include_directories(elizaos-dgen_kobold_provider PRIVATE ${CURL_INCLUDE_DIRS})
    endif()
    target_compile_definitions(elizaos-dgen_kobold_provider
        PRIVATE ELIZAOS_DGEN_KOBOLD_HAS_LIBCURL=1)
else()
    target_compile_definitions(elizaos-dgen_kobold_provider
        PRIVATE ELIZAOS_DGEN_KOBOLD_HAS_LIBCURL=0)
endif()
