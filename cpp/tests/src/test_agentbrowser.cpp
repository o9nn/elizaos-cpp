#include <gtest/gtest.h>
#include "elizaos/agentbrowser.hpp"
#include <memory>
#include <string>
#include <atomic>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <optional>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <vector>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#define SHUT_RDWR SD_BOTH
#define MSG_NOSIGNAL 0
inline int test_close_socket(int fd) { return ::closesocket(fd); }
using socklen_t = int;
#ifndef _SSIZE_T_DEFINED
using ssize_t = int;
#define _SSIZE_T_DEFINED
#endif
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
inline int test_close_socket(int fd) { return ::close(fd); }
#endif

using namespace elizaos;

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

namespace {
class LocalHttpServer {
public:
    explicit LocalHttpServer(std::unordered_map<std::string, std::string> routes)
        : routes_(std::move(routes)) {
        serverFd_ = ::socket(AF_INET, SOCK_STREAM, 0);
        EXPECT_GE(serverFd_, 0);
        int opt = 1;
        EXPECT_EQ(::setsockopt(serverFd_, SOL_SOCKET, SO_REUSEADDR,
                              reinterpret_cast<const char*>(&opt), sizeof(opt)), 0);

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        addr.sin_port = htons(0);
        EXPECT_EQ(::bind(serverFd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)), 0);
        EXPECT_EQ(::listen(serverFd_, 16), 0);

        socklen_t len = sizeof(addr);
        EXPECT_EQ(::getsockname(serverFd_, reinterpret_cast<sockaddr*>(&addr), &len), 0);
        port_ = ntohs(addr.sin_port);
        running_.store(true);
        worker_ = std::thread(&LocalHttpServer::serveLoop, this);
    }

    ~LocalHttpServer() { stop(); }

    std::string url(const std::string& path) const {
        return "http://127.0.0.1:" + std::to_string(port_) + path;
    }

    struct RecordedRequest {
        std::string method;
        std::string path;
        std::string body;
    };

    std::vector<RecordedRequest> requests() const {
        std::lock_guard<std::mutex> lock(requestsMutex_);
        return requests_;
    }

    std::optional<RecordedRequest> lastRequestForPath(const std::string& lookupPath) const {
        std::lock_guard<std::mutex> lock(requestsMutex_);
        for (auto it = requests_.rbegin(); it != requests_.rend(); ++it) {
            std::string normalized = it->path;
            if (auto query = normalized.find('?'); query != std::string::npos) normalized = normalized.substr(0, query);
            if (normalized == lookupPath) return *it;
        }
        return std::nullopt;
    }

    void stop() {
        if (!running_.exchange(false)) return;
        if (serverFd_ >= 0) {
            ::shutdown(serverFd_, SHUT_RDWR);
            test_close_socket(serverFd_);
            serverFd_ = -1;
        }
        if (worker_.joinable()) worker_.join();
    }

private:
    void serveLoop() {
        while (running_.load()) {
            int client = ::accept(serverFd_, nullptr, nullptr);
            if (client < 0) break;
            handleClient(client);
            ::shutdown(client, SHUT_RDWR);
            test_close_socket(client);
        }
    }

