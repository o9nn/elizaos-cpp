add_library(elizaos-otaku STATIC
    src/otaku.cpp
)
target_compile_features(elizaos-otaku PUBLIC cxx_std_17)
target_include_directories(elizaos-otaku PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)

target_link_libraries(elizaos-otaku
    elizaos-core
    elizaos-agentlogger
    Threads::Threads
)
