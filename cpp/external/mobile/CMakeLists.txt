# CMakeLists.txt for mobile
cmake_minimum_required(VERSION 3.16)

# Collect all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.hpp" "include/*.hpp")

if(SOURCES)
    add_library(elizaos-mobile STATIC ${SOURCES})

    target_include_directories(elizaos-mobile PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/cpp/include
    )

    target_link_libraries(elizaos-mobile
        nlohmann_json::nlohmann_json
        Threads::Threads
    )

    set_property(TARGET elizaos-mobile PROPERTY CXX_STANDARD 17)

    # Suppress warnings for transpiled code
    if(MSVC)
        target_compile_options(elizaos-mobile PRIVATE /W0)
    else()
        target_compile_options(elizaos-mobile PRIVATE -w)
    endif()

    install(TARGETS elizaos-mobile
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
    )
else()
    message(STATUS "No source files found for mobile, skipping")
endif()