    void handleClient(int client) {
        std::string request;
        char buffer[1024];
        while (request.find("\r\n\r\n") == std::string::npos) {
            ssize_t n = ::recv(client, buffer, sizeof(buffer), 0);
            if (n <= 0) break;
            request.append(buffer, static_cast<std::size_t>(n));
            if (request.size() > 65536) break;
        }

        const auto headerEnd = request.find("\r\n\r\n");
        std::string headers = headerEnd == std::string::npos ? request : request.substr(0, headerEnd + 4);
        std::size_t contentLength = 0;
        {
            std::istringstream headerStream(headers);
            std::string line;
            while (std::getline(headerStream, line)) {
                if (!line.empty() && line.back() == '\r') line.pop_back();
                const std::string prefix = "Content-Length:";
                if (line.rfind(prefix, 0) == 0) {
                    contentLength = static_cast<std::size_t>(std::stoul(line.substr(prefix.size())));
                }
            }
        }
        while (headerEnd != std::string::npos && request.size() < headerEnd + 4 + contentLength) {
            ssize_t n = ::recv(client, buffer, sizeof(buffer), 0);
            if (n <= 0) break;
            request.append(buffer, static_cast<std::size_t>(n));
            if (request.size() > 65536) break;
        }

        std::string path = "/";
        std::istringstream stream(request);
        std::string method;
        stream >> method >> path;
        std::string requestBody;
        if (headerEnd != std::string::npos && request.size() > headerEnd + 4) {
            requestBody = request.substr(headerEnd + 4, contentLength);
        }
        {
            std::lock_guard<std::mutex> lock(requestsMutex_);
            requests_.push_back({method, path, requestBody});
        }

        int code = 200;
        std::string lookupPath = path;
        if (auto query = lookupPath.find('?'); query != std::string::npos) lookupPath = lookupPath.substr(0, query);
        if (auto fragment = lookupPath.find('#'); fragment != std::string::npos) lookupPath = lookupPath.substr(0, fragment);

        auto it = routes_.find(lookupPath);
        std::string body;
        if (it == routes_.end()) {
            code = 404;
            body = "<html><title>Not Found</title><body>missing</body></html>";
        } else {
            body = it->second;
        }

        std::ostringstream response;
        response << "HTTP/1.1 " << code << (code == 200 ? " OK" : " Not Found") << "\r\n"
                 << "Content-Type: text/html; charset=utf-8\r\n"
                 << "Content-Length: " << body.size() << "\r\n"
                 << "Connection: close\r\n\r\n"
                 << body;
        const std::string bytes = response.str();
        const char* cursor = bytes.data();
        std::size_t remaining = bytes.size();
        while (remaining > 0) {
            ssize_t sent = ::send(client, cursor, remaining, MSG_NOSIGNAL);
            if (sent <= 0) break;
            cursor += sent;
            remaining -= static_cast<std::size_t>(sent);
        }
    }

    std::unordered_map<std::string, std::string> routes_;
    mutable std::mutex requestsMutex_;
    std::vector<RecordedRequest> requests_;
    int serverFd_{-1};
    uint16_t port_{0};
    std::atomic<bool> running_{false};
    std::thread worker_;
};
}

