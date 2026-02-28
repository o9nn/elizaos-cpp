# CMakeLists.txt for generated_cli
cmake_minimum_required(VERSION 3.16)

# Collect all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.hpp")

if(SOURCES)
    # Create library
    add_library(elizaos-generated_cli STATIC ${SOURCES})

    # Include directories
    target_include_directories(elizaos-generated_cli PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/cpp/include
    )

    # Link dependencies
    target_link_libraries(elizaos-generated_cli
        nlohmann_json::nlohmann_json
        Threads::Threads
    )

    # Set C++ standard
    set_property(TARGET elizaos-generated_cli PROPERTY CXX_STANDARD 17)

    # Suppress warnings for auto-generated code
    if(MSVC)
        target_compile_options(elizaos-generated_cli PRIVATE /W0)
    else()
        target_compile_options(elizaos-generated_cli PRIVATE -w)
    endif()

    # Install
    install(TARGETS elizaos-generated_cli
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
    )
    install(DIRECTORY include/ DESTINATION include/elizaos/generated_cli
        FILES_MATCHING PATTERN "*.hpp"
    )
else()
    message(STATUS "No source files found for generated_cli, skipping")
endif()
