#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tools/src/web-browser/index.h"

BrowserManager::BrowserManager() {
    if (!fs::existsSync(this->screenshotDir)) {
        fs::mkdirSync(this->screenshotDir, object{
            object::pair{std:("recursive"), true}
        });
    }
}

std::shared_ptr<Promise<void>> BrowserManager::init()
{
    if (!this->browser) {
        this->browser = std::async([=]() { chromium->launch(object{
            object::pair{std:("headless"), this->isHeadless}, 
            object::pair{std:("args"), array<string>{ std:("--no-sandbox"), std:("--disable-setuid-sandbox") }}
        }); });
        auto context = std::async([=]() { this->browser->newContext(object{
            object::pair{std:("viewport"), object{
                object::pair{std:("width"), 1024}, 
                object::pair{std:("height"), 768}
            }}
        }); });
        this->page = std::async([=]() { context->newPage(); });
    }
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> BrowserManager::openSite(string url)
{
    std::async([=]() { this->init(); });
    if (!this->page) throw any(std::make_shared<Error>(std:("Browser not initialized")));
    if (fs::existsSync(url)) {
        url = std:("file://") + path->resolve(url) + string_empty;
    }
    std::async([=]() { this->page->goto(url, object{
        object::pair{std:("waitUntil"), std:("load")}
    }); });
    console->log(std:("Navigated to ") + url + string_empty);
    console->log(std:("Page title: ") + std::async([=]() { this->page->title(); }) + string_empty);
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> BrowserManager::closeSite()
{
    if (this->browser) {
        std::async([=]() { this->browser->close(); });
        this->browser = undefined;
        this->page = undefined;
        console->log(std:("Browser closed"));
    }
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> BrowserManager::screenshot(string filename)
{
    if (!this->page) {
        console->error(std:("No page open"));
        return std::shared_ptr<Promise<void>>();
    }
    auto screenshotPath = path->join(this->screenshotDir, OR((filename), (std:("screenshot-") + Date->now() + std:(".png"))));
    std::async([=]() { this->page->screenshot(object{
        object::pair{std:("path"), screenshotPath}, 
        object::pair{std:("fullPage"), true}
    }); });
    auto imageBuffer = fs::readFileSync(screenshotPath);
    auto base64 = imageBuffer->toString(std:("base64"));
    console->log(std:("![Screenshot](data:image/png;base64,") + base64 + std:(")"));
}

std::shared_ptr<Promise<void>> BrowserManager::type(string text)
{
    if (!this->page) {
        console->error(std:("No page open"));
        return std::shared_ptr<Promise<void>>();
    }
    std::async([=]() { this->page->keyboard->type(text); });
    console->log(std:("Typed: ") + text + string_empty);
}

std::shared_ptr<Promise<void>> BrowserManager::scroll(double deltaX, double deltaY)
{
    if (!this->page) {
        console->error(std:("No page open"));
        return std::shared_ptr<Promise<void>>();
    }
    std::async([=]() { this->page->mouse->wheel(deltaX, deltaY); });
    console->log(std:("Scrolled by (") + deltaX + std:(", ") + deltaY + std:(")"));
}

std::shared_ptr<Promise<void>> BrowserManager::executeScript(string script)
{
    if (!this->page) {
        console->error(std:("No page open"));
        return std::shared_ptr<Promise<void>>();
    }
    auto result = std::async([=]() { this->page->evaluate(script); });
    console->log(std:("Script executed. Result:"), result);
}

std::shared_ptr<Promise<void>> BrowserManager::getConsoleOutput()
{
    if (!this->page) {
        console->error(std:("No page open"));
        return std::shared_ptr<Promise<void>>();
    }
    this->page->on(std:("console"), [=](auto msg) mutable
    {
        console->log(std:("[Console ") + msg["type"]() + std:("]: ") + msg["text"]() + string_empty);
    }
    );
    console->log(std:("Console output listener activated"));
}

BrowserServer::BrowserServer(double port) {
    this->port = port;
    this->app = express();
    this->browserManager = std::make_shared<BrowserManager>();
    this->setupRoutes();
}

void BrowserServer::setupRoutes()
{
    this->app->use(express->json());
    this->app->post(std:("/goto"), [=](auto req, auto res) mutable
    {
        try
        {
            std::async([=]() { this->browserManager->openSite(req["body"]["url"]); });
            res["json"](object{
                object::pair{std:("status"), std:("success")}
            });
        }
        catch (const any& error)
        {
            res["status"](500)["json"](object{
                object::pair{std:("status"), std:("error")}, 
                object::pair{std:("message"), String(error)}
            });
        }
    }
    );
    this->app->post(std:("/close"), [=](auto _req, auto res) mutable
    {
        try
        {
            std::async([=]() { this->browserManager->closeSite(); });
            res["json"](object{
                object::pair{std:("status"), std:("success")}
            });
        }
        catch (const any& error)
        {
            res["status"](500)["json"](object{
                object::pair{std:("status"), std:("error")}, 
                object::pair{std:("message"), String(error)}
            });
        }
    }
    );
    this->app->get(std:("/screenshot"), [=](auto _req, auto res) mutable
    {
        try
        {
            std::async([=]() { this->browserManager->screenshot(); });
            res["json"](object{
                object::pair{std:("status"), std:("success")}
            });
        }
        catch (const any& error)
        {
            res["status"](500)["json"](object{
                object::pair{std:("status"), std:("error")}, 
                object::pair{std:("message"), String(error)}
            });
        }
    }
    );
    this->app->post(std:("/click"), [=](auto req, auto res) mutable
    {
        try
        {
            std::async([=]() { this->browserManager->click(req["body"]["x"], req["body"]["y"], req["body"]["button"]); });
            res["json"](object{
                object::pair{std:("status"), std:("success")}
            });
        }
        catch (const any& error)
        {
            res["status"](500)["json"](object{
                object::pair{std:("status"), std:("error")}, 
                object::pair{std:("message"), String(error)}
            });
        }
    }
    );
    this->app->post(std:("/type"), [=](auto req, auto res) mutable
    {
        try
        {
            std::async([=]() { this->browserManager->type(req["body"]["text"]); });
            res["json"](object{
                object::pair{std:("status"), std:("success")}
            });
        }
        catch (const any& error)
        {
            res["status"](500)["json"](object{
                object::pair{std:("status"), std:("error")}, 
                object::pair{std:("message"), String(error)}
            });
        }
    }
    );
}

void BrowserServer::start()
{
    this->app->listen(this->port, [=]() mutable
    {
        console->log(std:("Browser server running on port ") + this->port + string_empty);
    }
    );
}


void Main(void)
{
    if (require->main == module) {
        program->name(std:("web-browser"))->description(std:("Web browser automation tool"))->version(std:("1.0.0"));
        program->command(std:("server"))->description(std:("Start the browser server"))->option(std:("-p, --port <port>"), std:("Server port"), std:("8009"))->action([=](auto options) mutable
        {
            auto server = std::make_shared<BrowserServer>(parseInt(options["port"]));
            server->start();
        }
        );
        program->command(std:("open <url>"))->description(std:("Open a URL"))->action([=](auto url) mutable
        {
            auto manager = std::make_shared<BrowserManager>();
            std::async([=]() { manager->openSite(url); });
            std::async([=]() { manager->closeSite(); });
        }
        );
        program->command(std:("screenshot <url>"))->description(std:("Take a screenshot of a URL"))->action([=](auto url) mutable
        {
            auto manager = std::make_shared<BrowserManager>();
            std::async([=]() { manager->openSite(url); });
            std::async([=]() { manager->screenshot(); });
            std::async([=]() { manager->closeSite(); });
        }
        );
        program->parse(process->argv);
    }
}

MAIN
