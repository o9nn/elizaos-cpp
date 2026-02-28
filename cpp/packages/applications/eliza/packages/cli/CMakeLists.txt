# CMakeLists.txt for eliza-cli
cmake_minimum_required(VERSION 3.16)

# Collect all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.hpp" "include/*.hpp")

if(SOURCES)
    add_library(elizaos-eliza-cli STATIC ${SOURCES})

    target_include_directories(elizaos-eliza-cli PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/cpp/include
    )

    target_link_libraries(elizaos-eliza-cli
        nlohmann_json::nlohmann_json
        Threads::Threads
    )

    set_property(TARGET elizaos-eliza-cli PROPERTY CXX_STANDARD 17)

    # Suppress warnings for transpiled code
    if(MSVC)
        target_compile_options(elizaos-eliza-cli PRIVATE /W0)
    else()
        target_compile_options(elizaos-eliza-cli PRIVATE -w)
    endif()

    install(TARGETS elizaos-eliza-cli
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
    )
else()
    message(STATUS "No source files found for eliza-cli, skipping")
endif()
