add_library(elizaos-evolutionary STATIC
    src/evolutionary.cpp
    src/pattern_extraction.cpp
    src/adaptation_hooks_simple.cpp
)
target_compile_features(elizaos-evolutionary PUBLIC cxx_std_17)

target_include_directories(elizaos-evolutionary PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)

if(MSVC)
    target_compile_options(elizaos-evolutionary PRIVATE /W4 /WX)
else()
    target_compile_options(elizaos-evolutionary PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_link_libraries(elizaos-evolutionary
    elizaos-core
    Threads::Threads
)
