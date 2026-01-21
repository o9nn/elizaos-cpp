// agentbrowser_real.cpp - Real HTTP and HTML parsing implementation for AgentBrowser
// This file replaces the mock implementation with real web automation capabilities
// using libcurl for HTTP requests and gumbo-parser for HTML parsing

#include "elizaos/agentbrowser.hpp"
#include "elizaos/agentlogger.hpp"
#include "elizaos/agentmemory.hpp"

#ifdef AGENTBROWSER_REAL_IMPLEMENTATION

// Real implementation using libcurl and gumbo-parser
#include <curl/curl.h>
#include <gumbo.h>
#include <sstream>
#include <fstream>
#include <regex>
#include <iomanip>
#include <random>
#include <thread>
#include <algorithm>
#include <cctype>

namespace elizaos {

// ==============================================================================
// HTTP CLIENT IMPLEMENTATION
// ==============================================================================

namespace browser_impl {

// Callback function for libcurl to write response data
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

class HttpClient {
private:
    CURL* curl_;
    std::string lastUrl_;
    std::string lastHtml_;
    long lastHttpCode_;
    std::string lastError_;
    std::unordered_map<std::string, std::string> headers_;
    
public:
    HttpClient() : curl_(nullptr), lastHttpCode_(0) {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl_ = curl_easy_init();
    }
    
    ~HttpClient() {
        if (curl_) {
            curl_easy_cleanup(curl_);
        }
        curl_global_cleanup();
    }
    
    bool fetch(const std::string& url, int timeoutSeconds = 30) {
        if (!curl_) {
            lastError_ = "CURL not initialized";
            return false;
        }
        
        lastUrl_ = url;
        lastHtml_.clear();
        lastError_.clear();
        
        // Set URL
        curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
        
        // Set write callback
        curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &lastHtml_);
        
        // Set timeout
        curl_easy_setopt(curl_, CURLOPT_TIMEOUT, timeoutSeconds);
        curl_easy_setopt(curl_, CURLOPT_CONNECTTIMEOUT, 10);
        
        // Follow redirects
        curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl_, CURLOPT_MAXREDIRS, 5L);
        
        // Set user agent
        curl_easy_setopt(curl_, CURLOPT_USERAGENT, "ElizaOS-AgentBrowser/1.0");
        
        // SSL verification
#ifdef CURL_NO_SSL_VERIFY
        curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYHOST, 0L);
#else
        curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYHOST, 2L);
#endif
        
        // Perform request
        CURLcode res = curl_easy_perform(curl_);
        
        if (res != CURLE_OK) {
            lastError_ = curl_easy_strerror(res);
            return false;
        }
        
        // Get response code
        curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &lastHttpCode_);
        
        return lastHttpCode_ >= 200 && lastHttpCode_ < 300;
    }
    
    const std::string& getHtml() const { return lastHtml_; }
    const std::string& getUrl() const { return lastUrl_; }
    long getHttpCode() const { return lastHttpCode_; }
    const std::string& getError() const { return lastError_; }
    bool isSuccess() const { return lastHttpCode_ >= 200 && lastHttpCode_ < 300; }
};

// ==============================================================================
// HTML PARSER IMPLEMENTATION
// ==============================================================================

class HtmlParser {
private:
    GumboOutput* output_;
    std::string html_;
    std::string title_;
    std::string text_;
    std::vector<std::string> links_;
    std::vector<std::string> images_;
    
    // Helper to extract text from node
    void extractText(GumboNode* node, std::string& text) {
        if (node->type == GUMBO_NODE_TEXT) {
            text += node->v.text.text;
        } else if (node->type == GUMBO_NODE_ELEMENT) {
            GumboVector* children = &node->v.element.children;
            for (unsigned int i = 0; i < children->length; ++i) {
                extractText(static_cast<GumboNode*>(children->data[i]), text);
            }
        }
    }
    
