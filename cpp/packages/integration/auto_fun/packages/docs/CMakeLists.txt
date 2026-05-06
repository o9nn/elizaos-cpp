# CMakeLists.txt for autofun-docs
cmake_minimum_required(VERSION 3.16)

# Collect all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.hpp" "include/*.hpp")

if(SOURCES)
    add_library(elizaos-autofun-docs STATIC ${SOURCES})

    target_include_directories(elizaos-autofun-docs PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/cpp/include
    )

    target_link_libraries(elizaos-autofun-docs
        nlohmann_json::nlohmann_json
        Threads::Threads
    )

    set_property(TARGET elizaos-autofun-docs PROPERTY CXX_STANDARD 17)

    # Suppress warnings for transpiled code
    if(MSVC)
        target_compile_options(elizaos-autofun-docs PRIVATE /W0)
    else()
        target_compile_options(elizaos-autofun-docs PRIVATE -w)
    endif()

    install(TARGETS elizaos-autofun-docs
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
    )
else()
    message(STATUS "No source files found for autofun-docs, skipping")
endif()
