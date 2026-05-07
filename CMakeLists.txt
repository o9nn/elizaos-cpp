cmake_minimum_required(VERSION 3.20)

project(ElizaOS-CPP 
    VERSION 1.0.0
    DESCRIPTION "C++ implementation of ElizaOS agent framework"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler-specific options - use warnings but NOT -Werror to allow builds to complete
if(MSVC)
    add_compile_options(/W3)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    endif()
endif()

# Find packages
find_package(Threads REQUIRED)

# Find libcurl for HTTP operations
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(CURL QUIET libcurl)
endif()

if(NOT CURL_FOUND)
    find_package(CURL QUIET)
endif()

# Enable testing
enable_testing()

# Add GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Add nlohmann/json for JSON parsing
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# Include directories - both top-level and cpp-level include dirs
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/cpp/include)

# ============================================================================
# Stage 1 - Core Logic
# ============================================================================
add_subdirectory(cpp/packages/core/core)
add_subdirectory(cpp/packages/core/agentloop)
add_subdirectory(cpp/packages/core/agentmemory)
add_subdirectory(cpp/packages/core/agentaction)
add_subdirectory(cpp/packages/core/agentagenda)

# ============================================================================
# Stage 2 - Infrastructure
# ============================================================================
add_subdirectory(cpp/packages/infrastructure/agentcomms)
add_subdirectory(cpp/packages/infrastructure/agentlogger)
add_subdirectory(cpp/packages/infrastructure/agentshell)
add_subdirectory(cpp/packages/infrastructure/agentbrowser)

# ============================================================================
# Stage 3 - Application-specific modules
# ============================================================================
add_subdirectory(cpp/packages/applications/eliza)
add_subdirectory(cpp/packages/applications/characters)
add_subdirectory(cpp/packages/applications/knowledge)
add_subdirectory(cpp/packages/applications/goal_manager)

# Integration modules
add_subdirectory(cpp/packages/integration/auto_fun)
add_subdirectory(cpp/packages/integration/autonomous_starter)
add_subdirectory(cpp/packages/integration/sweagent)
add_subdirectory(cpp/packages/integration/mcp_gateway)
add_subdirectory(cpp/packages/integration/cognitive_bridge)
add_subdirectory(cpp/packages/integration/otaku)
add_subdirectory(cpp/packages/integration/otc_agent)

# Plugin modules
add_subdirectory(cpp/packages/plugins/eliza_plugin_starter)
add_subdirectory(cpp/packages/plugins/plugin_specification)
add_subdirectory(cpp/packages/plugins/plugins_automation)

# Starter templates
add_subdirectory(cpp/packages/starters/eliza_starter)
add_subdirectory(cpp/packages/starters/eliza_nextjs_starter)
add_subdirectory(cpp/packages/starters/eliza_3d_hyperfy_starter)

# ============================================================================
# Stage 4 - External modules (multimedia, web, tools)
# ============================================================================
add_subdirectory(cpp/external/autofun_idl)
add_subdirectory(cpp/external/awesome_eliza)
add_subdirectory(cpp/external/brandkit)
add_subdirectory(cpp/external/characterfile)
add_subdirectory(cpp/external/classified)
add_subdirectory(cpp/external/easycompletion)
add_subdirectory(cpp/external/elizaos_github_io)
add_subdirectory(cpp/external/elizas_list)
add_subdirectory(cpp/external/elizas_world)
add_subdirectory(cpp/external/hat)
add_subdirectory(cpp/external/hats)
add_subdirectory(cpp/external/registry)
add_subdirectory(cpp/external/spartan)
add_subdirectory(cpp/external/the_org)
add_subdirectory(cpp/external/trust_scoreboard)
add_subdirectory(cpp/external/workgroups)

# Multimedia
add_subdirectory(cpp/external/ljspeechtools)
add_subdirectory(cpp/external/livevideochat)

