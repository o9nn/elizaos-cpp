# ElizaOS-CPP Implementation Plan

**Date:** December 12, 2024  
**Priority:** Complete functional implementation without mocks

---

## Phase 1: Critical Infrastructure Fixes (IMMEDIATE)

### 1.1 Fix Test Discovery
**Status:** ✅ Already configured correctly
- Tests are using `gtest_discover_tests(elizaos_tests)` which is correct
- The issue is that GoogleTest discovery happens at build time
- Need to rebuild tests to discover all test cases

**Action:** No changes needed - tests are properly configured

### 1.2 Install Missing Dependencies
**Priority:** HIGH
**Estimated Time:** 30 minutes

```bash
# Install libsndfile for audio processing
sudo apt-get install -y libsndfile1-dev

# Rebuild with audio support
cd build && cmake .. && make -j$(nproc)
```

**Files Affected:**
- `cpp/ljspeechtools/src/ljspeechtools.cpp`

---

## Phase 2: AgentBrowser Real Implementation (HIGH PRIORITY)

### 2.1 Browser Integration Options

#### Option A: libcurl + HTML Parser (RECOMMENDED - Quickest)
**Pros:**
- libcurl already available and used
- No heavy dependencies
- Fast implementation
- Sufficient for most agent tasks

**Cons:**
- No JavaScript execution
- No dynamic content rendering
- Limited to static HTML parsing

**Implementation:**
1. Use libcurl for HTTP requests (already available)
2. Add HTML parser library (e.g., gumbo-parser or lexbor)
3. Implement DOM traversal and element selection
4. Screenshot: Generate text-based representation or skip

**Estimated Time:** 2-3 days

#### Option B: Chrome DevTools Protocol (CDP) via WebSocket
**Pros:**
- Full browser automation
- JavaScript execution
- Real screenshots
- Industry standard

**Cons:**
- Requires Chrome/Chromium installed
- More complex implementation
- Heavier resource usage

**Implementation:**
1. Add WebSocket library (websocketpp or libwebsockets)
2. Implement CDP protocol client
3. Launch Chrome in headless mode
4. Full browser automation capabilities

**Estimated Time:** 5-7 days

#### Option C: Playwright C++ (if available) or Selenium WebDriver
**Pros:**
- Full-featured browser automation
- Well-documented APIs
- Cross-browser support

**Cons:**
- Heavy dependencies
- Complex build setup
- May not have mature C++ bindings

**Estimated Time:** 7-10 days

### 2.2 Recommended Approach: Option A + Option B Hybrid

**Phase 2A: Quick Win (2-3 days)**
- Implement libcurl + HTML parser for basic functionality
- Real HTTP requests and HTML parsing
- CSS selector support
- Text extraction
- Link and image extraction

**Phase 2B: Full Feature (5-7 days later)**
- Add CDP support for advanced features
- JavaScript execution
- Real screenshots
- Dynamic content handling

### 2.3 Implementation Steps for Phase 2A

#### Step 1: Add HTML Parser Dependency
```cmake
# In CMakeLists.txt
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(GUMBO QUIET gumbo)
endif()
```

#### Step 2: Implement Real HTTP Client
```cpp
// Use libcurl for actual HTTP requests
class CurlHttpClient {
public:
    std::string fetch(const std::string& url);
    std::string getHtml();
    std::vector<uint8_t> getScreenshot(); // Text-based or skip
};
```

#### Step 3: Implement HTML Parser
```cpp
// Parse HTML and build DOM tree
class HtmlParser {
public:
    void parse(const std::string& html);
    std::optional<WebElement> findElement(const std::string& selector);
    std::vector<WebElement> findElements(const std::string& selector);
    std::string getText();
    std::vector<std::string> getLinks();
    std::vector<std::string> getImages();
};
```

