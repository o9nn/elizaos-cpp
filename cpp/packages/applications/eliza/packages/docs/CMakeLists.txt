# CMakeLists.txt for eliza-docs
cmake_minimum_required(VERSION 3.16)

# Collect all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.hpp" "include/*.hpp")

if(SOURCES)
    add_library(elizaos-eliza-docs STATIC ${SOURCES})

    target_include_directories(elizaos-eliza-docs PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/cpp/include
    )

    target_link_libraries(elizaos-eliza-docs
        nlohmann_json::nlohmann_json
        Threads::Threads
    )

    set_property(TARGET elizaos-eliza-docs PROPERTY CXX_STANDARD 17)

    # Suppress warnings for transpiled code
    if(MSVC)
        target_compile_options(elizaos-eliza-docs PRIVATE /W0)
    else()
        target_compile_options(elizaos-eliza-docs PRIVATE -w)
    endif()

    install(TARGETS elizaos-eliza-docs
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
    )
else()
    message(STATUS "No source files found for eliza-docs, skipping")
endif()