# Web and Documentation
add_subdirectory(cpp/external/vercel_api)
add_subdirectory(cpp/external/website)

# Tools and Automation
add_subdirectory(cpp/external/discord_summarizer)
add_subdirectory(cpp/external/discrub_ext)

# Learning and Adaptation
add_subdirectory(cpp/external/evolutionary)
add_subdirectory(cpp/external/ontogenesis)

# Embodiment
add_subdirectory(cpp/external/embodiment)

# ============================================================================
# Generated Modules (from transpiler) - Fixed and enabled
# All broken transpiled code has been rewritten to valid C++17.
# ============================================================================
add_subdirectory(cpp/generated/generated_api)
add_subdirectory(cpp/generated/generated_auth)
add_subdirectory(cpp/generated/generated_cli)
add_subdirectory(cpp/generated/generated_database)
add_subdirectory(cpp/generated/generated_docs)
add_subdirectory(cpp/generated/generated_misc)
add_subdirectory(cpp/generated/generated_plugins)
add_subdirectory(cpp/generated/generated_services)
add_subdirectory(cpp/generated/generated_testing)
add_subdirectory(cpp/generated/generated_trade)
add_subdirectory(cpp/generated/generated_ui)
add_subdirectory(cpp/generated/generated_utils)
add_subdirectory(cpp/generated/generated_websocket)

# ============================================================================
# Eliza Sub-Packages (core platform components)
# ============================================================================
add_subdirectory(cpp/packages/applications/eliza/packages/api-client)
add_subdirectory(cpp/packages/applications/eliza/packages/app)
add_subdirectory(cpp/packages/applications/eliza/packages/autodoc)
add_subdirectory(cpp/packages/applications/eliza/packages/cli)
add_subdirectory(cpp/packages/applications/eliza/packages/client)
add_subdirectory(cpp/packages/applications/eliza/packages/core)
add_subdirectory(cpp/packages/applications/eliza/packages/docs)
add_subdirectory(cpp/packages/applications/eliza/packages/plugin-bootstrap)
add_subdirectory(cpp/packages/applications/eliza/packages/plugin-dummy-services)
add_subdirectory(cpp/packages/applications/eliza/packages/plugin-sql)
add_subdirectory(cpp/packages/applications/eliza/packages/plugin-starter)
add_subdirectory(cpp/packages/applications/eliza/packages/project-starter)
add_subdirectory(cpp/packages/applications/eliza/packages/project-tee-starter)
add_subdirectory(cpp/packages/applications/eliza/packages/server)

# ============================================================================
# AutoFun Sub-Packages
# ============================================================================
add_subdirectory(cpp/packages/integration/auto_fun/packages/autodoc)
add_subdirectory(cpp/packages/integration/auto_fun/packages/client)
add_subdirectory(cpp/packages/integration/auto_fun/packages/docs)
add_subdirectory(cpp/packages/integration/auto_fun/packages/program)
add_subdirectory(cpp/packages/integration/auto_fun/packages/raydium)
add_subdirectory(cpp/packages/integration/auto_fun/packages/server)
add_subdirectory(cpp/packages/integration/auto_fun/packages/types)

# ============================================================================
# Additional External Modules
# ============================================================================
# LiveVideoChat already included as cpp/external/livevideochat above
add_subdirectory(cpp/external/aum_tracker)
add_subdirectory(cpp/external/mobile)
add_subdirectory(cpp/external/sandbox_template_cloud)
add_subdirectory(cpp/external/test_hybrid)

# ============================================================================
# Testing
# ============================================================================
add_subdirectory(cpp/tests)

# ============================================================================
# Demo and Test Executables
# ============================================================================

