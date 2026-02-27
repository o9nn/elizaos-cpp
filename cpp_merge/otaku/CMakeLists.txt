# otaku Module - Merged Implementation
# Combined transpiled + cpp_core implementation

cmake_minimum_required(VERSION 3.16)

# Module configuration
set(MODULE_NAME otaku_merge)

# Collect all source files
file(GLOB_RECURSE MODULE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

# Collect all header files  
file(GLOB_RECURSE MODULE_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

# Create the library (only if we have sources)
list(LENGTH MODULE_SOURCES SOURCE_COUNT)
if(SOURCE_COUNT GREATER 0)
    add_library(${MODULE_NAME} STATIC ${MODULE_SOURCES} ${MODULE_HEADERS})
    
    # Include directories
    target_include_directories(${MODULE_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp_core
        ${CMAKE_SOURCE_DIR}/../cpp/include/elizaos
        ${CMAKE_SOURCE_DIR}/../cpp/include
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
    
    # Set properties
    set_target_properties(${MODULE_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        POSITION_INDEPENDENT_CODE ON
    )
    
    message(STATUS "Configured: ${MODULE_NAME}")
else()
    message(STATUS "Skipping: ${MODULE_NAME} (no source files)")
endif()
