# CMakeLists.txt for aum_tracker
cmake_minimum_required(VERSION 3.16)

# Collect all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.hpp" "include/*.hpp")

if(SOURCES)
    add_library(elizaos-aum_tracker STATIC ${SOURCES})

    target_include_directories(elizaos-aum_tracker PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/cpp/include
    )

    target_link_libraries(elizaos-aum_tracker
        nlohmann_json::nlohmann_json
        Threads::Threads
    )

    set_property(TARGET elizaos-aum_tracker PROPERTY CXX_STANDARD 17)

    # Suppress warnings for transpiled code
    if(MSVC)
        target_compile_options(elizaos-aum_tracker PRIVATE /W0)
    else()
        target_compile_options(elizaos-aum_tracker PRIVATE -w)
    endif()

    install(TARGETS elizaos-aum_tracker
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
    )
else()
    message(STATUS "No source files found for aum_tracker, skipping")
endif()
