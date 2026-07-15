# Task 10.1.1 llama.cpp integration — reference engine implementation.
# The public API (include/elizaos/llama_cpp.hpp) is backend-agnostic; this
# package provides the self-contained CPU reference backend. Linking the real
# llama.cpp library replaces only the Impl structs in llama_cpp.cpp.
add_library(elizaos-llama_cpp STATIC
    src/llama_cpp.cpp
)

target_include_directories(elizaos-llama_cpp PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(elizaos-llama_cpp
    Threads::Threads
)
