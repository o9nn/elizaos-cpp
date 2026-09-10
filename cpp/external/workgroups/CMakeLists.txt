add_library(elizaos-workgroups
    src/workgroups.cpp
)

target_include_directories(elizaos-workgroups PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)

set(ELIZAOS_WORKGROUPS_CXX_STANDARD "17" CACHE STRING
    "C++ standard for Workgroups (17 or 20)")
set_property(CACHE ELIZAOS_WORKGROUPS_CXX_STANDARD PROPERTY STRINGS 17 20)
if(NOT ELIZAOS_WORKGROUPS_CXX_STANDARD MATCHES "^(17|20)$")
    message(FATAL_ERROR "ELIZAOS_WORKGROUPS_CXX_STANDARD must be 17 or 20")
endif()
set_target_properties(elizaos-workgroups PROPERTIES
    CXX_STANDARD ${ELIZAOS_WORKGROUPS_CXX_STANDARD}
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)

if(MSVC)
    target_compile_options(elizaos-workgroups PRIVATE /W4 /WX)
else()
    target_compile_options(elizaos-workgroups PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_link_libraries(elizaos-workgroups PUBLIC
    elizaos-core
    elizaos-agentcomms
    elizaos-agentmemory
    elizaos-agentagenda
    elizaos-agentlogger
    nlohmann_json::nlohmann_json
)