    // Helper to find title tag
    void findTitle(GumboNode* node) {
        if (node->type != GUMBO_NODE_ELEMENT) return;
        
        if (node->v.element.tag == GUMBO_TAG_TITLE) {
            extractText(node, title_);
            return;
        }
        
        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) {
            findTitle(static_cast<GumboNode*>(children->data[i]));
        }
    }
    
    // Helper to find all links
    void findLinks(GumboNode* node) {
        if (node->type != GUMBO_NODE_ELEMENT) return;
        
        if (node->v.element.tag == GUMBO_TAG_A) {
            GumboAttribute* href = gumbo_get_attribute(&node->v.element.attributes, "href");
            if (href && href->value) {
                links_.push_back(href->value);
            }
        }
        
        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) {
            findLinks(static_cast<GumboNode*>(children->data[i]));
        }
    }
    
    // Helper to find all images
    void findImages(GumboNode* node) {
        if (node->type != GUMBO_NODE_ELEMENT) return;
        
        if (node->v.element.tag == GUMBO_TAG_IMG) {
            GumboAttribute* src = gumbo_get_attribute(&node->v.element.attributes, "src");
            if (src && src->value) {
                images_.push_back(src->value);
            }
        }
        
        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) {
            findImages(static_cast<GumboNode*>(children->data[i]));
        }
    }
    
public:
    HtmlParser() : output_(nullptr) {}
    
    ~HtmlParser() {
        if (output_) {
            gumbo_destroy_output(&kGumboDefaultOptions, output_);
        }
    }
    
    bool parse(const std::string& html) {
        if (output_) {
            gumbo_destroy_output(&kGumboDefaultOptions, output_);
            output_ = nullptr;
        }
        
        html_ = html;
        output_ = gumbo_parse(html.c_str());
        
        if (!output_) return false;
        
        // Extract metadata
        title_.clear();
        text_.clear();
        links_.clear();
        images_.clear();
        
        findTitle(output_->root);
        extractText(output_->root, text_);
        findLinks(output_->root);
        findImages(output_->root);
        
        return true;
    }
    
    bool isParsed() const { return output_ != nullptr; }
    GumboNode* getRootNode() const { return output_ ? output_->root : nullptr; }
    
    const std::string& getTitle() const { return title_; }
    const std::string& getText() const { return text_; }
    const std::string& getHtml() const { return html_; }
    const std::vector<std::string>& getLinks() const { return links_; }
    const std::vector<std::string>& getImages() const { return images_; }
    
    // Find element by ID
    GumboNode* findById(GumboNode* node, const std::string& id) {
        if (!node || node->type != GUMBO_NODE_ELEMENT) return nullptr;
        
        GumboAttribute* idAttr = gumbo_get_attribute(&node->v.element.attributes, "id");
        if (idAttr && idAttr->value && id == idAttr->value) {
            return node;
        }
        
        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) {
            GumboNode* result = findById(static_cast<GumboNode*>(children->data[i]), id);
            if (result) return result;
        }
        
        return nullptr;
    }
    
    // Find elements by class
    void findByClass(GumboNode* node, const std::string& className, std::vector<GumboNode*>& results) {
        if (!node || node->type != GUMBO_NODE_ELEMENT) return;
        
        GumboAttribute* classAttr = gumbo_get_attribute(&node->v.element.attributes, "class");
        if (classAttr && classAttr->value) {
            std::string classes = classAttr->value;
            if (classes.find(className) != std::string::npos) {
                results.push_back(node);
            }
        }
        
        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) {
            findByClass(static_cast<GumboNode*>(children->data[i]), className, results);
        }
    }
    
    // Find elements by tag
    void findByTag(GumboNode* node, GumboTag tag, std::vector<GumboNode*>& results) {
        if (!node || node->type != GUMBO_NODE_ELEMENT) return;
        
        if (node->v.element.tag == tag) {
            results.push_back(node);
        }
        
        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) {
            findByTag(static_cast<GumboNode*>(children->data[i]), tag, results);
        }
    }
    
    // Convert GumboNode to WebElement
    WebElement nodeToElement(GumboNode* node) {
        WebElement element;
        
        if (!node || node->type != GUMBO_NODE_ELEMENT) return element;
        
        // Set tag name
        element.tag = gumbo_normalized_tagname(node->v.element.tag);
        
        // Extract attributes
        GumboVector* attrs = &node->v.element.attributes;
        for (unsigned int i = 0; i < attrs->length; ++i) {
            GumboAttribute* attr = static_cast<GumboAttribute*>(attrs->data[i]);
            element.attributes[attr->name] = attr->value;
            
            if (std::string(attr->name) == "id") {
                element.id = attr->value;
            }
        }
        
        // Extract text content
        extractText(node, element.text);
        
        // Note: innerHTML, position, and visibility require more complex extraction
        element.isVisible = true;
        element.isEnabled = true;
        
        return element;
    }
};

} // namespace browser_impl

