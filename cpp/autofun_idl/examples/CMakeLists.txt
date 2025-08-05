# Example executable for autofun_idl
add_executable(autofun_idl_demo
    autofun_idl_demo.cpp
)

target_include_directories(autofun_idl_demo PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(autofun_idl_demo 
    elizaos-autofun_idl
    elizaos-core
)