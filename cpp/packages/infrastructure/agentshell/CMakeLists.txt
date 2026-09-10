# Stage 2 - Infrastructure: AgentShell Implementation
add_library(elizaos-agentshell STATIC
    src/agentshell.cpp
)
target_compile_features(elizaos-agentshell PUBLIC cxx_std_17)

target_include_directories(elizaos-agentshell PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)

target_link_libraries(elizaos-agentshell
    elizaos-core
    elizaos-agentlogger
    Threads::Threads
)

if(MSVC)
    target_compile_options(elizaos-agentshell PRIVATE /W4 /WX)
else()
    target_compile_options(elizaos-agentshell PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Werror)
endif()
