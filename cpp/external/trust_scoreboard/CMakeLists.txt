add_library(elizaos-trust_scoreboard
    src/trust_scoreboard.cpp
)

add_library(elizaos-trust-scoreboard ALIAS elizaos-trust_scoreboard)

target_compile_features(elizaos-trust_scoreboard PUBLIC cxx_std_17)

target_include_directories(elizaos-trust_scoreboard PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)

target_link_libraries(elizaos-trust_scoreboard
    PUBLIC
        elizaos-core
    PRIVATE
        nlohmann_json::nlohmann_json
)

if(MSVC)
    target_compile_options(elizaos-trust_scoreboard PRIVATE /W4 /WX)
else()
    target_compile_options(elizaos-trust_scoreboard PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Werror
    )
endif()