# Spartan Integration Test
add_executable(spartan_integration_test
    cpp/tests/spartan_test.cpp
)
target_include_directories(spartan_integration_test PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
target_link_libraries(spartan_integration_test
    elizaos-core
    elizaos-spartan
    gtest
    gtest_main
    Threads::Threads
)

# Spartan Demo
add_executable(spartan_demo
    examples/spartan_demo.cpp
)
target_include_directories(spartan_demo PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
target_link_libraries(spartan_demo
    elizaos-core
    elizaos-agentlogger
    elizaos-spartan
    Threads::Threads
)

# Stage 4 Demo
add_executable(stage4_demo
    examples/demo_stage4.cpp
)
target_include_directories(stage4_demo PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
target_link_libraries(stage4_demo
    elizaos-core
    elizaos-agentlogger
    elizaos-ljspeechtools
    elizaos-livevideochat
    elizaos-evolutionary
    elizaos-embodiment
    Threads::Threads
)

# Stage 5 Demo
add_executable(stage5_demo
    examples/demo_stage5.cpp
)
target_include_directories(stage5_demo PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
target_link_libraries(stage5_demo
    elizaos-core
    elizaos-agentlogger
    elizaos-evolutionary
    elizaos-website
    elizaos-elizaos_github_io
    elizaos-vercel_api
    elizaos-embodiment
    Threads::Threads
)

# Stage 6 Demo
add_executable(stage6_demo
    examples/demo_stage6.cpp
)
target_include_directories(stage6_demo PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
target_link_libraries(stage6_demo
    elizaos-core
    elizaos-agentlogger
    elizaos-agentloop
    elizaos-agentmemory
    elizaos-agentaction
    elizaos-embodiment
    Threads::Threads
)

# Awesome Eliza Demo
add_executable(awesome_eliza_demo
    examples/awesome_eliza_demo.cpp
)
target_include_directories(awesome_eliza_demo PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
target_link_libraries(awesome_eliza_demo
    elizaos-core
    elizaos-awesome_eliza
    Threads::Threads
)

# TheOrg Demo
add_executable(the_org_demo
    examples/the_org_demo.cpp
)
target_include_directories(the_org_demo PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
target_link_libraries(the_org_demo
    elizaos-core
    elizaos-agentlogger
    elizaos-agentmemory
    elizaos-the_org
    Threads::Threads
)

# Main Eliza CLI executable
add_executable(eliza
    examples/eliza_main.cpp
)
target_include_directories(eliza PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
target_link_libraries(eliza
    elizaos-core
    elizaos-agentlogger
    elizaos-agentloop
    elizaos-agentmemory
    elizaos-eliza
    elizaos-knowledge
    elizaos-characters
    Threads::Threads
)

# EasyCompletion Demo
add_executable(easycompletion_demo
    examples/easycompletion_demo.cpp
)
target_include_directories(easycompletion_demo PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
target_link_libraries(easycompletion_demo
    elizaos-core
    elizaos-agentlogger
    elizaos-easycompletion
    Threads::Threads
)

# Characters Demo
add_executable(characters_demo
    examples/characters_demo.cpp
)
target_include_directories(characters_demo PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
target_link_libraries(characters_demo
    elizaos-core
    elizaos-agentlogger
    elizaos-agentmemory
    elizaos-characters
    nlohmann_json::nlohmann_json
    Threads::Threads
)

# Knowledge Demo
add_executable(knowledge_demo
    examples/knowledge_demo.cpp
)
target_include_directories(knowledge_demo PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
target_link_libraries(knowledge_demo
    elizaos-core
    elizaos-agentlogger
    elizaos-agentmemory
    elizaos-agentaction
    elizaos-knowledge
    Threads::Threads
)

# Eliza's List Demo
add_executable(elizas_list_demo
    examples/elizas_list_demo.cpp
)
target_include_directories(elizas_list_demo PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
target_link_libraries(elizas_list_demo
    elizaos-core
    elizaos-agentlogger
    elizaos-elizas_list
    nlohmann_json::nlohmann_json
    Threads::Threads
)

add_executable(knowledge_quick_demo
    examples/knowledge_quick_demo.cpp
)
target_include_directories(knowledge_quick_demo PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
target_link_libraries(knowledge_quick_demo
    elizaos-core
    elizaos-agentlogger
    elizaos-agentmemory
    elizaos-agentaction
    elizaos-knowledge
    Threads::Threads
)

# Eliza's List Real Data Test
add_executable(elizas_list_real_test
    examples/elizas_list_real_test.cpp
)
target_include_directories(elizas_list_real_test PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
target_link_libraries(elizas_list_real_test
    elizaos-core
    elizaos-agentlogger
    elizaos-elizas_list
    nlohmann_json::nlohmann_json
    Threads::Threads
)

# Eliza's List Unit Test
add_executable(elizas_list_unit_test
    examples/elizas_list_unit_test.cpp
)
target_include_directories(elizas_list_unit_test PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
target_link_libraries(elizas_list_unit_test
    elizaos-core
    elizaos-agentlogger
    elizaos-elizas_list
    nlohmann_json::nlohmann_json
    Threads::Threads
)

# Eliza 3D Hyperfy Starter Demo
add_executable(eliza_3d_hyperfy_starter_demo
    examples/eliza_3d_hyperfy_starter_demo.cpp
)
target_include_directories(eliza_3d_hyperfy_starter_demo PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
target_link_libraries(eliza_3d_hyperfy_starter_demo
    elizaos-core
    elizaos-agentlogger
    elizaos-eliza_3d_hyperfy_starter
    Threads::Threads
)

# Eliza's World Demo
add_executable(elizas_world_demo
    examples/elizas_world_demo.cpp
)
target_include_directories(elizas_world_demo PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
target_link_libraries(elizas_world_demo
    elizaos-core
    elizaos-agentlogger
    elizaos-elizas_world
    Threads::Threads
)

# Eliza's World Standalone Test
add_executable(elizas_world_standalone_test
    examples/elizas_world_standalone_test.cpp
)
target_include_directories(elizas_world_standalone_test PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
target_link_libraries(elizas_world_standalone_test
    elizaos-core
    elizaos-agentlogger
    elizaos-elizas_world
    gtest
    Threads::Threads
)

# Registry Demo
add_executable(registry_demo
    examples/registry_demo.cpp
)
target_include_directories(registry_demo PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
target_link_libraries(registry_demo
    elizaos-core
    elizaos-agentlogger
    elizaos-registry
    Threads::Threads
)

# Shell Demo
add_executable(shell_demo
    examples/shell_demo.cpp
)
target_include_directories(shell_demo PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
target_link_libraries(shell_demo
    elizaos-core
    elizaos-agentlogger
    elizaos-agentcomms
    elizaos-agentshell
    Threads::Threads
)

# AgentShell Integration Test
add_executable(agentshell_integration_test
    examples/agentshell_standalone_test.cpp
)
target_include_directories(agentshell_integration_test PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
target_link_libraries(agentshell_integration_test
    elizaos-core
    elizaos-agentlogger
    elizaos-agentcomms
    elizaos-agentshell
    Threads::Threads
)

# Ontogenesis Examples
add_executable(ontogenesis_simple_demo
    examples/ontogenesis_simple_demo.cpp
)
target_include_directories(ontogenesis_simple_demo PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
target_link_libraries(ontogenesis_simple_demo
    elizaos-ontogenesis
    Threads::Threads
)

add_executable(ontogenesis_evolution_demo
    examples/ontogenesis_evolution_demo.cpp
)
target_include_directories(ontogenesis_evolution_demo PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
target_link_libraries(ontogenesis_evolution_demo
    elizaos-ontogenesis
    Threads::Threads
)

add_executable(ontogenesis_lineage_demo
    examples/ontogenesis_lineage_demo.cpp
)
target_include_directories(ontogenesis_lineage_demo PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
target_link_libraries(ontogenesis_lineage_demo
    elizaos-ontogenesis
    Threads::Threads
)

# Add example executable (optional)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Dgen/KoboldCpp Provider Plugin (must be before install)
add_subdirectory(cpp/packages/plugins/dgen_kobold_provider)

# ============================================================================
# Installation
# ============================================================================
install(TARGETS
    # Main executables
    eliza shell_demo agentshell_integration_test
    spartan_integration_test spartan_demo stage4_demo stage5_demo stage6_demo
    awesome_eliza_demo the_org_demo easycompletion_demo
    characters_demo knowledge_demo knowledge_quick_demo
    elizas_list_demo elizas_list_real_test elizas_list_unit_test
    eliza_3d_hyperfy_starter_demo elizas_world_demo
    registry_demo
    
    # Core and Infrastructure libraries
    elizaos-core elizaos-agentloop elizaos-agentmemory
    elizaos-agentaction elizaos-agentagenda
    elizaos-agentcomms elizaos-agentlogger elizaos-agentshell
    elizaos-agentbrowser
    
    # Application modules
    elizaos-eliza elizaos-characters elizaos-knowledge
    elizaos-goal_manager
    
    # Integration modules
    elizaos-auto_fun elizaos-autonomous_starter
    elizaos-sweagent elizaos-mcp_gateway elizaos-otaku elizaos-otc_agent
    
    # Plugin modules
    elizaos-eliza_plugin_starter elizaos-plugin_specification
    elizaos-plugins_automation
    
    # Starter modules
    elizaos-eliza_starter elizaos-eliza_nextjs_starter
    elizaos-eliza_3d_hyperfy_starter
    
    # External modules
    elizaos-autofun_idl elizaos-awesome_eliza elizaos-brandkit
    elizaos-characterfile elizaos-classified elizaos-easycompletion
    elizaos-elizaos_github_io elizaos-elizas_list elizaos-elizas_world
    elizaos-hat elizaos-hats elizaos-registry elizaos-spartan
    elizaos-the_org elizaos-trust_scoreboard elizaos-workgroups
    
    # Multimedia
    elizaos-ljspeechtools elizaos-livevideochat
    
    # Web and Documentation
    elizaos-vercel_api elizaos-website
    
    # Tools and Automation
    elizaos-discord_summarizer elizaos-discrub_ext
    
    # Learning and Adaptation
    elizaos-evolutionary elizaos-ontogenesis
    
    # Embodiment
    elizaos-embodiment

    # Eliza Sub-Package libraries
    elizaos-eliza-api-client elizaos-eliza-app elizaos-eliza-autodoc
    elizaos-eliza-cli elizaos-eliza-client elizaos-eliza-core
    elizaos-eliza-docs elizaos-eliza-plugin-bootstrap
    elizaos-eliza-plugin-dummy-services elizaos-eliza-plugin-sql
    elizaos-eliza-plugin-starter elizaos-eliza-project-starter
    elizaos-eliza-project-tee-starter elizaos-eliza-server

    # AutoFun Sub-Package libraries
    elizaos-autofun-autodoc elizaos-autofun-client elizaos-autofun-docs
    elizaos-autofun-raydium elizaos-autofun-server

    # Generated Module libraries
    elizaos-generated_api elizaos-generated_auth elizaos-generated_cli
    elizaos-generated_database elizaos-generated_docs elizaos-generated_misc
    elizaos-generated_plugins elizaos-generated_services
    elizaos-generated_testing elizaos-generated_trade elizaos-generated_ui
    elizaos-generated_utils elizaos-generated_websocket

    # Additional External Module libraries
    elizaos-aum_tracker elizaos-sandbox_template_cloud elizaos-test_hybrid

    # Plugin Provider libraries
    elizaos-dgen_kobold_provider

    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# Generate package config
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/ElizaOSConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Print configuration summary
message(STATUS "ElizaOS-CPP Configuration Summary:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
if(CURL_FOUND)
    message(STATUS "  libcurl: Found")
else()
    message(STATUS "  libcurl: Not found (HTTP functionality will be limited)")
endif()

# ============================================================================
# CPack Configuration for Multi-Platform Packaging
# ============================================================================

# Install header files
install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
)

# Install documentation
install(FILES 
    README.md
    LICENSE
    DESTINATION share/doc/elizaos-cpp
    OPTIONAL
)

# CPack general settings
set(CPACK_PACKAGE_NAME "elizaos-cpp")
set(CPACK_PACKAGE_VENDOR "ElizaOS")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Next-Generation Cognitive Agent Framework")
set(CPACK_PACKAGE_DESCRIPTION "A high-performance C++ implementation of the ElizaOS agent framework for building sophisticated autonomous agents with advanced cognitive capabilities.")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/o9nn/elizaos-cpp")
set(CPACK_PACKAGE_CONTACT "ElizaOS Team <team@elizaos.ai>")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

# DEB package configuration
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "ElizaOS Team <team@elizaos.ai>")
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libcurl4, libstdc++6 (>= 9.0), libc6 (>= 2.31)")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/o9nn/elizaos-cpp")
set(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/packaging/debian/postinst;${CMAKE_CURRENT_SOURCE_DIR}/packaging/debian/prerm")

# RPM package configuration
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_GROUP "Development/Libraries")
set(CPACK_RPM_PACKAGE_URL "https://github.com/o9nn/elizaos-cpp")
set(CPACK_RPM_PACKAGE_REQUIRES "libcurl >= 7.0, glibc >= 2.31")
set(CPACK_RPM_FILE_NAME "RPM-DEFAULT")
set(CPACK_RPM_PACKAGE_RELEASE "1")
set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")

# NSIS (Windows installer) configuration
set(CPACK_NSIS_PACKAGE_NAME "ElizaOS C++")
set(CPACK_NSIS_DISPLAY_NAME "ElizaOS C++ Framework")
set(CPACK_NSIS_HELP_LINK "https://github.com/o9nn/elizaos-cpp")
set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/o9nn/elizaos-cpp")
set(CPACK_NSIS_CONTACT "team@elizaos.ai")
set(CPACK_NSIS_MODIFY_PATH ON)
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)

# Archive configuration (TGZ, ZIP)
set(CPACK_ARCHIVE_COMPONENT_INSTALL OFF)

# Component configuration
set(CPACK_COMPONENTS_ALL libraries headers executables documentation)
set(CPACK_COMPONENT_LIBRARIES_DISPLAY_NAME "Libraries")
set(CPACK_COMPONENT_HEADERS_DISPLAY_NAME "C++ Headers")
set(CPACK_COMPONENT_EXECUTABLES_DISPLAY_NAME "Executables")
set(CPACK_COMPONENT_DOCUMENTATION_DISPLAY_NAME "Documentation")

# Generator selection based on platform
if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
elseif(APPLE)
    set(CPACK_GENERATOR "TGZ;productbuild")
else()
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
endif()

# Source package configuration
set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")
list(APPEND CPACK_SOURCE_IGNORE_FILES
    "/.git/"
    "/.github/"
    "/build/"
    "/.vscode/"
    "/.idea/"
    "\\\\.swp$"
    "\\\\.swo$"
    "~$"
)

include(CPack)

message(STATUS "")
message(STATUS "Packaging configuration:")
message(STATUS "  Package name: ${CPACK_PACKAGE_NAME}")
message(STATUS "  Package version: ${CPACK_PACKAGE_VERSION}")
message(STATUS "  Generators: ${CPACK_GENERATOR}")
message(STATUS "")
message(STATUS "To create packages, run:")
message(STATUS "  cd build && cpack")
message(STATUS "")

# Dgen/KoboldCpp Provider Plugin - installed via main install() above
