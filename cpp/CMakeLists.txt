# ElizaOS C++ - Main Build Configuration
# Structure aligned with TypeScript packages layout

cmake_minimum_required(VERSION 3.16)
project(ElizaOS_CPP VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ==============================================================================
# Package Structure (mirrors TypeScript eliza/packages/)
# ==============================================================================

# Core modules - fundamental framework components
add_subdirectory(packages/core/core)
add_subdirectory(packages/core/agentloop)
add_subdirectory(packages/core/agentmemory)
add_subdirectory(packages/core/agentaction)
add_subdirectory(packages/core/agentagenda)

# Infrastructure modules - support services
add_subdirectory(packages/infrastructure/agentcomms)
add_subdirectory(packages/infrastructure/agentlogger)
add_subdirectory(packages/infrastructure/agentshell)
add_subdirectory(packages/infrastructure/agentbrowser)

# Application modules - main application logic
add_subdirectory(packages/applications/eliza)
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

# Integration modules - complex external integrations
add_subdirectory(packages/integration/auto_fun)
add_subdirectory(packages/integration/autonomous_starter)
add_subdirectory(packages/integration/SWEagent)
add_subdirectory(packages/integration/sweagent)
add_subdirectory(packages/integration/otaku)
add_subdirectory(packages/integration/otc_agent)
add_subdirectory(packages/integration/mcp_gateway)

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
