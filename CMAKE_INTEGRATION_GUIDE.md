# CMake Integration Guide for Real Implementations

**Author:** Manus AI  
**Date:** December 13, 2024  
**Version:** 1.0

---

## 1. Introduction

This guide explains how to use the integrated CMakeLists.txt files for AgentBrowser and Discord Summarizer. These files automatically detect dependencies and enable real implementations when available, gracefully falling back to mock implementations otherwise.

---

## 2. Quick Start

### 2.1. Install Dependencies

**AgentBrowser:**
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y libcurl4-openssl-dev libgumbo-dev

# macOS
brew install curl gumbo-parser

# Verify installation
pkg-config --modversion libcurl
pkg-config --modversion gumbo
```

**Discord Summarizer:**
```bash
# Follow the official D++ installation guide
# https://dpp.dev/build.html

# Quick install on Ubuntu/Debian
cd /tmp
git clone https://github.com/brainboxdotcc/DPP.git
cd DPP
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
```

### 2.2. Replace CMakeLists.txt Files

```bash
cd /path/to/elizaos-cpp

# Backup original files
cp cpp/agentbrowser/CMakeLists.txt cpp/agentbrowser/CMakeLists.txt.bak
cp cpp/discord_summarizer/CMakeLists.txt cpp/discord_summarizer/CMakeLists.txt.bak

# Install integrated versions
cp cpp/agentbrowser/CMakeLists_INTEGRATED.txt cpp/agentbrowser/CMakeLists.txt
cp cpp/discord_summarizer/CMakeLists_INTEGRATED.txt cpp/discord_summarizer/CMakeLists.txt
```

### 2.3. Build

```bash
cd build
cmake ..
make -j$(nproc)
```

---

## 3. CMake Configuration Details

### 3.1. AgentBrowser CMakeLists.txt

**Key Features:**

| Feature | Description |
|---------|-------------|
| **Dependency Detection** | Automatically finds libcurl and gumbo-parser |
| **Fallback Mechanism** | Uses pkg-config, then manual search |
| **Conditional Compilation** | Sets `AGENTBROWSER_REAL_IMPLEMENTATION` flag |
| **Source Selection** | Adds `http_client.cpp` and `html_parser.cpp` if real impl enabled |
| **Configuration Summary** | Displays detailed status during CMake configuration |

**Preprocessor Flag:**
```cmake
if(AGENTBROWSER_REAL_IMPL)
    target_compile_definitions(elizaos-agentbrowser PRIVATE
        AGENTBROWSER_REAL_IMPLEMENTATION
    )
endif()
```

**Expected Output (Real Implementation):**
```
-- AgentBrowser: Found libcurl 7.81.0
-- AgentBrowser: Found gumbo-parser via pkg-config
-- AgentBrowser: ✅ Real implementation ENABLED (libcurl + gumbo-parser)

═══════════════════════════════════════════════════════════
AgentBrowser Configuration Summary
═══════════════════════════════════════════════════════════
  Implementation:    TRUE
  libcurl:           7.81.0
  gumbo-parser:      Found
  HTTP Requests:     ✅ Real (libcurl)
  HTML Parsing:      ✅ Real (gumbo-parser)
  Element Finding:   ✅ Real (DOM traversal)
═══════════════════════════════════════════════════════════
```

**Expected Output (Mock Implementation):**
```
-- AgentBrowser: libcurl not found - HTTP functionality will use mock implementation
-- AgentBrowser: gumbo-parser not found - HTML parsing will use mock implementation
-- AgentBrowser: ⚠️  Mock implementation ENABLED (missing dependencies)
  - Install libcurl: sudo apt-get install libcurl4-openssl-dev
  - Install gumbo-parser: sudo apt-get install libgumbo-dev

═══════════════════════════════════════════════════════════
AgentBrowser Configuration Summary
═══════════════════════════════════════════════════════════
  Implementation:    FALSE
  HTTP Requests:     ⚠️  Mock
  HTML Parsing:      ⚠️  Mock
  Element Finding:   ⚠️  Mock
═══════════════════════════════════════════════════════════
```

### 3.2. Discord Summarizer CMakeLists.txt

**Key Features:**

| Feature | Description |
|---------|-------------|
| **Dependency Detection** | Automatically finds D++ library |
| **Multiple Search Methods** | Uses find_package, pkg-config, and manual search |
| **Conditional Compilation** | Sets `DISCORD_REAL_IMPLEMENTATION` flag |
| **Source Selection** | Adds `discord_client_real.cpp` if real impl enabled |
| **System Library Linking** | Automatically links `dl` and `pthread` on Linux |
| **Warning Suppression** | Suppresses D++ deprecation warnings |

**Preprocessor Flag:**
```cmake
if(DISCORD_REAL_IMPL)
    target_compile_definitions(elizaos-discord_summarizer PRIVATE
        DISCORD_REAL_IMPLEMENTATION
    )
endif()
```

**Expected Output (Real Implementation):**
```
-- Discord Summarizer: Found D++ library
-- Discord Summarizer: ✅ Real implementation ENABLED (D++ library)

═══════════════════════════════════════════════════════════
Discord Summarizer Configuration Summary
═══════════════════════════════════════════════════════════
  Implementation:    TRUE
  D++ Library:       Found
  Discord API:       ✅ Real (WebSocket + REST)
  Message Retrieval: ✅ Real (Discord API)
  Message Sending:   ✅ Real (Discord API)
  Event Handling:    ✅ Real (WebSocket)
