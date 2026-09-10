# Stage 6 - Tools and Automation - Discrub Extension
# discrub_ext.cpp owns classification/moderation; discrub_ext_complete.cpp owns
# transport-backed cleanup, analytics, persistence, and extension composition.
add_library(elizaos-discrub_ext STATIC
    src/discrub_ext.cpp
    src/discrub_ext_complete.cpp
)

target_include_directories(elizaos-discrub_ext PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)

target_link_libraries(elizaos-discrub_ext
    PUBLIC
        elizaos-discord_summarizer
    PRIVATE
        elizaos-core
        nlohmann_json::nlohmann_json
        Threads::Threads
)

target_compile_features(elizaos-discrub_ext PUBLIC cxx_std_17)
if(MSVC)
    target_compile_options(elizaos-discrub_ext PRIVATE /W4 /WX)
else()
    target_compile_options(elizaos-discrub_ext PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Werror)
endif()
