# Stage 3 - Spartan DeFi Trading Agent
add_library(elizaos-spartan STATIC
    src/spartan.cpp
)

target_include_directories(elizaos-spartan PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
target_compile_features(elizaos-spartan PUBLIC cxx_std_17)
if(MSVC)
    target_compile_options(elizaos-spartan PRIVATE /W4 /WX)
else()
    target_compile_options(elizaos-spartan PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
target_link_libraries(elizaos-spartan
    elizaos-core
)
