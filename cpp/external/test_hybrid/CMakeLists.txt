# CMakeLists.txt for test_hybrid
cmake_minimum_required(VERSION 3.16)

# Collect all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.hpp" "include/*.hpp")

if(SOURCES)
    add_library(elizaos-test_hybrid STATIC ${SOURCES})

    target_include_directories(elizaos-test_hybrid PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/cpp/include
    )

    target_link_libraries(elizaos-test_hybrid
        nlohmann_json::nlohmann_json
        Threads::Threads
    )

    set_property(TARGET elizaos-test_hybrid PROPERTY CXX_STANDARD 17)

    # Suppress warnings for transpiled code
    if(MSVC)
        target_compile_options(elizaos-test_hybrid PRIVATE /W0)
    else()
        target_compile_options(elizaos-test_hybrid PRIVATE -w)
    endif()

    install(TARGETS elizaos-test_hybrid
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
    )
else()
    message(STATUS "No source files found for test_hybrid, skipping")
endif()
