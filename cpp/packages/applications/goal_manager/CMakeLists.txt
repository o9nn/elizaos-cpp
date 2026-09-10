add_library(elizaos-goal_manager STATIC
    src/goal_manager.cpp
)
target_compile_features(elizaos-goal_manager PUBLIC cxx_std_17)
target_include_directories(elizaos-goal_manager PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
target_link_libraries(elizaos-goal_manager PUBLIC
    Threads::Threads
)
if(MSVC)
    target_compile_options(elizaos-goal_manager PRIVATE /W4 /WX)
else()
    target_compile_options(elizaos-goal_manager PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
