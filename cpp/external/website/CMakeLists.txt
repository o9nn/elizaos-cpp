# Stage 5 - Web and Documentation - Website module
add_library(elizaos-website STATIC
    src/website.cpp
)
target_compile_features(elizaos-website PUBLIC cxx_std_17)

target_include_directories(elizaos-website PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)

target_link_libraries(elizaos-website 
    elizaos-core
    elizaos-agentlogger
    Threads::Threads
)

if(MSVC)
    target_compile_options(elizaos-website PRIVATE /W4 /WX)
else()
    target_compile_options(elizaos-website PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

if(WIN32)
    target_link_libraries(elizaos-website ws2_32)
endif()