#### Step 4: Replace Mock Implementations
**Files to modify:**
- `cpp/agentbrowser/src/agentbrowser.cpp`
  - `initializeBrowserDriver()` - Initialize HTTP client
  - `navigateTo()` - Real HTTP request
  - `getPageHTML()` - Return actual HTML
  - `getPageText()` - Parse and extract text
  - `findElement()` - Real CSS selector parsing
  - `findElements()` - Real multi-element selection
  - `getLinks()` - Parse <a> tags
  - `getImages()` - Parse <img> tags
  - `executeScript()` - Return error or basic eval
  - `takeScreenshot()` - Skip or text representation

---

## Phase 3: Discord Integration (MEDIUM PRIORITY)

### 3.1 Discord Library Integration
**Recommended:** D++ (DPP) - Modern C++ Discord library

**Installation:**
```bash
# Install D++
git clone https://github.com/brainboxdotcc/DPP.git
cd DPP
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
```

**CMake Integration:**
```cmake
find_package(dpp REQUIRED)
target_link_libraries(elizaos-discord_summarizer dpp)
```

### 3.2 Replace Mock Discord Client
**File:** `cpp/discord_summarizer/src/discord_summarizer.cpp`

**Implementation:**
```cpp
class RealDiscordClient : public DiscordClient {
private:
    dpp::cluster bot;
    
public:
    RealDiscordClient(const std::string& token);
    std::vector<DiscordMessage> fetchMessages(const std::string& channelId, int limit) override;
    std::vector<DiscordMessage> fetchMessagesSince(const std::string& channelId, 
                                                     const std::chrono::system_clock::time_point& since) override;
};
```

**Estimated Time:** 2-3 days

---

## Phase 4: Remove Placeholder Modules (LOW PRIORITY)

### 4.1 Modules with Placeholder Implementations

#### A. brandkit (`cpp/brandkit/src/placeholder.cpp`)
**Options:**
1. Implement brand asset management system
2. Remove module if not essential
3. Convert to data-only module (JSON configs)

**Recommendation:** Remove or convert to data module

#### B. eliza_nextjs_starter (`cpp/eliza_nextjs_starter/src/placeholder.cpp`)
**Options:**
1. Implement Next.js project generator
2. Remove - this should be a separate npm package
3. Keep as documentation reference

**Recommendation:** Remove from C++ build, keep as docs/templates

#### C. eliza_plugin_starter (`cpp/eliza_plugin_starter/src/placeholder.cpp`)
**Options:**
1. Implement plugin scaffolding generator
2. Remove - should be templates
3. Keep as example code

**Recommendation:** Remove from C++ build, keep as templates

#### D. autonomous_starter (`cpp/autonomous_starter/src/autonomous_starter.cpp`)
**Current Status:** Has some implementation but with placeholder function

**Action:** Review and complete implementation or remove placeholder

### 4.2 Implementation Steps

**Step 1: Evaluate each module**
```bash
# Check usage of each placeholder module
grep -r "brandkit\|eliza_nextjs_starter\|eliza_plugin_starter" cpp/ include/
```

**Step 2: Remove unused modules**
```cmake
# Comment out in CMakeLists.txt
# add_subdirectory(cpp/brandkit)
# add_subdirectory(cpp/eliza_nextjs_starter)
# add_subdirectory(cpp/eliza_plugin_starter)
```

**Step 3: Move to templates directory**
```bash
mkdir -p templates/starters
mv eliza_nextjs_starter templates/starters/
mv eliza_plugin_starter templates/starters/
```

**Estimated Time:** 1 day

---

## Phase 5: Enhance Memory Attention Mechanisms (OPTIONAL)

### 5.1 Current Placeholders
**File:** `cpp/agentmemory/src/attention.cpp`

```cpp
// This is a placeholder for more sophisticated consolidation logic
// This is a placeholder for more sophisticated link creation
```

### 5.2 ECAN Implementation
**Estimated Time:** 5-7 days

**Features to implement:**
1. Short-term importance (STI) calculation
2. Long-term importance (LTI) calculation
3. Hebbian learning for link creation
4. Attention spreading algorithm
5. Forgetting mechanism

**Recommendation:** Defer to Phase 6 (after core functionality complete)

---

## Phase 6: GitHub Actions Optimization

