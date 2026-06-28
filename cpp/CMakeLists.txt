# ElizaOS C++ - Main Build Configuration
# Structure aligned with TypeScript packages layout

cmake_minimum_required(VERSION 3.16)
project(ElizaOS_CPP VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)
find_package(CURL REQUIRED)
find_package(PkgConfig REQUIRED)
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

function(elizaos_add_subdirectory_if_exists relative_dir)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${relative_dir}/CMakeLists.txt")
        add_subdirectory(${relative_dir})
    else()
        message(STATUS "Skipping optional module without CMakeLists.txt: ${relative_dir}")
    endif()
endfunction()

# ==============================================================================
# Package Structure (mirrors TypeScript eliza/packages/)
# ==============================================================================

# Core modules - fundamental framework components
add_subdirectory(packages/core/core)
add_subdirectory(packages/core/agentloop)
add_subdirectory(packages/core/agentmemory)
add_subdirectory(packages/core/agentaction)
add_subdirectory(packages/core/agentagenda)
add_subdirectory(packages/core/persistence)

# Infrastructure modules - support services
add_subdirectory(packages/infrastructure/agentcomms)
add_subdirectory(packages/infrastructure/agentlogger)
add_subdirectory(packages/infrastructure/agentshell)
add_subdirectory(packages/infrastructure/agentbrowser)

# Application modules - main application logic
add_subdirectory(packages/applications/eliza)
elizaos_add_subdirectory_if_exists(packages/applications/eliza/packages/api-client)
add_subdirectory(packages/applications/characters)
add_subdirectory(packages/applications/knowledge)
add_subdirectory(packages/applications/goal_manager)

# Plugin modules
add_subdirectory(packages/plugins/eliza_plugin_starter)
add_subdirectory(packages/plugins/plugin_specification)
add_subdirectory(packages/plugins/plugins_automation)

# Starter templates
add_subdirectory(packages/starters/eliza_starter)
add_subdirectory(packages/starters/eliza_nextjs_starter)
add_subdirectory(packages/starters/eliza_3d_hyperfy_starter)

# ==============================================================================
# External modules - multimedia, web, tools, learning, and embodiment
# ==============================================================================
elizaos_add_subdirectory_if_exists(external/autofun_idl)
elizaos_add_subdirectory_if_exists(external/awesome_eliza)
elizaos_add_subdirectory_if_exists(external/brandkit)
elizaos_add_subdirectory_if_exists(external/characterfile)
elizaos_add_subdirectory_if_exists(external/classified)
elizaos_add_subdirectory_if_exists(external/easycompletion)
elizaos_add_subdirectory_if_exists(external/elizaos_github_io)
elizaos_add_subdirectory_if_exists(external/elizas_list)
elizaos_add_subdirectory_if_exists(external/elizas_world)
elizaos_add_subdirectory_if_exists(external/hat)
elizaos_add_subdirectory_if_exists(external/hats)
elizaos_add_subdirectory_if_exists(external/registry)
elizaos_add_subdirectory_if_exists(external/spartan)
elizaos_add_subdirectory_if_exists(external/the_org)
elizaos_add_subdirectory_if_exists(external/trust_scoreboard)
elizaos_add_subdirectory_if_exists(external/workgroups)
elizaos_add_subdirectory_if_exists(external/ljspeechtools)
elizaos_add_subdirectory_if_exists(external/livevideochat)
elizaos_add_subdirectory_if_exists(external/vercel_api)
elizaos_add_subdirectory_if_exists(external/website)
elizaos_add_subdirectory_if_exists(external/discord_summarizer)
elizaos_add_subdirectory_if_exists(external/discrub_ext)
elizaos_add_subdirectory_if_exists(external/evolutionary)
elizaos_add_subdirectory_if_exists(external/ontogenesis)
elizaos_add_subdirectory_if_exists(external/embodiment)

# Integration modules - complex external integrations
add_subdirectory(packages/integration/auto_fun)
add_subdirectory(packages/integration/autonomous_starter)
elizaos_add_subdirectory_if_exists(packages/integration/SWEagent)
elizaos_add_subdirectory_if_exists(packages/integration/sweagent)
add_subdirectory(packages/integration/otaku)
add_subdirectory(packages/integration/otc_agent)
add_subdirectory(packages/integration/mcp_gateway)
elizaos_add_subdirectory_if_exists(packages/integration/cognitive_bridge)

# ==============================================================================
# Generated Modules (from transpiler)
# ==============================================================================

add_subdirectory(generated/generated_services)
add_subdirectory(generated/generated_testing)
add_subdirectory(generated/generated_utils)
add_subdirectory(generated/generated_auth)
add_subdirectory(generated/generated_plugins)
add_subdirectory(generated/generated_ui)
add_subdirectory(generated/generated_database)
add_subdirectory(generated/generated_cli)
add_subdirectory(generated/generated_docs)
add_subdirectory(generated/generated_api)
add_subdirectory(generated/generated_websocket)
add_subdirectory(generated/generated_trade)
add_subdirectory(generated/generated_misc)

# ==============================================================================
# Tests
# ==============================================================================

enable_testing()
add_subdirectory(tests)
