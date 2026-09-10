# Stage 4 - Multimedia - LiveVideoChat implementation with server support
add_library(elizaos-livevideochat STATIC
    src/livevideochat.cpp
    src/livevideochat_server.cpp
)
target_compile_features(elizaos-livevideochat PUBLIC cxx_std_17)
target_include_directories(elizaos-livevideochat PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)

target_link_libraries(elizaos-livevideochat 
    elizaos-core
    elizaos-agentlogger
    nlohmann_json::nlohmann_json
    Threads::Threads
)
