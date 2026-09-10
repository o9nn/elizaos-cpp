# Stage 6 - Tools and Automation - Plugins Automation
add_library(elizaos-plugins_automation STATIC
    src/plugins_automation.cpp
)

target_compile_features(elizaos-plugins_automation PUBLIC cxx_std_17)

target_include_directories(elizaos-plugins_automation PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)

target_link_libraries(elizaos-plugins_automation
    PUBLIC elizaos-core Threads::Threads
    PRIVATE elizaos-agentlogger
)