TEST_F(AgentBrowserTest, RealHttpNavigationSelectorsFormsArtifactsAndHistory) {
    LocalHttpServer server(std::unordered_map<std::string, std::string>{
        {"/index.html", R"HTML(
            <html>
              <head><title>ElizaOS Browser Fixture</title></head>
              <body>
                <h1 id="headline">AgentBrowser retrieved deterministic HTML</h1>
                <a id="cta" class="primary" href="/second.html">Continue to second page</a>
                <form id="login-form" method="get" action="/submitted.html"><input id="name" name="name" value=""><input id="agree" name="agree" type="checkbox"></form>
                <img src="/asset.png" alt="fixture">
              </body>
            </html>)HTML"},
        {"/second.html", R"HTML(
            <html><head><title>Second Fixture</title></head><body><p class="status">Second page loaded</p></body></html>)HTML"},
        {"/submitted.html", R"HTML(
            <html><head><title>Submitted Fixture</title></head><body><p class="submitted">Submitted page loaded</p></body></html>)HTML"}
    });

    AgentBrowser browser(config_);
    ASSERT_EQ(browser.initialize().result, BrowserActionResult::SUCCESS);

    auto nav = browser.navigateTo(server.url("/index.html"));
    ASSERT_EQ(nav.result, BrowserActionResult::SUCCESS) << nav.message;
    ASSERT_EQ(browser.waitForPageLoad(1).result, BrowserActionResult::SUCCESS);

    auto title = browser.getPageTitle();
    ASSERT_TRUE(title.has_value());
    EXPECT_EQ(*title, "ElizaOS Browser Fixture");

    auto pageText = browser.getPageText();
    ASSERT_TRUE(pageText.has_value());
    EXPECT_NE(pageText->find("AgentBrowser retrieved deterministic HTML"), std::string::npos);

    auto cta = browser.findElement("#cta");
    ASSERT_TRUE(cta.has_value());
    EXPECT_EQ(cta->tag, "a");
    EXPECT_EQ(cta->attributes["href"], "/second.html");
    EXPECT_NE(cta->text.find("Continue"), std::string::npos);
     EXPECT_GE(browser.findElements("a").size(), 1u);
    EXPECT_EQ(browser.waitForElement("#cta", 1).result, BrowserActionResult::SUCCESS);
    EXPECT_EQ(browser.typeText("#name", "Ada").result, BrowserActionResult::SUCCESS);
    EXPECT_EQ(browser.clearText("#name").result, BrowserActionResult::SUCCESS);
    EXPECT_EQ(browser.fillForm({{"#name", "Grace"}}).result, BrowserActionResult::SUCCESS);
    EXPECT_EQ(browser.checkCheckbox("#agree", true).result, BrowserActionResult::SUCCESS);

    auto submitted = browser.submitForm("#login-form");
    ASSERT_EQ(submitted.result, BrowserActionResult::SUCCESS) << submitted.message;
    ASSERT_TRUE(browser.getPageTitle().has_value());
    EXPECT_EQ(*browser.getPageTitle(), "Submitted Fixture");
    auto submittedUrl = browser.evaluateJavaScript("location.href");
    ASSERT_TRUE(submittedUrl.has_value());
    EXPECT_NE(submittedUrl->find("/submitted.html?"), std::string::npos);
    EXPECT_NE(submittedUrl->find("agree=on"), std::string::npos);
    EXPECT_NE(submittedUrl->find("name=Grace"), std::string::npos);

    auto backToForm = browser.goBack();
    ASSERT_EQ(backToForm.result, BrowserActionResult::SUCCESS) << backToForm.message;
    ASSERT_TRUE(browser.getPageTitle().has_value());
    EXPECT_EQ(*browser.getPageTitle(), "ElizaOS Browser Fixture");

    auto clickNav = browser.clickElement("#cta");
    ASSERT_EQ(clickNav.result, BrowserActionResult::SUCCESS) << clickNav.message;
    ASSERT_TRUE(browser.getPageTitle().has_value());
    EXPECT_EQ(*browser.getPageTitle(), "Second Fixture");

    const auto tempDir = std::filesystem::temp_directory_path();
    const auto htmlPath = tempDir / "elizaos_agentbrowser_fixture.html";
    const auto pngPath = tempDir / "elizaos_agentbrowser_fixture.png";
    EXPECT_EQ(browser.savePageHTML(htmlPath.string()).result, BrowserActionResult::SUCCESS);
    EXPECT_TRUE(std::filesystem::exists(htmlPath));
    EXPECT_EQ(browser.captureScreenshot(pngPath.string()).result, BrowserActionResult::SUCCESS);
    EXPECT_TRUE(std::filesystem::exists(pngPath));
    EXPECT_GT(std::filesystem::file_size(pngPath), 0u);
    EXPECT_FALSE(browser.getScreenshotData().empty());

    auto back = browser.goBack();
    ASSERT_EQ(back.result, BrowserActionResult::SUCCESS) << back.message;
    ASSERT_TRUE(browser.getPageTitle().has_value());
    EXPECT_EQ(*browser.getPageTitle(), "ElizaOS Browser Fixture");

    auto second = browser.navigateTo(server.url("/second.html"));
    ASSERT_EQ(second.result, BrowserActionResult::SUCCESS) << second.message;
    ASSERT_TRUE(browser.getPageTitle().has_value());
    EXPECT_EQ(*browser.getPageTitle(), "Second Fixture");

    auto stats = browser.getStatistics();
    EXPECT_GE(stats.pagesVisited, 2);
    EXPECT_GE(stats.elementsClicked, 1);
    EXPECT_GE(stats.formsSubmitted, 1);
    EXPECT_GE(stats.screenshotsTaken, 1);

    std::filesystem::remove(htmlPath);
    std::filesystem::remove(pngPath);
    EXPECT_EQ(browser.shutdown().result, BrowserActionResult::SUCCESS);
}


