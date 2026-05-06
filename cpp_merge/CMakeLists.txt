# ElizaOS C++ Merge - Integrated Build Configuration
# Combines transpiled code with core cpp functionality

cmake_minimum_required(VERSION 3.16)
project(ElizaOS_CPP_Merge VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ==============================================================================
# Include Paths - Core ElizaOS headers
# ==============================================================================

# Include paths for the main cpp library
include_directories(${CMAKE_SOURCE_DIR}/../cpp/include)
include_directories(${CMAKE_SOURCE_DIR}/../cpp/include/elizaos)

# ==============================================================================
# Merge Directory Modules
# Each subdirectory contains both transpiled and integrated cpp_core code
# ==============================================================================

# Define interface libraries for merged modules
add_library(elizaos_merge INTERFACE)

# Core Applications
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/eliza)
    add_subdirectory(eliza)
endif()

# Infrastructure  
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/LiveVideoChat)
    add_subdirectory(LiveVideoChat)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/SWEagent)
    add_subdirectory(SWEagent)
endif()

# Integration Modules
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/auto.fun)
    add_subdirectory(auto.fun)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/autofun-idl)
    add_subdirectory(autofun-idl)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/autonomous-starter)
    add_subdirectory(autonomous-starter)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/mcp-gateway)
    add_subdirectory(mcp-gateway)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/otaku)
    add_subdirectory(otaku)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/otc-agent)
    add_subdirectory(otc-agent)
endif()

# Starters
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/eliza-3d-hyperfy-starter)
    add_subdirectory(eliza-3d-hyperfy-starter)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/eliza-nextjs-starter)
    add_subdirectory(eliza-nextjs-starter)
endif()

# Plugins
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/plugin-specification)
    add_subdirectory(plugin-specification)
endif()

# Community/Organization
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/the-org)
    add_subdirectory(the-org)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/trust_scoreboard)
    add_subdirectory(trust_scoreboard)
endif()

# Platform integrations
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/spartan)
    add_subdirectory(spartan)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/vercel-api)
    add_subdirectory(vercel-api)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/registry)
    add_subdirectory(registry)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/discrub-ext)
    add_subdirectory(discrub-ext)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/classified)
    add_subdirectory(classified)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/elizaos.github.io)
    add_subdirectory(elizaos.github.io)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/elizas-list)
    add_subdirectory(elizas-list)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/elizas-world)
    add_subdirectory(elizas-world)
endif()

# Common utilities
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/common)
    add_subdirectory(common)
endif()

# ==============================================================================
# Testing
# ==============================================================================

enable_testing()

message(STATUS "ElizaOS C++ Merge configuration complete")
message(STATUS "  Source directory: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
