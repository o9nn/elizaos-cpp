# CMakeLists.txt for eliza-server
cmake_minimum_required(VERSION 3.16)

# Collect all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.hpp" "include/*.hpp")

if(SOURCES)
    add_library(elizaos-eliza-server STATIC ${SOURCES})

    target_include_directories(elizaos-eliza-server PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/cpp/include
    )

    target_link_libraries(elizaos-eliza-server
        nlohmann_json::nlohmann_json
        Threads::Threads
    )

    set_property(TARGET elizaos-eliza-server PROPERTY CXX_STANDARD 17)

    # Suppress warnings for transpiled code
    if(MSVC)
        target_compile_options(elizaos-eliza-server PRIVATE /W0)
    else()
        target_compile_options(elizaos-eliza-server PRIVATE -w)
    endif()

    install(TARGETS elizaos-eliza-server
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
    )
else()
    message(STATUS "No source files found for eliza-server, skipping")
endif()
