# Ontogenesis Module - Self-Generating Kernels
# Implementation of self-generating, evolving kernels through recursive application of differential operators

add_library(elizaos-ontogenesis STATIC
    ${CMAKE_SOURCE_DIR}/src/ontogenesis.cpp
)

target_include_directories(elizaos-ontogenesis PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(elizaos-ontogenesis PUBLIC
    Threads::Threads
)

# Set properties
set_target_properties(elizaos-ontogenesis PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Install rules
install(TARGETS elizaos-ontogenesis
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES
    ${CMAKE_SOURCE_DIR}/include/elizaos/ontogenesis.hpp
    DESTINATION include/elizaos
)
