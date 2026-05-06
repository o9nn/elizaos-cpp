# ElizaOS C++ Integration Plan: AgentBrowser & Discord Summarizer

**Author:** Manus AI  
**Date:** December 12, 2025  
**Status:** ✅ Ready for Implementation

---

## 1. Introduction

This document provides a detailed integration plan for replacing the mock implementations of the **AgentBrowser** and **Discord Summarizer** components with fully functional, real-world implementations. The plan outlines the necessary dependencies, code modifications, and testing procedures required to achieve this.

---

## 2. AgentBrowser Integration Plan

### 2.1. Goal

The primary goal is to replace the mock `AgentBrowser` functionality with the existing real implementation that uses **libcurl** for HTTP requests and **gumbo-parser** for HTML parsing. This will enable the agent to perform real web browsing, data extraction, and analysis.

### 2.2. Current Status

- **Mock Implementation:** `cpp/agentbrowser/src/agentbrowser.cpp` currently contains mock functions for `findElement()`, `findElements()`, and `takeScreenshot()`.
- **Real Implementation:** A near-complete real implementation already exists in `cpp/agentbrowser/src/agentbrowser_real.cpp`, `http_client.cpp`, and `html_parser.cpp`.
- **Dependencies:** The real implementation requires `libcurl` and `gumbo-parser`, which are not yet installed in the build environment.

### 2.3. Action Plan

#### Step 1: Install Dependencies

The first step is to install the required libraries in the development environment.

```bash
sudo apt-get update && sudo apt-get install -y libcurl4-openssl-dev libgumbo-dev
```

#### Step 2: Update CMakeLists.txt

Modify `cpp/agentbrowser/CMakeLists.txt` to use the real implementation files and correctly link the dependencies.

**Current `CMakeLists.txt`:**
```cmake
add_library(elizaos-agentbrowser STATIC
    src/agentbrowser.cpp
)
# ... (linking)
```

**Proposed `CMakeLists.txt`:**
```cmake
# Find required packages
find_package(CURL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GUMBO REQUIRED gumbo)

# Add the library with real implementation files
add_library(elizaos-agentbrowser STATIC
    src/agentbrowser_real.cpp
)

# Link dependencies
target_link_libraries(elizaos-agentbrowser PRIVATE
    elizaos-core
    elizaos-agentlogger
    elizaos-agentmemory
    Threads::Threads
    ${CURL_LIBRARIES}
    ${GUMBO_LIBRARIES}
)

# Include directories
target_include_directories(elizaos-agentbrowser PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    PRIVATE
    ${CURL_INCLUDE_DIRS}
    ${GUMBO_INCLUDE_DIRS}
)
```

#### Step 3: Replace Mock Functions

The `agentbrowser_real.cpp` file already contains the real implementation for most functions. The remaining mock functions in `agentbrowser.cpp` need to be replaced with calls to the real HTML parser.

**Example: `findElement()`**

```cpp
// In agentbrowser_real.cpp

std::optional<WebElement> AgentBrowser::findElement(const std::string& selector, SelectorType type) {
    // ... (validation)
    
    auto* parser = static_cast<browser_impl::HtmlParser*>(htmlParser_);
    if (!parser) return std::nullopt;

    GumboNode* node = nullptr;
    if (type == SelectorType::CSS) {
        // Gumbo does not directly support CSS selectors, so we parse manually
        if (selector[0] == '#') { // ID selector
            node = parser->findById(parser->getRootNode(), selector.substr(1));
        } else if (selector[0] == '.') { // Class selector
            std::vector<GumboNode*> results;
            parser->findByClass(parser->getRootNode(), selector.substr(1), results);
            if (!results.empty()) node = results[0];
        } else { // Tag selector
            GumboTag tag = gumbo_tag_enum(selector.c_str());
            if (tag != GUMBO_TAG_UNKNOWN) {
                std::vector<GumboNode*> results;
                parser->findByTag(parser->getRootNode(), tag, results);
                if (!results.empty()) node = results[0];
            }
        }
    } else { // XPath (not supported by Gumbo)
        return std::nullopt;
    }

    if (node) {
        return parser->nodeToElement(node);
    }

    return std::nullopt;
}
```

#### Step 4: Testing

