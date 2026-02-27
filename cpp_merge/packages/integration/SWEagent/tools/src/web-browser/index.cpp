#include "index.hpp"
#include <string>

BrowserManager::BrowserManager() {
    if (!fs::existsSync(this->screenshotDir)) {
        fs::mkdirSync(this->screenshotDir, object{
            object::pair{std::string("recursive"), true}
        });
    }
}

std::shared_ptr<Promise<void>> BrowserManager::init()
{
    if (!this->browser) {
        this->browser = std::async([=]() { chromium->launch(object{
            object::pair{std::string("headless"), this->isHeadless}, 
            object::pair{std::string("args"), array<string>{ std::string("--no-sandbox"), std::string("--disable-setuid-sandbox") }}
        }); });
        auto context = std::async([=]() { this->browser->newContext(object{
            object::pair{std::string("viewport"), object{
                object::pair{std::string("width"), 1024}, 
                object::pair{std::string("height"), 768}
            }}
        }); });
        this->page = std::async([=]() { context->newPage(); });
    }
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> BrowserManager::openSite(string url)
{
    std::async([=]() { this->init(); });
    if (!this->page) throw any(std::make_shared<Error>(std::string("Browser not initialized")));
    if (fs::existsSync(url)) {
        url = std::string("file://") + path->resolve(url) + string_empty;
    }
    std::async([=]() { this->page->goto(url, object{
        object::pair{std::string("waitUntil"), std::string("load")}
    }); });
    console->log(std::string("Navigated to ") + url + string_empty);
    console->log(std::string("Page title: ") + std::async([=]() { this->page->title(); }) + string_empty);
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> BrowserManager::closeSite()
{
    if (this->browser) {
        std::async([=]() { this->browser->close(); });
        this->browser = std::nullopt;
        this->page = std::nullopt;
        console->log(std::string("Browser closed"));
    }
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> BrowserManager::screenshot(string filename)
{
    if (!this->page) {
        console->error(std::string("No page open"));
        return std::shared_ptr<Promise<void>>();
    }
    auto screenshotPath = path->join(this->screenshotDir, OR((filename), (std::string("screenshot-") + Date->now() + std::string(".png"))));
    std::async([=]() { this->page->screenshot(object{
        object::pair{std::string("path"), screenshotPath}, 
        object::pair{std::string("fullPage"), true}
    }); });
    auto imageBuffer = fs::readFileSync(screenshotPath);
    auto base64 = imageBuffer->toString(std::string("base64"));
    console->log(std::string("![Screenshot](data:image/png;base64,") + base64 + std::string(")"));
}

std::shared_ptr<Promise<void>> BrowserManager::type(string text)
{
    if (!this->page) {
        console->error(std::string("No page open"));
        return std::shared_ptr<Promise<void>>();
    }
    std::async([=]() { this->page->keyboard->type(text); });
    console->log(std::string("Typed: ") + text + string_empty);
}

std::shared_ptr<Promise<void>> BrowserManager::scroll(double deltaX, double deltaY)
{
    if (!this->page) {
        console->error(std::string("No page open"));
        return std::shared_ptr<Promise<void>>();
    }
    std::async([=]() { this->page->mouse->wheel(deltaX, deltaY); });
    console->log(std::string("Scrolled by (") + deltaX + std::string(", ") + deltaY + std::string(")"));
}

std::shared_ptr<Promise<void>> BrowserManager::executeScript(string script)
{
    if (!this->page) {
        console->error(std::string("No page open"));
        return std::shared_ptr<Promise<void>>();
    }
    auto result = std::async([=]() { this->page->evaluate(script); });
    console->log(std::string("Script executed. Result:"), result);
}

std::shared_ptr<Promise<void>> BrowserManager::getConsoleOutput()
{
    if (!this->page) {
        console->error(std::string("No page open"));
        return std::shared_ptr<Promise<void>>();
    }
    this->page->on(std::string("console"), [=](auto msg) mutable
    {
        console->log(std::string("[Console ") + msg["type"]() + std::string("]: ") + msg["text"]() + string_empty);
    }
    );
    console->log(std::string("Console output listener activated"));
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
    this->app->post(std::string("/goto"), [=](auto req, auto res) mutable
    {
        try
        {
            std::async([=]() { this->browserManager->openSite(req["body"]["url"]); });
            res["json"](object{
                object::pair{std::string("status"), std::string("success")}
            });
        }
        catch (const any& error)
        {
            res["status"](500)["json"](object{
                object::pair{std::string("status"), std::string("error")}, 
                object::pair{std::string("message"), String(error)}
            });
        }
    }
    );
    this->app->post(std::string("/close"), [=](auto _req, auto res) mutable
    {
        try
        {
            std::async([=]() { this->browserManager->closeSite(); });
            res["json"](object{
                object::pair{std::string("status"), std::string("success")}
            });
        }
        catch (const any& error)
        {
            res["status"](500)["json"](object{
                object::pair{std::string("status"), std::string("error")}, 
                object::pair{std::string("message"), String(error)}
            });
        }
    }
    );
    this->app->get(std::string("/screenshot"), [=](auto _req, auto res) mutable
    {
        try
        {
            std::async([=]() { this->browserManager->screenshot(); });
            res["json"](object{
                object::pair{std::string("status"), std::string("success")}
            });
        }
        catch (const any& error)
        {
            res["status"](500)["json"](object{
                object::pair{std::string("status"), std::string("error")}, 
                object::pair{std::string("message"), String(error)}
            });
        }
    }
    );
    this->app->post(std::string("/click"), [=](auto req, auto res) mutable
    {
        try
        {
            std::async([=]() { this->browserManager->click(req["body"]["x"], req["body"]["y"], req["body"]["button"]); });
            res["json"](object{
                object::pair{std::string("status"), std::string("success")}
            });
        }
        catch (const any& error)
        {
            res["status"](500)["json"](object{
                object::pair{std::string("status"), std::string("error")}, 
                object::pair{std::string("message"), String(error)}
            });
        }
    }
    );
    this->app->post(std::string("/type"), [=](auto req, auto res) mutable
    {
        try
        {
            std::async([=]() { this->browserManager->type(req["body"]["text"]); });
            res["json"](object{
                object::pair{std::string("status"), std::string("success")}
            });
        }
        catch (const any& error)
        {
            res["status"](500)["json"](object{
                object::pair{std::string("status"), std::string("error")}, 
                object::pair{std::string("message"), String(error)}
            });
        }
    }
    );
}

void BrowserServer::start()
{
    this->app->listen(this->port, [=]() mutable
    {
        console->log(std::string("Browser server running on port ") + this->port + string_empty);
    }
    );
}


void Main(void)
{
    if (require->main == module) {
        program->name(std::string("web-browser"))->description(std::string("Web browser automation tool"))->version(std::string("1.0.0"));
        program->command(std::string("server"))->description(std::string("Start the browser server"))->option(std::string("-p, --port <port>"), std::string("Server port"), std::string("8009"))->action([=](auto options) mutable
        {
            auto server = std::make_shared<BrowserServer>(parseInt(options["port"]));
            server->start();
        }
        );
        program->command(std::string("open <url>"))->description(std::string("Open a URL"))->action([=](auto url) mutable
        {
            auto manager = std::make_shared<BrowserManager>();
            std::async([=]() { manager->openSite(url); });
            std::async([=]() { manager->closeSite(); });
        }
        );
        program->command(std::string("screenshot <url>"))->description(std::string("Take a screenshot of a URL"))->action([=](auto url) mutable
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