TEST_F(AgentBrowserTest, SubmitPostFormSendsScopedEncodedBodyAndUpdatesPage) {
    LocalHttpServer server(std::unordered_map<std::string, std::string>{
        {"/post-form.html", R"HTML(
            <html>
              <head><title>POST Form</title></head>
              <body>
                <form id="post-form" method="post" action="/post-submitted.html">
                  <input id="name" name="name" type="text" value="Default Name">
                  <input id="city" name="city" type="text" value="Cape Town">
                  <input id="disabled-field" name="disabled" type="text" value="must-not-submit" disabled>
                  <input id="checked-opt" name="opt" type="checkbox" value="yes" checked>
                  <input id="notify" name="notify" type="checkbox" value="email">
                  <select id="role" name="role"><option value="operator">Operator</option><option value="architect" selected>Architect</option></select>
                  <textarea id="notes" name="notes">default note</textarea>
                </form>
                <form id="other-form" method="post" action="/other.html">
                  <input id="other-field" name="other" type="text" value="leak">
                </form>
              </body>
            </html>)HTML"},
        {"/post-submitted.html", R"HTML(
            <html><head><title>POST Submitted</title></head><body><p>POST form submitted</p></body></html>)HTML"}
    });

    AgentBrowser browser(config_);
    ASSERT_EQ(browser.initialize().result, BrowserActionResult::SUCCESS);
    ASSERT_EQ(browser.navigateTo(server.url("/post-form.html")).result, BrowserActionResult::SUCCESS);

    ASSERT_EQ(browser.fillForm({{"#name", "Ada Lovelace"}, {"#other-field", "outside override"}}).result, BrowserActionResult::SUCCESS);
    ASSERT_EQ(browser.selectOption("#role", "architect").result, BrowserActionResult::SUCCESS);
    ASSERT_EQ(browser.checkCheckbox("#notify", true).result, BrowserActionResult::SUCCESS);

    auto result = browser.submitForm("#post-form");
    ASSERT_EQ(result.result, BrowserActionResult::SUCCESS) << result.message;
    EXPECT_NE(result.message.find("Submitted POST form"), std::string::npos);
    ASSERT_TRUE(browser.getPageTitle().has_value());
    EXPECT_EQ(*browser.getPageTitle(), "POST Submitted");

    auto submittedUrl = browser.evaluateJavaScript("location.href");
    ASSERT_TRUE(submittedUrl.has_value());
    EXPECT_NE(submittedUrl->find("/post-submitted.html"), std::string::npos);
    EXPECT_EQ(submittedUrl->find("?name="), std::string::npos);

    auto request = server.lastRequestForPath("/post-submitted.html");
    ASSERT_TRUE(request.has_value());
    EXPECT_EQ(request->method, "POST");
    EXPECT_NE(request->body.find("name=Ada+Lovelace"), std::string::npos);
    EXPECT_NE(request->body.find("city=Cape+Town"), std::string::npos);
    EXPECT_NE(request->body.find("notes=default+note"), std::string::npos);
    EXPECT_NE(request->body.find("opt=yes"), std::string::npos);
    EXPECT_NE(request->body.find("notify=email"), std::string::npos);
    EXPECT_NE(request->body.find("role=architect"), std::string::npos);
    EXPECT_EQ(request->body.find("disabled=must-not-submit"), std::string::npos);
    EXPECT_EQ(request->body.find("other=leak"), std::string::npos);
    EXPECT_EQ(request->body.find("other=outside+override"), std::string::npos);

    auto stats = browser.getStatistics();
    EXPECT_GE(stats.pagesVisited, 2);
    EXPECT_GE(stats.formsSubmitted, 1);
    EXPECT_EQ(browser.shutdown().result, BrowserActionResult::SUCCESS);
}

