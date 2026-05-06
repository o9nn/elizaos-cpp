# CMakeLists.txt for autofun-server
cmake_minimum_required(VERSION 3.16)

# Collect all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.hpp" "include/*.hpp")

if(SOURCES)
    add_library(elizaos-autofun-server STATIC ${SOURCES})

    target_include_directories(elizaos-autofun-server PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/cpp/include
    )

    target_link_libraries(elizaos-autofun-server
        nlohmann_json::nlohmann_json
        Threads::Threads
    )

    set_property(TARGET elizaos-autofun-server PROPERTY CXX_STANDARD 17)

    # Suppress warnings for transpiled code
    if(MSVC)
        target_compile_options(elizaos-autofun-server PRIVATE /W0)
    else()
        target_compile_options(elizaos-autofun-server PRIVATE -w)
    endif()

    install(TARGETS elizaos-autofun-server
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
    )
else()
    message(STATUS "No source files found for autofun-server, skipping")
endif()
