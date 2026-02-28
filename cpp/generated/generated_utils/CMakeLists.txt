# CMakeLists.txt for generated_utils
cmake_minimum_required(VERSION 3.16)

# Collect all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.hpp")

if(SOURCES)
    # Create library
    add_library(elizaos-generated_utils STATIC ${SOURCES})

    # Include directories
    target_include_directories(elizaos-generated_utils PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/cpp/include
    )

    # Link dependencies
    target_link_libraries(elizaos-generated_utils
        nlohmann_json::nlohmann_json
        Threads::Threads
    )

    # Set C++ standard
    set_property(TARGET elizaos-generated_utils PROPERTY CXX_STANDARD 17)

    # Suppress warnings for auto-generated code
    if(MSVC)
        target_compile_options(elizaos-generated_utils PRIVATE /W0)
    else()
        target_compile_options(elizaos-generated_utils PRIVATE -w)
    endif()

    # Install
    install(TARGETS elizaos-generated_utils
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
    )
    install(DIRECTORY include/ DESTINATION include/elizaos/generated_utils
        FILES_MATCHING PATTERN "*.hpp"
    )
else()
    message(STATUS "No source files found for generated_utils, skipping")
endif()
