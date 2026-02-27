#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-starter/src/__tests__/cypress/support/e2e.h"

namespace global {
    namespace Cypress {
    }
}

void Main(void)
{
    Cypress->on(std:("uncaught:exception"), [=](auto err, auto runnable) mutable
    {
        console->error(std:("Uncaught exception:"), err);
        return false;
    }
    );
    Cypress->Commands->add(std:("login"), [=](auto username = std:("testuser"), auto password = std:("testpass")) mutable
    {
        cy->get(std:("body"))->then([=](auto $body) mutable
        {
            if ($body["find"](std:("[data-testid="login"], form[name="login"], input[name="username"]"))["length"]) {
                cy->get(std:("input[name="username"], input[type="email"]"))->first()->type(username);
                cy->get(std:("input[name="password"], input[type="password"]"))->first()->type(password);
                cy->get(std:("button[type="submit"], button:contains("Login")"))->first()->click();
                cy->wait(1000);
            }
        }
        );
    }
    );
    Cypress->Commands->add(std:("waitForApp"), [=]() mutable
    {
        cy->get(std:("[data-testid="loading"], .loading, .spinner"), object{
            object::pair{std:("timeout"), 10000}
        })->should(std:("not.exist"));
        cy->get(std:("#root, #app, [data-testid="app"]"))->should(std:("be.visible"));
        cy->wait(500);
    }
    );
    Cypress->Commands->add(std:("navigateToAgent"), [=](auto agentId = undefined) mutable
    {
        if (agentId) {
            cy->visit(std:("/agent/") + agentId + string_empty);
        } else {
            cy->get(std:("a[href*="agent"], button:contains("agent")"))->first()->click(object{
                object::pair{std:("force"), true}
            });
        }
        cy->waitForApp();
    }
    );
    Cypress->Commands->add(std:("sendChatMessage"), [=](auto message) mutable
    {
        cy->get(std:("input[type="text"], textarea, [contenteditable="true"]"))->filter(std:(":visible"))->first()->clear()->type(message);
        cy->get(std:("button"))->filter(std:(":contains("Send"), [aria-label*="send"]"))->first()->click();
        cy->contains(message, object{
            object::pair{std:("timeout"), 5000}
        })->should(std:("be.visible"));
        cy->get(std:("[data-testid*="agent"], [class*="agent"], [data-sender="agent"]"), object{
            object::pair{std:("timeout"), 15000}
        })->should(std:("exist"));
    }
    );
    Cypress->Commands->add(std:("clearAppData"), [=]() mutable
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
    Cypress->on(std:("fail"), [=](auto error, auto runnable) mutable
    {
        cy->screenshot(std:("failed-") + runnable["parent"]["title"] + std:("-") + runnable["title"] + string_empty, object{
            object::pair{std:("capture"), std:("runner")}
        });
        throw any(error);
    }
    );
}

MAIN
