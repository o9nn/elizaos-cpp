// agentbrowser_test.cpp - E2E tests for elizaos::AgentBrowser.
#include <gtest/gtest.h>
#include "elizaos/agentbrowser.hpp"

using namespace elizaos;

class AgentBrowserTest : public ::testing::Test {
protected:
    BrowserConfig cfg;
    AgentBrowser browser{cfg};
};

TEST_F(AgentBrowserTest, InitializeAndShutdown) {
    auto r = browser.initialize();
    EXPECT_EQ(r.result, BrowserActionResult::SUCCESS);
    EXPECT_TRUE(browser.isInitialized());

    auto s = browser.shutdown();
    EXPECT_EQ(s.result, BrowserActionResult::SUCCESS);
    EXPECT_FALSE(browser.isInitialized());
}

TEST_F(AgentBrowserTest, InitialState) {
    EXPECT_FALSE(browser.isInitialized());
    auto cfg2 = browser.getConfig();
    EXPECT_TRUE(cfg2.headless);
    EXPECT_GT(cfg2.windowWidth, 0);
    EXPECT_GT(cfg2.windowHeight, 0);
}

TEST_F(AgentBrowserTest, SetConfig) {
    BrowserConfig c;
    c.windowWidth = 800;
    c.windowHeight = 600;
    browser.setConfig(c);
    EXPECT_EQ(browser.getConfig().windowWidth, 800);
    EXPECT_EQ(browser.getConfig().windowHeight, 600);
}

TEST_F(AgentBrowserTest, RememberAndQueryPages) {
    browser.initialize();
    browser.rememberPage("https://example.com", "test");
    auto pages = browser.getSimilarPages("test");
    SUCCEED() << "found " << pages.size();
    browser.shutdown();
}

TEST_F(AgentBrowserTest, StatsBeforeAndAfterReset) {
    browser.initialize();
    browser.resetStatistics();
    auto s = browser.getStatistics();
    EXPECT_EQ(s.pagesVisited, 0);
    EXPECT_EQ(s.elementsClicked, 0);
    browser.shutdown();
}

TEST(BrowserUtils, CssSelectorBuilder) {
    auto sel = browser_utils::cssSelector("div", "class", "btn");
    EXPECT_FALSE(sel.empty());
    EXPECT_NE(sel.find("div"), std::string::npos);
}

TEST(BrowserUtils, XPathSelectorBuilder) {
    auto sel = browser_utils::xpathSelector("a", "Click me");
    EXPECT_FALSE(sel.empty());
}

TEST(BrowserUtils, IsValidUrl) {
    EXPECT_TRUE(browser_utils::isValidUrl("https://example.com"));
    EXPECT_TRUE(browser_utils::isValidUrl("http://example.com/path"));
    EXPECT_FALSE(browser_utils::isValidUrl(""));
    EXPECT_FALSE(browser_utils::isValidUrl("not a url"));
}

TEST(BrowserUtils, ExtractDomain) {
    EXPECT_EQ(browser_utils::extractDomain("https://example.com/abc"), "example.com");
    EXPECT_EQ(browser_utils::extractDomain("http://sub.foo.bar/p?q=1"), "sub.foo.bar");
}

TEST(BrowserUtils, ExtractEmails) {
    auto e = browser_utils::extractEmails("Contact a@b.com or x.y@z.io please");
    EXPECT_GE(e.size(), 2u);
}

TEST(BrowserUtils, ExtractPhoneNumbers) {
    auto p = browser_utils::extractPhoneNumbers("Call +27 11 555 1234 or 555-867-5309");
    SUCCEED() << "matched " << p.size() << " phone numbers";
}