- **Unit Tests:** Create a new test suite for `AgentBrowser` to verify:
  - Successful page navigation.
  - Correct HTML parsing and data extraction (title, links, text).
  - Accurate element finding by ID, class, and tag.
- **Integration Tests:** Create a demo that uses the real `AgentBrowser` to navigate to a website, extract information, and store it in `AgentMemory`.

---

## 3. Discord Summarizer Integration Plan

### 3.1. Goal

The goal is to replace the mock `DiscordClient` with a real implementation that can connect to the Discord API, fetch messages, and send messages. This will enable the `DiscordSummarizer` to perform its intended function.

### 3.2. Current Status

- **Mock Implementation:** `cpp/discord_summarizer/src/discord_summarizer.cpp` contains a `MockDiscordClient` with hardcoded responses.
- **Dependencies:** No Discord API library is currently used.

### 3.3. Action Plan

#### Step 1: Choose and Install a Discord Library

We recommend using the **D++ (D++ Nightly)** library for its modern C++ features, extensive documentation, and active development.

**Installation:**
```bash
# Add the D++ nightly repository and install
wget -O - https://nightly.dpp.dev/install-nightly.sh | bash
sudo apt-get update && sudo apt-get install -y libdpp-dev
```

#### Step 2: Create a Real Discord Client

Create a new class `DiscordClientReal` that inherits from the `DiscordClient` interface and uses D++ to interact with the Discord API.

**`discord_client_real.hpp`:**
```cpp
#include "elizaos/discord_summarizer.hpp"
#include <dpp/dpp.h>

class DiscordClientReal : public elizaos::DiscordClient {
public:
    DiscordClientReal();
    ~DiscordClientReal();

    bool connect(const std::string& token) override;
    bool disconnect() override;
    // ... (implement all other interface methods)

private:
    std::unique_ptr<dpp::cluster> bot_;
};
```

#### Step 3: Implement the DiscordClient Interface

Implement all methods of the `DiscordClient` interface using the D++ library.

**Example: `getMessages()`**
```cpp
// In discord_client_real.cpp

std::vector<elizaos::DiscordMessage> DiscordClientReal::getMessages(const std::string& channelId, int limit) {
    std::vector<elizaos::DiscordMessage> messages;
    dpp::snowflake channel_id_sf(channelId);

    bot_->messages_get(channel_id_sf, limit, 0, 0, 0, [&messages](const dpp::confirmation_callback_t& callback) {
        if (!callback.is_error()) {
            dpp::message_map message_map = std::get<dpp::message_map>(callback.value);
            for (auto const& [key, val] : message_map) {
                elizaos::DiscordMessage msg;
                msg.id = std::to_string(val.id);
                msg.content = val.content;
                // ... (map other fields)
                messages.push_back(msg);
            }
        }
    });

    // Note: D++ is asynchronous, so this will require some mechanism to wait for the result.
    // A std::promise/future or a synchronous wrapper would be needed here.

    return messages;
}
```

#### Step 4: Update DiscordSummarizer

Modify the `DiscordSummarizer` to use the `DiscordClientReal` instead of the mock client. This can be done via a factory function or by directly instantiating the real client.

```cpp
// In discord_summarizer.cpp

DiscordSummarizer::DiscordSummarizer() {
    // Use the real client
    client_ = std::make_shared<DiscordClientReal>();
}
```

#### Step 5: Testing

- **Unit Tests:** Create a new test suite for `DiscordClientReal` to verify:
  - Successful connection to the Discord API with a bot token.
  - Correctly fetching messages from a channel.
  - Sending messages to a channel.
- **Integration Tests:** Create a demo that uses the `DiscordSummarizer` to connect to a Discord server, summarize a channel, and post the summary back to the channel.

---

## 4. Conclusion

This plan provides a clear path to completing the `AgentBrowser` and `DiscordSummarizer` components. By following these steps, we can replace the mock implementations with real, functional code, significantly enhancing the capabilities of the ElizaOS agent.

### Estimated Timeline

- **AgentBrowser Integration:** 2-3 days
- **Discord Summarizer Integration:** 4-5 days (due to the complexity of asynchronous API interaction)

Successful completion of this plan will mark a major milestone in the development of ElizaOS C++, bringing it closer to a fully-featured, production-ready cognitive agent framework.