// ==============================================================================
// AGENTBROWSER REAL IMPLEMENTATION
// ==============================================================================

// Utility functions
namespace browser_utils {
    bool isValidUrl(const std::string& url) {
        std::regex urlPattern(R"(^https?://[^\s/$.?#].[^\s]*$)", std::regex::icase);
        return std::regex_match(url, urlPattern);
    }
}

BrowserResult AgentBrowser::initializeBrowserDriver() {
    try {
        browserDriver_ = new browser_impl::HttpClient();
        htmlParser_ = new browser_impl::HtmlParser();
        
        if (logger_) {
            logger_->log("Initialized HTTP client and HTML parser (REAL implementation)", 
                        "agentbrowser", "Browser Init", LogLevel::SUCCESS);
        }
        
        return {BrowserActionResult::SUCCESS, "Browser driver initialized (HTTP + HTML parser)", 
                std::nullopt, std::chrono::milliseconds(0)};
    } catch (const std::exception& e) {
        return {BrowserActionResult::FAILED, std::string("Failed to initialize: ") + e.what(), 
                std::nullopt, std::chrono::milliseconds(0)};
    }
}

void AgentBrowser::shutdownBrowserDriver() {
    if (browserDriver_) {
        delete static_cast<browser_impl::HttpClient*>(browserDriver_);
        browserDriver_ = nullptr;
    }
    
    if (htmlParser_) {
        delete static_cast<browser_impl::HtmlParser*>(htmlParser_);
        htmlParser_ = nullptr;
    }
    
    if (logger_) {
        logger_->log("Shutdown HTTP client and HTML parser", "agentbrowser", 
                    "Browser Shutdown", LogLevel::SUCCESS);
    }
}

BrowserResult AgentBrowser::navigateTo(const std::string& url) {
    if (!initialized_.load()) {
        return {BrowserActionResult::FAILED, "Browser not initialized", std::nullopt, std::chrono::milliseconds(0)};
    }
    
    if (!browser_utils::isValidUrl(url)) {
        return {BrowserActionResult::FAILED, "Invalid URL: " + url, std::nullopt, std::chrono::milliseconds(0)};
    }
    
    auto start = std::chrono::steady_clock::now();
    std::lock_guard<std::mutex> lock(sessionMutex_);
    
    auto* httpClient = static_cast<browser_impl::HttpClient*>(browserDriver_);
    auto* parser = static_cast<browser_impl::HtmlParser*>(htmlParser_);
    
    if (!httpClient || !parser) {
        return {BrowserActionResult::FAILED, "Browser driver not properly initialized", 
                std::nullopt, std::chrono::milliseconds(0)};
    }
    
    // Fetch the page using real HTTP request
    bool success = httpClient->fetch(url, config_.pageLoadTimeout);
    
    if (!success) {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start);
        
        std::string errorMsg = "Failed to fetch URL: " + httpClient->getError();
        logAction("navigate", {BrowserActionResult::NAVIGATION_ERROR, errorMsg, url, duration});
        
        return {BrowserActionResult::NAVIGATION_ERROR, errorMsg, url, duration};
    }
    
    // Parse the HTML
    const std::string& html = httpClient->getHtml();
    if (!parser->parse(html)) {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start);
        
        return {BrowserActionResult::FAILED, "Failed to parse HTML", url, duration};
    }
    
    // Update current URL and stats
    currentUrl_ = url;
    stats_.pagesVisited++;
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start);
    
    updateStatistics("navigation", duration);
    
    std::string message = "Successfully navigated to: " + url + 
                         " (HTTP " + std::to_string(httpClient->getHttpCode()) + ")";
    logAction("navigate", {BrowserActionResult::SUCCESS, message, url, duration});
    
    if (logger_) {
        logger_->log(message, "agentbrowser", "Navigation", LogLevel::SUCCESS);
    }
    
    return {BrowserActionResult::SUCCESS, message, url, duration};
}

