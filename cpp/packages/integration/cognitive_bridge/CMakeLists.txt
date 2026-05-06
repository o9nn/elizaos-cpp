# CognitiveBridge module — in-process WebSocket-style pub/sub bridge for
# cognitive state, sensory input, speech output, and Echobeats heartbeat.
add_library(elizaos-cognitive_bridge STATIC
    src/cognitive_bridge.cpp
)

target_include_directories(elizaos-cognitive_bridge PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)

target_link_libraries(elizaos-cognitive_bridge PUBLIC
    Threads::Threads
)

set_target_properties(elizaos-cognitive_bridge PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

install(TARGETS elizaos-cognitive_bridge
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)
