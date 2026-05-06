# CMakeLists.txt for sandbox_template_cloud
cmake_minimum_required(VERSION 3.16)

# Collect all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.hpp" "include/*.hpp")

if(SOURCES)
    add_library(elizaos-sandbox_template_cloud STATIC ${SOURCES})

    target_include_directories(elizaos-sandbox_template_cloud PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/cpp/include
    )

    target_link_libraries(elizaos-sandbox_template_cloud
        nlohmann_json::nlohmann_json
        Threads::Threads
    )

    set_property(TARGET elizaos-sandbox_template_cloud PROPERTY CXX_STANDARD 17)

    # Suppress warnings for transpiled code
    if(MSVC)
        target_compile_options(elizaos-sandbox_template_cloud PRIVATE /W0)
    else()
        target_compile_options(elizaos-sandbox_template_cloud PRIVATE -w)
    endif()

    install(TARGETS elizaos-sandbox_template_cloud
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
    )
else()
    message(STATUS "No source files found for sandbox_template_cloud, skipping")
endif()