═══════════════════════════════════════════════════════════
```

**Expected Output (Mock Implementation):**
```
-- Discord Summarizer: D++ not found - Discord API will use mock implementation
-- Discord Summarizer: ⚠️  Mock implementation ENABLED (missing D++)
  - Install D++: https://dpp.dev/build.html

═══════════════════════════════════════════════════════════
Discord Summarizer Configuration Summary
═══════════════════════════════════════════════════════════
  Implementation:    FALSE
  Discord API:       ⚠️  Mock
  Message Retrieval: ⚠️  Mock (hardcoded)
  Message Sending:   ⚠️  Mock (simulated)
  Event Handling:    ⚠️  Mock (none)
═══════════════════════════════════════════════════════════
```

---

## 4. FindGumbo.cmake Module

A custom CMake module is provided for better gumbo-parser detection.

**Location:** `cmake/FindGumbo.cmake`

**Usage:**
```cmake
# In main CMakeLists.txt, add:
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Then in agentbrowser/CMakeLists.txt:
find_package(Gumbo REQUIRED)

if(Gumbo_FOUND)
    target_link_libraries(elizaos-agentbrowser PRIVATE Gumbo::Gumbo)
endif()
```

**Advantages:**
- Creates an imported target `Gumbo::Gumbo`
- Searches multiple standard locations
- Provides version information
- Follows CMake best practices

---

## 5. Troubleshooting

### 5.1. libcurl Not Found

**Error:**
```
-- AgentBrowser: libcurl not found
```

**Solution:**
```bash
# Ubuntu/Debian
sudo apt-get install libcurl4-openssl-dev

# macOS
brew install curl

# Verify
pkg-config --modversion libcurl
```

### 5.2. gumbo-parser Not Found

**Error:**
```
-- AgentBrowser: gumbo-parser not found
```

**Solution:**
```bash
# Ubuntu/Debian
sudo apt-get install libgumbo-dev

# macOS
brew install gumbo-parser

# Verify
pkg-config --modversion gumbo
```

### 5.3. D++ Not Found

**Error:**
```
-- Discord Summarizer: D++ not found
```

**Solution:**
```bash
# Build from source (recommended)
cd /tmp
git clone https://github.com/brainboxdotcc/DPP.git
cd DPP
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
sudo ldconfig

# Verify
pkg-config --modversion dpp
```

### 5.4. CMake Cache Issues

If CMake doesn't detect newly installed dependencies:

```bash
# Clear CMake cache
cd build
rm -rf CMakeCache.txt CMakeFiles/

# Reconfigure
cmake ..
```

---

## 6. Testing

### 6.1. Verify Real Implementation is Active

**Check CMake Output:**
Look for the configuration summary during `cmake ..`:
- ✅ symbols indicate real implementation
- ⚠️ symbols indicate mock implementation

**Check Preprocessor Definitions:**
```bash
# In build directory
grep -r "AGENTBROWSER_REAL_IMPLEMENTATION" CMakeFiles/
grep -r "DISCORD_REAL_IMPLEMENTATION" CMakeFiles/
```

### 6.2. Runtime Verification

**AgentBrowser:**
```cpp
#include "elizaos/agentbrowser.hpp"

int main() {
    elizaos::AgentBrowser browser;
    browser.initialize();
    
    // Navigate to a real website
    auto result = browser.navigateTo("https://example.com");
    
    // Get real page title
    auto title = browser.getPageTitle();
    
    std::cout << "Title: " << title.value_or("No title") << std::endl;
    // Expected: "Example Domain" (real) vs "Sample Page - example.com" (mock)
    
    return 0;
}
```

**Discord Summarizer:**
```cpp
#include "elizaos/discord_summarizer.hpp"

int main() {
    elizaos::DiscordSummarizer summarizer;
    
    // Connect with real bot token
    bool connected = summarizer.connect("YOUR_BOT_TOKEN");
    
    if (connected) {
        // Get real messages from Discord
        auto messages = summarizer.getMessages("CHANNEL_ID", 10);
        
        std::cout << "Retrieved " << messages.size() << " messages" << std::endl;
        // Real: Actual message count from Discord
        // Mock: Always 10 hardcoded messages
    }
    
    return 0;
}
```

---

## 7. Advanced Configuration

### 7.1. Force Mock Implementation

To force the use of mock implementations even when dependencies are available:

```bash
cmake .. -DFORCE_MOCK_IMPLEMENTATIONS=ON
```

Add to CMakeLists.txt:
```cmake
option(FORCE_MOCK_IMPLEMENTATIONS "Force use of mock implementations" OFF)

if(FORCE_MOCK_IMPLEMENTATIONS)
    set(AGENTBROWSER_REAL_IMPL FALSE)
    set(DISCORD_REAL_IMPL FALSE)
endif()
```

### 7.2. Specify Dependency Locations

If dependencies are installed in non-standard locations:

```bash
cmake .. \
  -DCURL_ROOT=/path/to/curl \
  -DGumbo_ROOT_DIR=/path/to/gumbo \
  -Ddpp_DIR=/path/to/dpp/lib/cmake/dpp
```

---

## 8. Conclusion

The integrated CMakeLists.txt files provide a robust, flexible build system that automatically adapts to the available dependencies. This ensures the ElizaOS C++ repository can be built and used in any environment, with real implementations enabled when possible and graceful fallback to mock implementations when necessary.
