# CMakeLists.txt for autofun-types
cmake_minimum_required(VERSION 3.16)

# Collect all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.hpp" "include/*.hpp")

if(SOURCES)
    add_library(elizaos-autofun-types STATIC ${SOURCES})

    target_include_directories(elizaos-autofun-types PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/cpp/include
    )

    target_link_libraries(elizaos-autofun-types
        nlohmann_json::nlohmann_json
        Threads::Threads
    )

    set_property(TARGET elizaos-autofun-types PROPERTY CXX_STANDARD 17)

    # Suppress warnings for transpiled code
    if(MSVC)
        target_compile_options(elizaos-autofun-types PRIVATE /W0)
    else()
        target_compile_options(elizaos-autofun-types PRIVATE -w)
    endif()

    install(TARGETS elizaos-autofun-types
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
    )
else()
    message(STATUS "No source files found for autofun-types, skipping")
endif()
