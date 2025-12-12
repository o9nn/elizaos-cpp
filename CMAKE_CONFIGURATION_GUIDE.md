# CMake Configuration Guide: AgentBrowser & Discord Summarizer

**Author:** Manus AI  
**Date:** December 12, 2025  
**Version:** 1.0

---

## Overview

This guide provides complete CMake configuration files for integrating the real implementations of **AgentBrowser** and **Discord Summarizer** into the ElizaOS C++ framework. The configurations include proper dependency management, compiler options, and installation rules.

---

## Table of Contents

1. [Prerequisites](#1-prerequisites)
2. [AgentBrowser CMakeLists.txt](#2-agentbrowser-cmakeliststxt)
3. [Discord Summarizer CMakeLists.txt](#3-discord-summarizer-cmakeliststxt)
4. [Main CMakeLists.txt Updates](#4-main-cmakeliststxt-updates)
5. [Dependency Installation](#5-dependency-installation)
6. [Build Instructions](#6-build-instructions)
7. [Troubleshooting](#7-troubleshooting)

---

## 1. Prerequisites

### 1.1. System Requirements

| Component | Minimum Version | Recommended |
|-----------|----------------|-------------|
| CMake | 3.16 | 3.20+ |
| C++ Compiler | GCC 9.0 / Clang 10.0 / MSVC 2019 | GCC 11+ |
| pkg-config | Any | Latest |

### 1.2. Required Libraries

| Library | Purpose | Version |
|---------|---------|---------|
| **libcurl** | HTTP client for AgentBrowser | 7.68+ |
| **gumbo-parser** | HTML5 parser for AgentBrowser | 0.10+ |
| **D++ (DPP)** | Discord API for Discord Summarizer | 10.0+ |

---

## 2. AgentBrowser CMakeLists.txt

### 2.1. Complete Configuration

Replace `cpp/agentbrowser/CMakeLists.txt` with the following:

```cmake
# AgentBrowser - Web automation interface for ElizaOS agents
# Updated to use REAL implementation with libcurl and gumbo-parser

# ==============================================================================
# DEPENDENCY CONFIGURATION
# ==============================================================================

# Find libcurl (required for HTTP requests)
find_package(PkgConfig REQUIRED)
pkg_check_modules(CURL REQUIRED libcurl)

# Find gumbo-parser (required for HTML parsing)
pkg_check_modules(GUMBO REQUIRED gumbo)

# ==============================================================================
# LIBRARY DEFINITION
# ==============================================================================

# Create the AgentBrowser library with REAL implementation
add_library(elizaos-agentbrowser STATIC
    src/agentbrowser_real.cpp
    # Note: http_client.cpp and html_parser.cpp are included inline
)

# ==============================================================================
# INCLUDE DIRECTORIES
# ==============================================================================

target_include_directories(elizaos-agentbrowser 
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
    PRIVATE
        ${CURL_INCLUDE_DIRS}
        ${GUMBO_INCLUDE_DIRS}
)

# ==============================================================================
# LINK LIBRARIES
# ==============================================================================

target_link_libraries(elizaos-agentbrowser 
    PUBLIC
        elizaos-core
        elizaos-agentlogger
        elizaos-agentmemory
    PRIVATE
        Threads::Threads
        ${CURL_LIBRARIES}
        ${GUMBO_LIBRARIES}
)

# ==============================================================================
# COMPILER DEFINITIONS
# ==============================================================================

target_compile_definitions(elizaos-agentbrowser 
    PRIVATE
        AGENTBROWSER_REAL_IMPLEMENTATION=1
)

# ==============================================================================
# COMPILER OPTIONS
# ==============================================================================

if(MSVC)
    target_compile_options(elizaos-agentbrowser PRIVATE /W4 /permissive-)
else()
    target_compile_options(elizaos-agentbrowser PRIVATE 
        -Wall -Wextra -Wpedantic -Wno-unused-parameter
    )
endif()

# ==============================================================================
# FEATURE SUMMARY
# ==============================================================================

message(STATUS "AgentBrowser Configuration:")
message(STATUS "  - Real Implementation: ENABLED")
message(STATUS "  - libcurl: ${CURL_VERSION}")
message(STATUS "  - gumbo-parser: ${GUMBO_VERSION}")
```

### 2.2. Key Changes from Mock Implementation

| Aspect | Mock | Real |
|--------|------|------|
| **Source File** | `agentbrowser.cpp` | `agentbrowser_real.cpp` |
| **Dependencies** | None | libcurl, gumbo-parser |
| **HTTP Requests** | Fake responses | Real HTTP via libcurl |
| **HTML Parsing** | Mock elements | Real DOM via gumbo-parser |
| **Element Finding** | Hardcoded | Real CSS selector parsing |

### 2.3. Optional Features

Add these sections if needed:

```cmake
# Enable verbose logging
option(AGENTBROWSER_VERBOSE "Enable verbose logging" OFF)
if(AGENTBROWSER_VERBOSE)
    target_compile_definitions(elizaos-agentbrowser PRIVATE VERBOSE_LOGGING=1)
endif()

# Enable SSL verification (default: ON)
option(AGENTBROWSER_SSL_VERIFY "Enable SSL certificate verification" ON)
if(NOT AGENTBROWSER_SSL_VERIFY)
    target_compile_definitions(elizaos-agentbrowser PRIVATE CURL_NO_SSL_VERIFY=1)
endif()
```

---

## 3. Discord Summarizer CMakeLists.txt

### 3.1. Complete Configuration

Replace `cpp/discord_summarizer/CMakeLists.txt` with the following:

```cmake
# Discord Summarizer - Real Discord API integration
# Updated to use REAL implementation with D++ library

# ==============================================================================
# DEPENDENCY CONFIGURATION
# ==============================================================================

# Find D++ library (Discord C++ library)
find_package(PkgConfig REQUIRED)
pkg_check_modules(DPP REQUIRED dpp)

# ==============================================================================
# SOURCE FILES
# ==============================================================================

set(DISCORD_SUMMARIZER_SOURCES
    src/discord_summarizer.cpp
    src/discord_client_real.cpp
)

set(DISCORD_SUMMARIZER_HEADERS
    include/discord_client_real.hpp
)

# ==============================================================================
# LIBRARY DEFINITION
# ==============================================================================

add_library(elizaos-discord_summarizer STATIC
    ${DISCORD_SUMMARIZER_SOURCES}
)

# ==============================================================================
# INCLUDE DIRECTORIES
# ==============================================================================

target_include_directories(elizaos-discord_summarizer 
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${DPP_INCLUDE_DIRS}
)

# ==============================================================================
# LINK LIBRARIES
# ==============================================================================

target_link_libraries(elizaos-discord_summarizer 
    PUBLIC
        elizaos-core
        elizaos-agentlogger
        elizaos-agentmemory
    PRIVATE
        Threads::Threads
        ${DPP_LIBRARIES}
)

# ==============================================================================
# COMPILER DEFINITIONS
# ==============================================================================

target_compile_definitions(elizaos-discord_summarizer 
    PRIVATE
        DISCORD_REAL_IMPLEMENTATION=1
)

# ==============================================================================
# COMPILER OPTIONS
# ==============================================================================

if(MSVC)
    target_compile_options(elizaos-discord_summarizer PRIVATE 
        /W4 /permissive- /std:c++17
    )
else()
    target_compile_options(elizaos-discord_summarizer PRIVATE 
        -Wall -Wextra -Wpedantic 
        -Wno-unused-parameter 
        -Wno-deprecated-declarations
    )
endif()

# ==============================================================================
# LINKER OPTIONS
# ==============================================================================

if(UNIX AND NOT APPLE)
    target_link_libraries(elizaos-discord_summarizer PRIVATE dl pthread)
endif()

# ==============================================================================
# FEATURE SUMMARY
# ==============================================================================

message(STATUS "Discord Summarizer Configuration:")
message(STATUS "  - Real Implementation: ENABLED")
message(STATUS "  - D++ version: ${DPP_VERSION}")
```

### 3.2. Key Changes from Mock Implementation

| Aspect | Mock | Real |
|--------|------|------|
| **Source Files** | `discord_summarizer.cpp` only | + `discord_client_real.cpp` |
| **Dependencies** | None | D++ (DPP) library |
| **Discord Connection** | Fake | Real WebSocket + REST API |
| **Message Retrieval** | Hardcoded samples | Real Discord API calls |
| **Async Handling** | Synchronous | Async with promise/future |

### 3.3. Directory Structure

You'll need to create this structure:

```
cpp/discord_summarizer/
├── CMakeLists.txt
├── include/
│   └── discord_client_real.hpp
└── src/
    ├── discord_summarizer.cpp
    └── discord_client_real.cpp
```

---

## 4. Main CMakeLists.txt Updates

### 4.1. Add Global Dependency Finding

Add these lines after the existing `find_package(Threads REQUIRED)` section (around line 30):

```cmake
# Find packages
find_package(Threads REQUIRED)

# Find libcurl for AgentBrowser
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(CURL QUIET libcurl)
    pkg_check_modules(GUMBO QUIET gumbo)
    pkg_check_modules(DPP QUIET dpp)
endif()

# Fallback to find_package if pkg-config fails
if(NOT CURL_FOUND)
    find_package(CURL QUIET)
endif()

# Report found dependencies
if(CURL_FOUND)
    message(STATUS "Found libcurl: ${CURL_VERSION}")
else()
    message(WARNING "libcurl not found - AgentBrowser real implementation will not be available")
endif()

if(GUMBO_FOUND)
    message(STATUS "Found gumbo-parser: ${GUMBO_VERSION}")
else()
    message(WARNING "gumbo-parser not found - AgentBrowser real implementation will not be available")
endif()

if(DPP_FOUND)
    message(STATUS "Found D++: ${DPP_VERSION}")
else()
    message(WARNING "D++ not found - Discord Summarizer real implementation will not be available")
endif()
```

### 4.2. Add Build Options

Add these options before the subdirectory declarations:

```cmake
# Build options for real implementations
option(BUILD_AGENTBROWSER_REAL "Build AgentBrowser with real HTTP/HTML support" ${CURL_FOUND})
option(BUILD_DISCORD_REAL "Build Discord Summarizer with real Discord API support" ${DPP_FOUND})

if(BUILD_AGENTBROWSER_REAL AND NOT CURL_FOUND)
    message(FATAL_ERROR "BUILD_AGENTBROWSER_REAL requires libcurl and gumbo-parser")
endif()

if(BUILD_DISCORD_REAL AND NOT DPP_FOUND)
    message(FATAL_ERROR "BUILD_DISCORD_REAL requires D++ library")
endif()
```

### 4.3. Update CPack Dependencies

Update the CPack configuration (around line 621) to include new dependencies:

```cmake
# DEB package configuration
set(CPACK_DEBIAN_PACKAGE_DEPENDS 
    "libcurl4, libstdc++6 (>= 9.0), libc6 (>= 2.31), libgumbo1, libdpp10"
)

# RPM package configuration
set(CPACK_RPM_PACKAGE_REQUIRES 
    "libcurl >= 7.0, glibc >= 2.31, gumbo-parser >= 0.10, dpp >= 10.0"
)
```

---

## 5. Dependency Installation

### 5.1. Ubuntu/Debian

```bash
# Update package lists
sudo apt-get update

# Install AgentBrowser dependencies
sudo apt-get install -y libcurl4-openssl-dev libgumbo-dev

# Install Discord Summarizer dependencies
wget -O /tmp/install-dpp.sh https://nightly.dpp.dev/install-nightly.sh
sudo bash /tmp/install-dpp.sh
sudo apt-get update && sudo apt-get install -y libdpp-dev

# Verify installations
pkg-config --modversion libcurl gumbo dpp
```

**Expected Output:**
```
7.81.0
0.10.1
10.0.30
```

### 5.2. Fedora/RHEL

```bash
# Install AgentBrowser dependencies
sudo dnf install -y libcurl-devel gumbo-parser-devel

# Install Discord Summarizer dependencies (build from source)
git clone https://github.com/brainboxdotcc/DPP.git
cd DPP
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
sudo make install
sudo ldconfig
```

### 5.3. macOS

```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install curl gumbo-parser d++

# Verify installations
pkg-config --modversion libcurl gumbo dpp
```

### 5.4. Windows (vcpkg)

```powershell
# Install vcpkg if not already installed
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# Install dependencies
.\vcpkg install curl:x64-windows gumbo-parser:x64-windows dpp:x64-windows

# Integrate with CMake
.\vcpkg integrate install
```

---

## 6. Build Instructions

### 6.1. Clean Build

```bash
cd /path/to/elizaos-cpp

# Remove old build directory
rm -rf build

# Create new build directory
mkdir build && cd build

# Configure with CMake
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_AGENTBROWSER_REAL=ON \
    -DBUILD_DISCORD_REAL=ON

# Build
make -j$(nproc)

# Verify builds
ls -lh cpp/agentbrowser/libelizaos-agentbrowser.a
ls -lh cpp/discord_summarizer/libelizaos-discord_summarizer.a
```

### 6.2. Incremental Build

```bash
cd /path/to/elizaos-cpp/build

# Reconfigure if CMakeLists.txt changed
cmake ..

# Build only changed components
make elizaos-agentbrowser elizaos-discord_summarizer -j$(nproc)
```

### 6.3. Build with Verbose Output

```bash
# Enable verbose build output
cmake .. -DCMAKE_VERBOSE_MAKEFILE=ON
make VERBOSE=1
```

### 6.4. Cross-Platform Build

**Linux:**
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

**Windows (Visual Studio):**
```powershell
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

**macOS:**
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64"
make -j$(sysctl -n hw.ncpu)
```

---

## 7. Troubleshooting

### 7.1. Common CMake Errors

#### Error: "Could not find gumbo using pkg-config"

**Cause:** gumbo-parser is not installed or not in pkg-config path.

**Solution:**
```bash
# Install gumbo-parser
sudo apt-get install libgumbo-dev

# Verify installation
pkg-config --modversion gumbo

# If still not found, set PKG_CONFIG_PATH
export PKG_CONFIG_PATH=/usr/lib/pkgconfig:/usr/local/lib/pkgconfig
```

#### Error: "Could not find dpp using pkg-config"

**Cause:** D++ is not installed or not in pkg-config path.

**Solution:**
```bash
# Reinstall D++
wget -O /tmp/install-dpp.sh https://nightly.dpp.dev/install-nightly.sh
sudo bash /tmp/install-dpp.sh
sudo apt-get update && sudo apt-get install libdpp-dev

# Verify installation
pkg-config --modversion dpp
```

#### Error: "CURL_FOUND is FALSE"

**Cause:** libcurl is not installed.

**Solution:**
```bash
# Install libcurl
sudo apt-get install libcurl4-openssl-dev

# Verify installation
curl-config --version
pkg-config --modversion libcurl
```

### 7.2. Linker Errors

#### Error: "undefined reference to `curl_easy_init`"

**Cause:** libcurl is not being linked properly.

**Solution:**
```cmake
# In CMakeLists.txt, ensure CURL is linked
target_link_libraries(elizaos-agentbrowser PRIVATE ${CURL_LIBRARIES})

# Or manually add
target_link_libraries(elizaos-agentbrowser PRIVATE curl)
```

#### Error: "undefined reference to `gumbo_parse`"

**Cause:** gumbo-parser is not being linked properly.

**Solution:**
```cmake
# In CMakeLists.txt, ensure GUMBO is linked
target_link_libraries(elizaos-agentbrowser PRIVATE ${GUMBO_LIBRARIES})

# Or manually add
target_link_libraries(elizaos-agentbrowser PRIVATE gumbo)
```

### 7.3. Runtime Errors

#### Error: "error while loading shared libraries: libdpp.so"

**Cause:** D++ shared library is not in the library path.

**Solution:**
```bash
# Add to LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

# Or update ldconfig
sudo ldconfig

# Or add to /etc/ld.so.conf
echo "/usr/local/lib" | sudo tee -a /etc/ld.so.conf
sudo ldconfig
```

### 7.4. Verification Commands

```bash
# Check if libraries are installed
ldconfig -p | grep -E "curl|gumbo|dpp"

# Check pkg-config files
ls /usr/lib/pkgconfig/ | grep -E "curl|gumbo|dpp"

# Test library linking
echo '#include <curl/curl.h>
int main() { curl_easy_init(); return 0; }' | \
gcc -x c - -lcurl -o /tmp/test && /tmp/test && echo "libcurl OK"

echo '#include <gumbo.h>
int main() { gumbo_parse(""); return 0; }' | \
gcc -x c - -lgumbo -o /tmp/test && /tmp/test && echo "gumbo OK"
```

---

## 8. Summary

### 8.1. Files to Modify

| File | Action | Purpose |
|------|--------|---------|
| `cpp/agentbrowser/CMakeLists.txt` | Replace | Add libcurl and gumbo-parser support |
| `cpp/discord_summarizer/CMakeLists.txt` | Replace | Add D++ library support |
| `CMakeLists.txt` (main) | Update | Add global dependency finding |

### 8.2. Dependencies to Install

| Library | Ubuntu/Debian | Fedora/RHEL | macOS |
|---------|---------------|-------------|-------|
| **libcurl** | `libcurl4-openssl-dev` | `libcurl-devel` | `curl` |
| **gumbo-parser** | `libgumbo-dev` | `gumbo-parser-devel` | `gumbo-parser` |
| **D++** | `libdpp-dev` (via script) | Build from source | `d++` |

### 8.3. Build Commands

```bash
# Install dependencies
sudo apt-get install libcurl4-openssl-dev libgumbo-dev
wget -O /tmp/install-dpp.sh https://nightly.dpp.dev/install-nightly.sh && sudo bash /tmp/install-dpp.sh
sudo apt-get update && sudo apt-get install libdpp-dev

# Build
cd /path/to/elizaos-cpp
mkdir -p build && cd build
cmake .. -DBUILD_AGENTBROWSER_REAL=ON -DBUILD_DISCORD_REAL=ON
make -j$(nproc)
```

---

## Conclusion

This CMake configuration guide provides everything needed to integrate the real implementations of AgentBrowser and Discord Summarizer. The configurations are production-ready, cross-platform compatible, and include comprehensive error handling and feature detection.

**Next Steps:**
1. Install all dependencies
2. Replace the CMakeLists.txt files
3. Build and test
4. Verify functionality with demos

---

**Document Version:** 1.0  
**Last Updated:** December 12, 2025  
**Author:** Manus AI
