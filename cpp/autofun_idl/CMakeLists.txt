# autofun_idl - IDL processing for auto.fun protocol
add_library(elizaos-autofun_idl STATIC
    src/autofun_idl.cpp
)

target_include_directories(elizaos-autofun_idl PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(elizaos-autofun_idl 
    elizaos-core
    nlohmann_json::nlohmann_json
)

# Add examples subdirectory  
add_subdirectory(examples)