TEST_F(AgentBrowserTest, FormControlValidationRejectsWrongControlsAndMissingOptions) {
    LocalHttpServer server(std::unordered_map<std::string, std::string>{
        {"/controls.html", R"HTML(
            <html>
              <head><title>Control Validation</title></head>
              <body>
                <input id="name" name="name" type="text" value="">
                <input id="disabled-name" name="disabled_name" type="text" disabled>
                <select id="role" name="role"><option value="operator">Operator</option><option value="architect">Architect</option></select>
                <input id="agree" name="agree" type="checkbox" value="yes">
                <button id="plain-button">Plain Button</button>
              </body>
            </html>)HTML"}
    });

    AgentBrowser browser(config_);
    ASSERT_EQ(browser.initialize().result, BrowserActionResult::SUCCESS);
    ASSERT_EQ(browser.navigateTo(server.url("/controls.html")).result, BrowserActionResult::SUCCESS);

    EXPECT_EQ(browser.typeText("#name", "Ada").result, BrowserActionResult::SUCCESS);
    EXPECT_EQ(browser.typeText("#role", "architect").result, BrowserActionResult::FAILED);
    EXPECT_EQ(browser.typeText("#plain-button", "click text").result, BrowserActionResult::FAILED);
    EXPECT_EQ(browser.clearText("#disabled-name").result, BrowserActionResult::FAILED);

    EXPECT_EQ(browser.selectOption("#role", "architect").result, BrowserActionResult::SUCCESS);
    EXPECT_EQ(browser.selectOption("#role", "missing").result, BrowserActionResult::FAILED);
    EXPECT_EQ(browser.selectOption("#name", "Ada").result, BrowserActionResult::FAILED);

    EXPECT_EQ(browser.checkCheckbox("#agree", true).result, BrowserActionResult::SUCCESS);
    EXPECT_EQ(browser.checkCheckbox("#name", true).result, BrowserActionResult::FAILED);

    EXPECT_EQ(browser.shutdown().result, BrowserActionResult::SUCCESS);
}

TEST_F(AgentBrowserTest, RealHttpRelativeLinkNavigationAndJavaScriptLimits) {
    LocalHttpServer server(std::unordered_map<std::string, std::string>{
        {"/nested/start.html", R"HTML(
            <html>
              <head><title>Relative Start</title></head>
              <body>
                <a id="relative" href="../second.html?from=relative">Relative traversal</a>
                <button id="plain-button">No navigation</button>
              </body>
            </html>)HTML"},
        {"/second.html", R"HTML(
            <html><head><title>Relative Destination</title></head><body><p>Arrived through a relative URL.</p></body></html>)HTML"}
    });

    AgentBrowser browser(config_);
    ASSERT_EQ(browser.initialize().result, BrowserActionResult::SUCCESS);

    auto nav = browser.navigateTo(server.url("/nested/start.html"));
    ASSERT_EQ(nav.result, BrowserActionResult::SUCCESS) << nav.message;
    ASSERT_TRUE(browser.getPageTitle().has_value());
    EXPECT_EQ(*browser.getPageTitle(), "Relative Start");

    auto unsupported = browser.executeJavaScript("document.body.append('fake')");
    EXPECT_EQ(unsupported.result, BrowserActionResult::FAILED);
    EXPECT_NE(unsupported.message.find("requires a real browser engine"), std::string::npos);
    EXPECT_EQ(browser.evaluateJavaScript("document.title").value_or(""), "Relative Start");

    auto plainClick = browser.clickElement("#plain-button");
    EXPECT_EQ(plainClick.result, BrowserActionResult::SUCCESS) << plainClick.message;
    EXPECT_EQ(browser.getPageTitle().value_or(""), "Relative Start");

    auto linkClick = browser.clickElement("#relative");
    ASSERT_EQ(linkClick.result, BrowserActionResult::SUCCESS) << linkClick.message;
    EXPECT_EQ(browser.getPageTitle().value_or(""), "Relative Destination");
    auto href = browser.evaluateJavaScript("location.href");
    ASSERT_TRUE(href.has_value());
    EXPECT_NE(href->find("/second.html?from=relative"), std::string::npos);

    EXPECT_EQ(browser.shutdown().result, BrowserActionResult::SUCCESS);
}

