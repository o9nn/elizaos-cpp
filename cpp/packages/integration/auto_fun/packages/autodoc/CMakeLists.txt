# CMakeLists.txt for autofun-autodoc
cmake_minimum_required(VERSION 3.16)

# Collect all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.hpp" "include/*.hpp")

if(SOURCES)
    add_library(elizaos-autofun-autodoc STATIC ${SOURCES})

    target_include_directories(elizaos-autofun-autodoc PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/cpp/include
    )

    target_link_libraries(elizaos-autofun-autodoc
        nlohmann_json::nlohmann_json
        Threads::Threads
    )

    set_property(TARGET elizaos-autofun-autodoc PROPERTY CXX_STANDARD 17)

    # Suppress warnings for transpiled code
    if(MSVC)
        target_compile_options(elizaos-autofun-autodoc PRIVATE /W0)
    else()
        target_compile_options(elizaos-autofun-autodoc PRIVATE -w)
    endif()

    install(TARGETS elizaos-autofun-autodoc
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
    )
else()
    message(STATUS "No source files found for autofun-autodoc, skipping")
endif()
