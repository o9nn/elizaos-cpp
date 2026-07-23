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

# Try to link libcurl if available (for HTTP requests)
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(CURL QUIET libcurl)
endif()

if(NOT CURL_FOUND)
    find_package(CURL QUIET)
endif()

if(CURL_FOUND)
    if(TARGET CURL::libcurl)
        target_link_libraries(elizaos-easycompletion CURL::libcurl)
    else()
        target_link_libraries(elizaos-easycompletion ${CURL_LIBRARIES})
        target_include_directories(elizaos-easycompletion PRIVATE ${CURL_INCLUDE_DIRS})
    endif()
    target_compile_definitions(elizaos-easycompletion PRIVATE HAVE_CURL)
endif()

# For JSON handling - already available in the main project
target_link_libraries(elizaos-easycompletion nlohmann_json::nlohmann_json)
