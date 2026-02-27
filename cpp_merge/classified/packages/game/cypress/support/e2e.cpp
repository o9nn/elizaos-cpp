#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/game/cypress/support/e2e.h"

namespace global {
    namespace Cypress {
    }
}

void Main(void)
{
    Cypress->config(std:("defaultCommandTimeout"), 15000);
    Cypress->config(std:("requestTimeout"), 20000);
    Cypress->config(std:("responseTimeout"), 20000);
    Cypress->config(std:("pageLoadTimeout"), 30000);
    Cypress->on(std:("uncaught:exception"), [=](auto err, auto _runnable) mutable
    {
        if (err["message"]["includes"](std:("ResizeObserver loop limit exceeded"))) {
            return false;
        }
        if (err["message"]["includes"](std:("Non-Error promise rejection captured"))) {
            return false;
        }
        if (err["message"]["includes"](std:("Script error"))) {
            return false;
        }
        console->error(std:("Uncaught exception:"), err);
        return false;
    }
    );
    beforeEach([=]() mutable
    {
        shared authToken = Cypress->env(std:("ELIZA_SERVER_AUTH_TOKEN"));
        if (authToken) {
            cy->intercept(std:("**"), [=](auto req) mutable
            {
                if (AND((req["url"]["includes"](std:("/api/"))), (req["url"]["includes"](std:("localhost:7777"))))) {
                    req["headers"][std:("X-API-KEY")] = authToken;
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
            win["localStorage"]["setItem"](std:("testingMode"), std:("true"));
            win["localStorage"]["setItem"](std:("cypressTest"), std:("true"));
        }
        );
        cy->intercept(std:("**/*"), [=](auto req) mutable
        {
            console->log(std:("Network request: ") + req["method"] + std:(" ") + req["url"] + string_empty);
            req["_continue"]();
        }
        );
    }
    );
    afterEach([=]() mutable
    {
        cy->window()->then([=](auto _win) mutable
        {
            console->log(std:("Test completed at:"), ((std::make_shared<Date>()))->toISOString());
        }
        );
        cy->screenshot(std:("test-completed"));
    }
    );
    chai->use([=](auto chai, auto utils) mutable
    {
        utils->addMethod(chai->Assertion->prototype, std:("containOneOf"), [=](any list) mutable
        {
            shared obj = utils->flag(shared_from_this(), std:("object"));
            auto found = list["some"]([=](auto item) mutable
            {
                return obj["includes"](item);
            }
            );
            this["assert"](found, std:("expected "") + obj + std:("" to contain one of [") + list["join"](std:(", ")) + std:("]"), std:("expected "") + obj + std:("" not to contain any of [") + list["join"](std:(", ")) + std:("]"));
        }
        );
    }
    );
}

MAIN
