cmake_minimum_required(VERSION 3.16)
project(elizaos_installed_consumer LANGUAGES CXX)

find_package(ElizaOS 1.0 CONFIG REQUIRED)

add_executable(elizaos_installed_consumer main.cpp)
target_compile_features(elizaos_installed_consumer PRIVATE cxx_std_17)
target_link_libraries(elizaos_installed_consumer PRIVATE
    ElizaOS::cognitive_curriculum
    ElizaOS::llama_cpp
    ElizaOS::cognitive_bridge
    ElizaOS::persistence
)
