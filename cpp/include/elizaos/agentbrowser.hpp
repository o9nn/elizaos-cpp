#pragma once

/**
 * ElizaOS C++ - AgentBrowser Module
 *
 * Web automation: browser control, page parsing, form interaction, and
 * screenshot capture via simulated/mock browser.
 */

#include "elizaos.hpp"
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// ============================================================================
// BrowserConfig
// ============================================================================

struct BrowserConfig {
    bool        headless          = true;
    int         windowWidth       = 1280;
    int         windowHeight      = 720;
    std::string userAgent         = "ElizaOS-Agent/1.0";
    int         pageLoadTimeout   = 30;
    int         elementTimeout    = 10;
    bool        enableJavaScript  = true;
    bool        enableImages      = false;
    std::string downloadDirectory = "/tmp";
    std::string profileDirectory  = "";
    std::string proxyServer       = "";
};

// ============================================================================
// BrowserResult
// ============================================================================

struct BrowserResult {
    bool        success = false;
    std::string message;
    int         errorCode = 0;

    static BrowserResult ok(const std::string& msg = "") { return {true, msg, 0}; }
    static BrowserResult fail(const std::string& msg, int code = -1) { return {false, msg, code}; }
};

// ============================================================================
// PageInfo
// ============================================================================

struct PageInfo {
    std::string url;
    std::string title;
    std::string contentType;
    int         statusCode = 0;
    bool        isLoaded   = false;
};

// ============================================================================
// SelectorType
// ============================================================================

enum class SelectorType {
    CSS,
    XPATH,
    ID,
    NAME,
    CLASS_NAME,
    TAG_NAME,
    LINK_TEXT,
    PARTIAL_LINK_TEXT
};

// ============================================================================
// WebElement
// ============================================================================

struct WebElement {
    std::string                                  elementId;
    std::string                                  tagName;
    std::string                                  text;
    std::string                                  value;
    bool                                         visible  = false;
    bool                                         enabled  = false;
    bool                                         selected = false;
    std::unordered_map<std::string, std::string> attributes;
};

// ============================================================================
// AgentBrowser
// ============================================================================

class AgentBrowser {
public:
    explicit AgentBrowser(const BrowserConfig& config = BrowserConfig{});
    ~AgentBrowser();

    // Lifecycle
    BrowserResult initialize();
    BrowserResult shutdown();

    // Navigation
    BrowserResult                navigateTo(const std::string& url);
    BrowserResult                goBack();
    BrowserResult                goForward();
    BrowserResult                refresh();
    BrowserResult                waitForPageLoad(int timeoutSec = 30);

    // Page info
    std::optional<PageInfo>      getCurrentPageInfo();
    std::optional<std::string>   getPageTitle();
    std::optional<std::string>   getPageText();
    std::optional<std::string>   getPageHTML();
    std::vector<std::string>     getLinks();
    std::vector<std::string>     getImages();

    // Element interaction
    std::optional<WebElement>    findElement(const std::string& selector,
                                             SelectorType type = SelectorType::CSS);
    std::vector<WebElement>      findElements(const std::string& selector,
                                              SelectorType type = SelectorType::CSS);
    BrowserResult                clickElement(const std::string& selector,
                                              SelectorType type = SelectorType::CSS);
    BrowserResult                typeText(const std::string& selector,
                                          const std::string& text,
                                          SelectorType type = SelectorType::CSS);
    BrowserResult                clearText(const std::string& selector,
                                           SelectorType type = SelectorType::CSS);

    // Form interaction
    BrowserResult                fillForm(const std::unordered_map<std::string, std::string>& formData);
    BrowserResult                submitForm(const std::string& formSelector);
    BrowserResult                selectOption(const std::string& selector, const std::string& value);
    BrowserResult                checkCheckbox(const std::string& selector, bool checked = true);

    // JavaScript
    BrowserResult                executeJavaScript(const std::string& script);
    std::optional<std::string>   evaluateJavaScript(const std::string& expression);

    // Scrolling
    BrowserResult                scrollToElement(const std::string& selector);
    BrowserResult                scrollBy(int x, int y);

    // Screenshots
    BrowserResult                captureScreenshot(const std::string& filename);

    // State
    bool isInitialized() const { return initialized_; }

private:
    BrowserConfig config_;
    bool          initialized_ = false;
    PageInfo      currentPage_;
    std::string   currentHTML_;

    mutable std::mutex browserMutex_;
};

} // namespace elizaos
