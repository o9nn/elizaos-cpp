#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "elizaos/agentbrowser.hpp"
#include <memory>
#include <string>

using namespace elizaos;
using namespace ::testing;

class AgentBrowserTest : public ::testing::Test {
protected:
    void SetUp() override {
        config_.headless = true;
        config_.windowWidth = 1280;
        config_.windowHeight = 720;
        config_.userAgent = "ElizaOS-Test/1.0";
        config_.pageLoadTimeout = 30;
        config_.elementTimeout = 10;
        config_.enableJavaScript = true;
        config_.enableImages = false;
    }

    BrowserConfig config_;
};

// ============================================================================
// BrowserConfig Tests
// ============================================================================

TEST_F(AgentBrowserTest, DefaultBrowserConfigValues) {
    BrowserConfig defaultConfig;

    EXPECT_TRUE(defaultConfig.headless);
    EXPECT_EQ(defaultConfig.windowWidth, 1280);
    EXPECT_EQ(defaultConfig.windowHeight, 720);
    EXPECT_EQ(defaultConfig.userAgent, "ElizaOS-Agent/1.0");
    EXPECT_EQ(defaultConfig.pageLoadTimeout, 30);
    EXPECT_EQ(defaultConfig.elementTimeout, 10);
    EXPECT_TRUE(defaultConfig.enableJavaScript);
    EXPECT_FALSE(defaultConfig.enableImages);
}

TEST_F(AgentBrowserTest, CustomBrowserConfigValues) {
    config_.headless = false;
    config_.windowWidth = 1920;
    config_.windowHeight = 1080;
    config_.userAgent = "CustomAgent/2.0";
    config_.pageLoadTimeout = 60;

    EXPECT_FALSE(config_.headless);
    EXPECT_EQ(config_.windowWidth, 1920);
    EXPECT_EQ(config_.windowHeight, 1080);
    EXPECT_EQ(config_.userAgent, "CustomAgent/2.0");
    EXPECT_EQ(config_.pageLoadTimeout, 60);
}

// ============================================================================
// WebElement Tests
// ============================================================================

TEST_F(AgentBrowserTest, WebElementCreation) {
    WebElement element;
    element.id = "submit-btn";
    element.tag = "button";
    element.text = "Submit";
    element.innerHTML = "<span>Submit</span>";
    element.isVisible = true;
    element.isEnabled = true;

    EXPECT_EQ(element.id, "submit-btn");
    EXPECT_EQ(element.tag, "button");
    EXPECT_EQ(element.text, "Submit");
    EXPECT_TRUE(element.isVisible);
    EXPECT_TRUE(element.isEnabled);
}

TEST_F(AgentBrowserTest, WebElementAttributes) {
    WebElement element;
    element.attributes["class"] = "btn btn-primary";
    element.attributes["data-testid"] = "submit-button";
    element.attributes["type"] = "submit";

    EXPECT_EQ(element.attributes.size(), 3);
    EXPECT_EQ(element.attributes["class"], "btn btn-primary");
    EXPECT_EQ(element.attributes["data-testid"], "submit-button");
    EXPECT_EQ(element.attributes["type"], "submit");
}

TEST_F(AgentBrowserTest, WebElementPositionAndSize) {
    WebElement element;
    element.x = 100;
    element.y = 200;
    element.width = 150;
    element.height = 50;

    EXPECT_EQ(element.x, 100);
    EXPECT_EQ(element.y, 200);
    EXPECT_EQ(element.width, 150);
    EXPECT_EQ(element.height, 50);
}

// ============================================================================
// PageInfo Tests
// ============================================================================

TEST_F(AgentBrowserTest, PageInfoCreation) {
    PageInfo page;
    page.url = "https://example.com";
    page.title = "Example Domain";
    page.html = "<html><body>Hello World</body></html>";
    page.isLoaded = true;

    EXPECT_EQ(page.url, "https://example.com");
    EXPECT_EQ(page.title, "Example Domain");
    EXPECT_TRUE(page.isLoaded);
}

