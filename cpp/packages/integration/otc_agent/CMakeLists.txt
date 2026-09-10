add_library(elizaos-otc_agent STATIC
    src/otc_agent.cpp
)
target_compile_features(elizaos-otc_agent PUBLIC cxx_std_17)

target_include_directories(elizaos-otc_agent PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)

target_link_libraries(elizaos-otc_agent
    elizaos-core
    elizaos-agentlogger
    Threads::Threads
)

if(MSVC)
    target_compile_options(elizaos-otc_agent PRIVATE /W4 /WX)
else()
    target_compile_options(elizaos-otc_agent PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