std::optional<PageInfo> AgentBrowser::getCurrentPageInfo() {
    if (!initialized_.load()) {
        return std::nullopt;
    }
    
    std::lock_guard<std::mutex> lock(sessionMutex_);
    
    auto* parser = static_cast<browser_impl::HtmlParser*>(htmlParser_);
    if (!parser || !parser->isParsed()) {
        return std::nullopt;
    }
    
    PageInfo info;
    info.url = currentUrl_;
    info.title = parser->getTitle();
    info.html = parser->getHtml();
    info.links = parser->getLinks();
    info.images = parser->getImages();
    info.loadTime = std::chrono::system_clock::now();
    info.isLoaded = true;
    
    return info;
}

std::optional<WebElement> AgentBrowser::findElement(const std::string& selector, SelectorType type) {
    if (!initialized_.load()) {
        return std::nullopt;
    }
    
    auto validationResult = validateSelector(selector, type);
    if (!validationResult) {
        return std::nullopt;
    }
    
    std::lock_guard<std::mutex> lock(sessionMutex_);
    
    auto* parser = static_cast<browser_impl::HtmlParser*>(htmlParser_);
    if (!parser || !parser->isParsed()) {
        return std::nullopt;
    }

    GumboNode* node = nullptr;
    
    if (type == SelectorType::CSS) {
        if (selector.empty()) return std::nullopt;
        
        if (selector[0] == '#') {
            // ID selector: #myId
            std::string id = selector.substr(1);
            node = parser->findById(parser->getRootNode(), id);
        } 
        else if (selector[0] == '.') {
            // Class selector: .myClass
            std::string className = selector.substr(1);
            std::vector<GumboNode*> results;
            parser->findByClass(parser->getRootNode(), className, results);
            if (!results.empty()) node = results[0];
        } 
        else {
            // Tag selector: div, span, etc.
            GumboTag tag = gumbo_tag_enum(selector.c_str());
            if (tag != GUMBO_TAG_UNKNOWN) {
                std::vector<GumboNode*> results;
                parser->findByTag(parser->getRootNode(), tag, results);
                if (!results.empty()) node = results[0];
            }
        }
    } 
    else if (type == SelectorType::XPATH) {
        // XPath is not natively supported by Gumbo
        if (logger_) {
            logger_->log("XPath selectors are not supported in real implementation", 
                        "agentbrowser", "Element Finding", LogLevel::WARNING);
        }
        return std::nullopt;
    }

    if (node) {
        return parser->nodeToElement(node);
    }

    return std::nullopt;
}

