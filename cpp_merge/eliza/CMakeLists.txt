# Eliza Module - Core Conversation Engine
# Combined transpiled + cpp_core implementation

cmake_minimum_required(VERSION 3.16)

# Module configuration
set(MODULE_NAME eliza_merge)

# Collect all source files
file(GLOB_RECURSE MODULE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

# Collect all header files
file(GLOB_RECURSE MODULE_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

# Create the library
add_library(${MODULE_NAME} STATIC ${MODULE_SOURCES} ${MODULE_HEADERS})

# Include directories
target_include_directories(${MODULE_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp_core
    ${CMAKE_CURRENT_SOURCE_DIR}/packages
)

# Link to core elizaos if available
if(TARGET elizaos_core)
    target_link_libraries(${MODULE_NAME} PUBLIC elizaos_core)
endif()

# Export the target
set_target_properties(${MODULE_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)

message(STATUS "Configured: ${MODULE_NAME} with ${CMAKE_CURRENT_SOURCE_DIR}")