### 6.1 Current Status
✅ Packaging workflows exist and are well-configured
✅ CPack is properly included
✅ Debian control scripts exist
✅ Chocolatey configuration exists

### 6.2 Potential Improvements

#### A. Add Explicit Test Running
**File:** `.github/workflows/cpp-build.yml`

Add explicit test execution with verbose output:
```yaml
- name: Run all tests
  working-directory: build
  run: |
    ctest --verbose --output-on-failure
    ./elizaos_tests --gtest_list_tests
    ./elizaos_tests
```

#### B. Verify Package Contents
Add package verification step:
```yaml
- name: Verify DEB package contents
  run: |
    dpkg-deb -c build/*.deb
    dpkg-deb -I build/*.deb
```

#### C. Add APT Repository Publishing
**New File:** `.github/workflows/apt-publish.yml`

Create workflow to publish to APT repository (requires setup)

#### D. Add Homebrew Formula
**New File:** `Formula/elizaos-cpp.rb`

Create Homebrew formula for macOS distribution

**Estimated Time:** 2-3 days

---

## Implementation Timeline

### Week 1: Critical Fixes
- ✅ Day 1: Install libsndfile, rebuild, verify tests
- 🔧 Day 2-3: Implement libcurl + HTML parser for AgentBrowser
- 🔧 Day 4-5: Replace AgentBrowser mock implementations

### Week 2: Integration
- 🔧 Day 1-2: Test AgentBrowser with real websites
- 🔧 Day 3-4: Implement Discord integration with D++
- 🔧 Day 5: Remove/relocate placeholder modules

### Week 3: Advanced Features & Polish
- 🔧 Day 1-2: Add CDP support for AgentBrowser (optional)
- 🔧 Day 3-4: Enhance memory attention mechanisms (optional)
- 🔧 Day 5: GitHub Actions optimization

### Week 4: Testing & Documentation
- ✅ Day 1-2: Comprehensive testing of all modules
- ✅ Day 3-4: Update documentation
- ✅ Day 5: Final validation and release

---

## Quick Start: Immediate Actions

### Action 1: Install Dependencies (5 minutes)
```bash
cd /home/ubuntu/elizaos-cpp
sudo apt-get install -y libsndfile1-dev libgumbo-dev
```

### Action 2: Add HTML Parser to CMake (10 minutes)
```cmake
# Add to CMakeLists.txt after libcurl
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(GUMBO QUIET gumbo)
endif()
```

### Action 3: Create Real HTTP Client (2 hours)
Create `cpp/agentbrowser/src/http_client.cpp` with libcurl implementation

### Action 4: Implement HTML Parser Wrapper (2 hours)
Create `cpp/agentbrowser/src/html_parser.cpp` with gumbo parser

### Action 5: Replace Mock Methods (3 hours)
Update `agentbrowser.cpp` to use real HTTP client and HTML parser

### Action 6: Test & Validate (1 hour)
Run tests and verify real web pages can be fetched and parsed

**Total Time for Quick Win:** ~1 day of focused work

---

## Success Criteria

### Phase 1 Complete:
- ✅ All tests discovered and running
- ✅ libsndfile integrated, audio processing functional
- ✅ No build errors or warnings

### Phase 2 Complete:
- ✅ AgentBrowser can fetch real web pages
- ✅ HTML parsing and element selection works
- ✅ No mock implementations in AgentBrowser
- ✅ All AgentBrowser tests passing

### Phase 3 Complete:
- ✅ Discord client connects to real Discord API
- ✅ Can fetch messages and channels
- ✅ Summarization works with real data

### Phase 4 Complete:
- ✅ No placeholder modules in build
- ✅ All modules have real implementations or removed
- ✅ Clean build with no warnings

### Final Success:
- ✅ 0 mock implementations in codebase
- ✅ 0 placeholder functions
- ✅ All 318 tests passing
- ✅ All packages build successfully
- ✅ GitHub Actions all green
- ✅ Documentation updated

---

**Next Step:** Begin Phase 1 - Install dependencies and start AgentBrowser implementation
