# Stage 5 - Web and Documentation - Vercel API module
add_library(elizaos-vercel_api STATIC
    src/vercel_api.cpp
)

target_include_directories(elizaos-vercel_api PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)

target_link_libraries(elizaos-vercel_api
    elizaos-core
    elizaos-agentlogger
    nlohmann_json::nlohmann_json
)

target_compile_features(elizaos-vercel_api PUBLIC cxx_std_17)
if(MSVC)
    target_compile_options(elizaos-vercel_api PRIVATE /W4)
else()
    target_compile_options(elizaos-vercel_api PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Add test executable
add_executable(test_vercel_api test_vercel_api.cpp)
target_link_libraries(test_vercel_api elizaos-vercel_api)
if(MSVC)
    target_compile_options(test_vercel_api PRIVATE /W4)
else()
    target_compile_options(test_vercel_api PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Add demo executable
add_executable(demo_vercel_api demo_vercel_api.cpp)
target_link_libraries(demo_vercel_api elizaos-vercel_api)

# Add test to CTest
add_test(NAME VercelAPITest COMMAND test_vercel_api)