TEST_F(AgentBrowserTest, PageInfoLinksAndImages) {
    PageInfo page;
    page.links.push_back("https://example.com/about");
    page.links.push_back("https://example.com/contact");
    page.images.push_back("https://example.com/logo.png");

    EXPECT_EQ(page.links.size(), 2);
    EXPECT_EQ(page.images.size(), 1);
    EXPECT_EQ(page.links[0], "https://example.com/about");
    EXPECT_EQ(page.images[0], "https://example.com/logo.png");
}

// ============================================================================
// BrowserResult Tests
// ============================================================================

TEST_F(AgentBrowserTest, BrowserResultSuccess) {
    BrowserResult result;
    result.result = BrowserActionResult::SUCCESS;
    result.message = "Navigation successful";
    result.data = "Page loaded";
    result.duration = std::chrono::milliseconds(500);

    EXPECT_TRUE(static_cast<bool>(result));
    EXPECT_EQ(result.message, "Navigation successful");
    EXPECT_TRUE(result.data.has_value());
    EXPECT_EQ(result.data.value(), "Page loaded");
    EXPECT_EQ(result.duration.count(), 500);
}

TEST_F(AgentBrowserTest, BrowserResultFailure) {
    BrowserResult result;
    result.result = BrowserActionResult::FAILED;
    result.message = "Connection refused";

    EXPECT_FALSE(static_cast<bool>(result));
    EXPECT_EQ(result.message, "Connection refused");
    EXPECT_FALSE(result.data.has_value());
}

TEST_F(AgentBrowserTest, BrowserResultTimeout) {
    BrowserResult result;
    result.result = BrowserActionResult::TIMEOUT;
    result.message = "Page load timeout after 30 seconds";

    EXPECT_FALSE(static_cast<bool>(result));
    EXPECT_EQ(result.result, BrowserActionResult::TIMEOUT);
}

TEST_F(AgentBrowserTest, BrowserResultElementNotFound) {
    BrowserResult result;
    result.result = BrowserActionResult::ELEMENT_NOT_FOUND;
    result.message = "Element #missing-id not found";

    EXPECT_FALSE(static_cast<bool>(result));
    EXPECT_EQ(result.result, BrowserActionResult::ELEMENT_NOT_FOUND);
}

TEST_F(AgentBrowserTest, BrowserResultNavigationError) {
    BrowserResult result;
    result.result = BrowserActionResult::NAVIGATION_ERROR;
    result.message = "DNS resolution failed";

    EXPECT_FALSE(static_cast<bool>(result));
    EXPECT_EQ(result.result, BrowserActionResult::NAVIGATION_ERROR);
}

// ============================================================================
// SelectorType Tests
// ============================================================================

TEST_F(AgentBrowserTest, SelectorTypeValues) {
    EXPECT_EQ(static_cast<int>(SelectorType::CSS), 0);
    EXPECT_EQ(static_cast<int>(SelectorType::XPATH), 1);
    EXPECT_EQ(static_cast<int>(SelectorType::ID), 2);
    EXPECT_EQ(static_cast<int>(SelectorType::CLASS_NAME), 3);
    EXPECT_EQ(static_cast<int>(SelectorType::TAG_NAME), 4);
}

// ============================================================================
// BrowserActionResult Tests
// ============================================================================

TEST_F(AgentBrowserTest, BrowserActionResultValues) {
    EXPECT_EQ(static_cast<int>(BrowserActionResult::SUCCESS), 0);
    EXPECT_EQ(static_cast<int>(BrowserActionResult::FAILED), 1);
    EXPECT_EQ(static_cast<int>(BrowserActionResult::TIMEOUT), 2);
    EXPECT_EQ(static_cast<int>(BrowserActionResult::ELEMENT_NOT_FOUND), 3);
    EXPECT_EQ(static_cast<int>(BrowserActionResult::NAVIGATION_ERROR), 4);
}
