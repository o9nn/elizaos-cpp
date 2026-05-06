#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/game/cypress/support/e2e.h"

namespace global {
    namespace Cypress {
    }
}

void Main(void)
{
    Cypress->config(std::string("defaultCommandTimeout"), 15000);
    Cypress->config(std::string("requestTimeout"), 20000);
    Cypress->config(std::string("responseTimeout"), 20000);
    Cypress->config(std::string("pageLoadTimeout"), 30000);
    Cypress->on(std::string("uncaught:exception"), [=](auto err, auto _runnable) mutable
    {
        if (err["message"]["includes"](std::string("ResizeObserver loop limit exceeded"))) {
            return false;
        }
        if (err["message"]["includes"](std::string("Non-Error promise rejection captured"))) {
            return false;
        }
        if (err["message"]["includes"](std::string("Script error"))) {
            return false;
        }
        console->error(std::string("Uncaught exception:"), err);
        return false;
    }
    );
    beforeEach([=]() mutable
    {
        shared authToken = Cypress->env(std::string("ELIZA_SERVER_AUTH_TOKEN"));
        if (authToken) {
            cy->intercept(std::string("**"), [=](auto req) mutable
            {
                if (AND((req["url"]["includes"](std::string("/api/"))), (req["url"]["includes"](std::string("localhost:7777"))))) {
                    req["headers"][std::string("X-API-KEY")] = authToken;
                }
            }
            );
        }
    }
    );
    beforeEach([=]() mutable
    {
        cy->viewport(1280, 720);
        cy->clearLocalStorage();
        cy->clearCookies();
        cy->window()->then([=](auto win) mutable
        {
            win["localStorage"]["setItem"](std::string("testingMode"), std::string("true"));
            win["localStorage"]["setItem"](std::string("cypressTest"), std::string("true"));
        }
        );
        cy->intercept(std::string("**/*"), [=](auto req) mutable
        {
            console->log(std::string("Network request: ") + req["method"] + std::string(" ") + req["url"] + string_empty);
            req["_continue"]();
        }
        );
    }
    );
    afterEach([=]() mutable
    {
        cy->window()->then([=](auto _win) mutable
        {
            console->log(std::string("Test completed at:"), ((std::make_shared<Date>()))->toISOString());
        }
        );
        cy->screenshot(std::string("test-completed"));
    }
    );
    chai->use([=](auto chai, auto utils) mutable
    {
        utils->addMethod(chai->Assertion->prototype, std::string("containOneOf"), [=](any list) mutable
        {
            shared obj = utils->flag(shared_from_this(), std::string("object"));
            auto found = list["some"]([=](auto item) mutable
            {
                return obj["includes"](item);
            }
            );
            this["assert"](found, std::string("expected "") + obj + std::string("" to contain one of [") + list["join"](std::string(", ")) + std::string("]"), std::string("expected "") + obj + std::string("" not to contain any of [") + list["join"](std::string(", ")) + std::string("]"));
        }
        );
    }
    );
}

MAIN
