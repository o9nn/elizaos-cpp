# CMakeLists.txt for generated_websocket
cmake_minimum_required(VERSION 3.16)

# Collect all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.hpp")

if(SOURCES)
    # Create library
    add_library(elizaos-generated_websocket STATIC ${SOURCES})

    # Include directories
    target_include_directories(elizaos-generated_websocket PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/cpp/include
    )

    # Link dependencies
    target_link_libraries(elizaos-generated_websocket
        nlohmann_json::nlohmann_json
        Threads::Threads
    )

    # Set C++ standard
    set_property(TARGET elizaos-generated_websocket PROPERTY CXX_STANDARD 17)

    # Suppress warnings for auto-generated code
    if(MSVC)
        target_compile_options(elizaos-generated_websocket PRIVATE /W0)
    else()
        target_compile_options(elizaos-generated_websocket PRIVATE -w)
    endif()

    # Install
    install(TARGETS elizaos-generated_websocket
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
    )
    install(DIRECTORY include/ DESTINATION include/elizaos/generated_websocket
        FILES_MATCHING PATTERN "*.hpp"
    )
else()
    message(STATUS "No source files found for generated_websocket, skipping")
endif()