TEST_F(AgentBrowserTest, SubmitFormIncludesScopedHtmlDefaultsAndUserOverrides) {
    LocalHttpServer server(std::unordered_map<std::string, std::string>{
        {"/forms.html", R"HTML(
            <html>
              <head><title>Scoped Form Defaults</title></head>
              <body>
                <form id="target-form" method="get" action="/multi-submitted.html">
                  <input id="name" name="name" type="text" value="Default Name">
                  <input id="city" name="city" type="text" value="Cape Town">
                  <input id="disabled-field" name="disabled" type="text" value="must-not-submit" disabled>
                  <input id="checked-opt" name="opt" type="checkbox" value="yes" checked>
                  <input id="unchecked-opt" name="skip" type="checkbox" value="no">
                  <select id="role" name="role"><option value="operator">Operator</option><option value="architect" selected>Architect</option></select>
                  <textarea id="notes" name="notes">default note</textarea>
                </form>
                <form id="other-form" method="get" action="/other.html">
                  <input id="other-field" name="other" type="text" value="leak">
                  <input id="outside-check" name="outside" type="checkbox" value="outside-value">
                </form>
              </body>
            </html>)HTML"},
        {"/multi-submitted.html", R"HTML(
            <html><head><title>Scoped Submitted</title></head><body><p>Scoped form submitted</p></body></html>)HTML"}
    });

    AgentBrowser browser(config_);
    ASSERT_EQ(browser.initialize().result, BrowserActionResult::SUCCESS);
    ASSERT_EQ(browser.navigateTo(server.url("/forms.html")).result, BrowserActionResult::SUCCESS);
    ASSERT_EQ(browser.fillForm({{"#name", "Ada Lovelace"}, {"#other-field", "outside override"}}).result, BrowserActionResult::SUCCESS);
    ASSERT_EQ(browser.checkCheckbox("#outside-check", true).result, BrowserActionResult::SUCCESS);

    auto result = browser.submitForm("#target-form");
    ASSERT_EQ(result.result, BrowserActionResult::SUCCESS) << result.message;
    auto submittedUrl = browser.evaluateJavaScript("location.href");
    ASSERT_TRUE(submittedUrl.has_value());
    EXPECT_NE(submittedUrl->find("/multi-submitted.html?"), std::string::npos);
    EXPECT_NE(submittedUrl->find("name=Ada+Lovelace"), std::string::npos);
    EXPECT_NE(submittedUrl->find("city=Cape+Town"), std::string::npos);
    EXPECT_NE(submittedUrl->find("notes=default+note"), std::string::npos);
    EXPECT_NE(submittedUrl->find("opt=yes"), std::string::npos);
    EXPECT_NE(submittedUrl->find("role=architect"), std::string::npos);
    EXPECT_EQ(submittedUrl->find("disabled=must-not-submit"), std::string::npos);
    EXPECT_EQ(submittedUrl->find("skip=no"), std::string::npos);
    EXPECT_EQ(submittedUrl->find("other=leak"), std::string::npos);
    EXPECT_EQ(submittedUrl->find("other=outside+override"), std::string::npos);
    EXPECT_EQ(submittedUrl->find("outside=outside-value"), std::string::npos);

    ASSERT_TRUE(browser.getPageTitle().has_value());
    EXPECT_EQ(*browser.getPageTitle(), "Scoped Submitted");
    EXPECT_EQ(browser.shutdown().result, BrowserActionResult::SUCCESS);
}
