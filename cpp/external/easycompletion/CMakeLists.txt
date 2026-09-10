# EasyCompletion module - C++ implementation of easy text and chat completion
add_library(elizaos-easycompletion STATIC
    src/easycompletion.cpp
)

target_include_directories(elizaos-easycompletion PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)

target_link_libraries(elizaos-easycompletion 
    elizaos-core
)

# Link libcurl through its configuration-aware imported target when available.
if(NOT TARGET CURL::libcurl)
    find_package(CURL QUIET)
endif()

if(TARGET CURL::libcurl)
    target_link_libraries(elizaos-easycompletion CURL::libcurl)
    target_compile_definitions(elizaos-easycompletion PRIVATE HAVE_CURL)
endif()

# For JSON handling - already available in the main project
target_link_libraries(elizaos-easycompletion nlohmann_json::nlohmann_json)
