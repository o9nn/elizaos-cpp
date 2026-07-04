add_library(elizaos-endocrine STATIC
    src/endocrine.cpp
)
target_include_directories(elizaos-endocrine PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
target_link_libraries(elizaos-endocrine PUBLIC
    nlohmann_json::nlohmann_json
)
