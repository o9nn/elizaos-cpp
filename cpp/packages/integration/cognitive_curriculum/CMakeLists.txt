# Cognitive Curriculum + Homework Loop
# Teaches Eliza the 61 concepts / 12 KSM steps / 15 Alexander properties and gives it a
# bounded, aligned self-strengthening "homework loop" between developmental phases.
add_library(elizaos-cognitive_curriculum STATIC
    src/cognitive_curriculum.cpp
    src/homework_loop.cpp
)
target_include_directories(elizaos-cognitive_curriculum PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
target_link_libraries(elizaos-cognitive_curriculum
    elizaos-autonomous_starter
    elizaos-core
    elizaos-agentloop
    elizaos-agentshell
    elizaos-agentlogger
    Threads::Threads
)
