#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_TOOLS_SRC_WEB-BROWSER_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_TOOLS_SRC_WEB-BROWSER_INDEX_H
#include "core.h"
#include "commander.h"
#include "playwright.h"
#include "express.h"
using express = _default;
#include "fs.h"
#include "path.h"

class BrowserManager;
class BrowserServer;

class BrowserManager : public object, public std::enable_shared_from_this<BrowserManager> {
public:
    using std::enable_shared_from_this<BrowserManager>::shared_from_this;
    std::shared_ptr<Browser> browser;

    std::shared_ptr<Page> page;

    string screenshotDir = std::string("/tmp/browser-screenshots");

    boolean isHeadless = process->env->WEB_BROWSER_HEADLESS != std::string("0");

    BrowserManager();
    virtual std::shared_ptr<Promise<void>> init();
    virtual std::shared_ptr<Promise<void>> openSite(string url);
    virtual std::shared_ptr<Promise<void>> closeSite();
    virtual std::shared_ptr<Promise<void>> screenshot(string filename = undefined);
    template <typename P2>
    std::shared_ptr<Promise<void>> click(double x, double y, P2 button = std::string("left"));
    virtual std::shared_ptr<Promise<void>> type(string text);
    virtual std::shared_ptr<Promise<void>> scroll(double deltaX, double deltaY);
    virtual std::shared_ptr<Promise<void>> executeScript(string script);
    virtual std::shared_ptr<Promise<void>> getConsoleOutput();
};

class BrowserServer : public object, public std::enable_shared_from_this<BrowserServer> {
public:
    using std::enable_shared_from_this<BrowserServer>::shared_from_this;
    std::shared_ptr<express::Application> app;

    std::shared_ptr<BrowserManager> browserManager;

    double port;

    BrowserServer(double port = 8009);
    virtual void setupRoutes();
    virtual void start();
};

template <typename P2>
std::shared_ptr<Promise<void>> BrowserManager::click(double x, double y, P2 button)
{
    if (!this->page) {
        console->error(std::string("No page open"));
        return std::shared_ptr<Promise<void>>();
    }
    std::async([=]() { this->page->mouse->click(x, y, object{
        object::pair{std::string("button"), std::string("button")}
    }); });
    console->log(std::string("Clicked at (") + x + std::string(", ") + y + std::string(") with ") + button + std::string(" button"));
}

#endif
