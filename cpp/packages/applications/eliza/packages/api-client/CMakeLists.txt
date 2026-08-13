# CMakeLists.txt for eliza-api-client
cmake_minimum_required(VERSION 3.16)

# CurlTransport provides the real HTTP backend for the API client.
if(NOT TARGET CURL::libcurl)
    find_package(CURL REQUIRED)
endif()

# Collect all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.hpp" "include/*.hpp")

if(SOURCES)
    add_library(elizaos-eliza-api-client STATIC ${SOURCES})

    target_include_directories(elizaos-eliza-api-client PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/cpp/include
    )

    target_link_libraries(elizaos-eliza-api-client
        nlohmann_json::nlohmann_json
        Threads::Threads
        CURL::libcurl
    )

    set_property(TARGET elizaos-eliza-api-client PROPERTY CXX_STANDARD 17)

    # Suppress warnings for transpiled code
    if(MSVC)
        target_compile_options(elizaos-eliza-api-client PRIVATE /W0)
    else()
        target_compile_options(elizaos-eliza-api-client PRIVATE -w)
    endif()

    install(TARGETS elizaos-eliza-api-client
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
    )
else()
    message(STATUS "No source files found for eliza-api-client, skipping")
endif()