std::vector<WebElement> AgentBrowser::findElements(const std::string& selector, SelectorType type) {
    if (!initialized_.load()) {
        return {};
    }
    
    auto validationResult = validateSelector(selector, type);
    if (!validationResult) {
        return {};
    }
    
    std::lock_guard<std::mutex> lock(sessionMutex_);
    
    auto* parser = static_cast<browser_impl::HtmlParser*>(htmlParser_);
    if (!parser || !parser->isParsed()) {
        return {};
    }

    std::vector<GumboNode*> nodes;
    
    if (type == SelectorType::CSS) {
        if (selector.empty()) return {};
        
        if (selector[0] == '#') {
            // ID selector: #myId (should return 0 or 1 element)
            std::string id = selector.substr(1);
            GumboNode* node = parser->findById(parser->getRootNode(), id);
            if (node) nodes.push_back(node);
        } 
        else if (selector[0] == '.') {
            // Class selector: .myClass
            std::string className = selector.substr(1);
            parser->findByClass(parser->getRootNode(), className, nodes);
        } 
        else {
            // Tag selector: div, span, etc.
            GumboTag tag = gumbo_tag_enum(selector.c_str());
            if (tag != GUMBO_TAG_UNKNOWN) {
                parser->findByTag(parser->getRootNode(), tag, nodes);
            }
        }
    }

    // Convert GumboNodes to WebElements
    std::vector<WebElement> elements;
    for (auto* node : nodes) {
        elements.push_back(parser->nodeToElement(node));
    }

    return elements;
}

std::vector<uint8_t> AgentBrowser::takeScreenshot() {
    if (!initialized_.load()) {
        return {};
    }
    
    // Screenshot functionality requires a headless browser (Puppeteer/Selenium)
    // This is beyond the scope of libcurl + gumbo-parser
    if (logger_) {
        logger_->log("Screenshot functionality not available in HTTP-only implementation", 
                    "agentbrowser", "Screenshot", LogLevel::WARNING);
    }
    
    return {};
}

} // namespace elizaos

#else // !AGENTBROWSER_REAL_IMPLEMENTATION

// ==============================================================================
// MOCK IMPLEMENTATION (Fallback when real implementation is not enabled)
// ==============================================================================

#include "elizaos/agentbrowser.hpp"
#include "elizaos/agentlogger.hpp"

namespace elizaos {

BrowserResult AgentBrowser::initializeBrowserDriver() {
    if (logger_) {
        logger_->log("Using MOCK browser implementation (real implementation not enabled)", 
                    "agentbrowser", "Browser Init", LogLevel::WARNING);
    }
    return {BrowserActionResult::SUCCESS, "Mock browser driver initialized", 
            std::nullopt, std::chrono::milliseconds(0)};
}

void AgentBrowser::shutdownBrowserDriver() {
    // No-op for mock
}

BrowserResult AgentBrowser::navigateTo(const std::string& url) {
    if (logger_) {
        logger_->log("Mock navigation to: " + url, "agentbrowser", "Navigation", LogLevel::INFO);
    }
    currentUrl_ = url;
    return {BrowserActionResult::SUCCESS, "Mock navigation successful", url, std::chrono::milliseconds(100)};
}

std::optional<PageInfo> AgentBrowser::getCurrentPageInfo() {
    PageInfo info;
    info.url = currentUrl_;
    info.title = "Mock Page Title";
    info.html = "<html><body>Mock content</body></html>";
    info.isLoaded = true;
    return info;
}

std::optional<WebElement> AgentBrowser::findElement(const std::string& selector, SelectorType /* type */) {
    WebElement element;
    element.id = "mock_" + selector;
    element.tag = "div";
    element.text = "Mock element";
    return element;
}

std::vector<WebElement> AgentBrowser::findElements(const std::string& selector, SelectorType /* type */) {
    std::vector<WebElement> elements;
    WebElement element;
    element.id = "mock_" + selector;
    element.tag = "div";
    element.text = "Mock element";
    elements.push_back(element);
    return elements;
}

std::vector<uint8_t> AgentBrowser::takeScreenshot() {
    return {};
}

} // namespace elizaos

#endif // AGENTBROWSER_REAL_IMPLEMENTATION
