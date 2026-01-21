#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-starter/src/__tests__/cypress/support/e2e.h"

namespace global {
    namespace Cypress {
    }
}

void Main(void)
{
    Cypress->on(std::string("uncaught:exception"), [=](auto err, auto runnable) mutable
    {
        console->error(std::string("Uncaught exception:"), err);
        return false;
    }
    );
    Cypress->Commands->add(std::string("login"), [=](auto username = std::string("testuser"), auto password = std::string("testpass")) mutable
    {
        cy->get(std::string("body"))->then([=](auto $body) mutable
        {
            if ($body["find"](std::string("[data-testid="login"], form[name="login"], input[name="username"]"))["length"]) {
                cy->get(std::string("input[name="username"], input[type="email"]"))->first()->type(username);
                cy->get(std::string("input[name="password"], input[type="password"]"))->first()->type(password);
                cy->get(std::string("button[type="submit"], button:contains("Login")"))->first()->click();
                cy->wait(1000);
            }
        }
        );
    }
    );
    Cypress->Commands->add(std::string("waitForApp"), [=]() mutable
    {
        cy->get(std::string("[data-testid="loading"], .loading, .spinner"), object{
            object::pair{std::string("timeout"), 10000}
        })->should(std::string("not.exist"));
        cy->get(std::string("#root, #app, [data-testid="app"]"))->should(std::string("be.visible"));
        cy->wait(500);
    }
    );
    Cypress->Commands->add(std::string("navigateToAgent"), [=](auto agentId = undefined) mutable
    {
        if (agentId) {
            cy->visit(std::string("/agent/") + agentId + string_empty);
        } else {
            cy->get(std::string("a[href*="agent"], button:contains("agent")"))->first()->click(object{
                object::pair{std::string("force"), true}
            });
        }
        cy->waitForApp();
    }
    );
    Cypress->Commands->add(std::string("sendChatMessage"), [=](auto message) mutable
    {
        cy->get(std::string("input[type="text"], textarea, [contenteditable="true"]"))->filter(std::string(":visible"))->first()->clear()->type(message);
        cy->get(std::string("button"))->filter(std::string(":contains("Send"), [aria-label*="send"]"))->first()->click();
        cy->contains(message, object{
            object::pair{std::string("timeout"), 5000}
        })->should(std::string("be.visible"));
        cy->get(std::string("[data-testid*="agent"], [class*="agent"], [data-sender="agent"]"), object{
            object::pair{std::string("timeout"), 15000}
        })->should(std::string("exist"));
    }
    );
    Cypress->Commands->add(std::string("clearAppData"), [=]() mutable
    {
        cy->window()->then([=](auto win) mutable
        {
            (as<any>(win))["localStorage"]["clear"]();
            (as<any>(win))["sessionStorage"]["clear"]();
            cy->clearCookies();
        }
        );
    }
    );
    beforeEach([=]() mutable
    {
        cy->viewport(1280, 720);
    }
    );
    Cypress->on(std::string("fail"), [=](auto error, auto runnable) mutable
    {
        cy->screenshot(std::string("failed-") + runnable["parent"]["title"] + std::string("-") + runnable["title"] + string_empty, object{
            object::pair{std::string("capture"), std::string("runner")}
        });
        throw any(error);
    }
    );
}

MAIN
