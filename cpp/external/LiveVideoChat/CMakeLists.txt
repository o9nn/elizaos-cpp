# CMakeLists.txt for livevideochat2
cmake_minimum_required(VERSION 3.16)

# Collect all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.hpp" "include/*.hpp")

if(SOURCES)
    add_library(elizaos-livevideochat2 STATIC ${SOURCES})

    target_include_directories(elizaos-livevideochat2 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/cpp/include
    )

    target_link_libraries(elizaos-livevideochat2
        nlohmann_json::nlohmann_json
        Threads::Threads
    )

    set_property(TARGET elizaos-livevideochat2 PROPERTY CXX_STANDARD 17)

    # Suppress warnings for transpiled code
    if(MSVC)
        target_compile_options(elizaos-livevideochat2 PRIVATE /W0)
    else()
        target_compile_options(elizaos-livevideochat2 PRIVATE -w)
    endif()

    install(TARGETS elizaos-livevideochat2
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
    )
else()
    message(STATUS "No source files found for livevideochat2, skipping")
endif()
