#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-starter/src/__tests__/cypress/e2e/user-workflow.cy.h"

void Main(void)
{
    describe(std::string("Complete User Workflow E2E Tests"), [=]() mutable
    {
        describe(std::string("New User Onboarding"), [=]() mutable
        {
            it(std::string("should complete the full onboarding flow"), [=]() mutable
            {
                cy->visit(std::string("/"));
                cy->get(std::string("body"))->then([=](auto $body) mutable
                {
                    if (OR(($body["text"]()["includes"](std::string("Welcome"))), ($body["text"]()["includes"](std::string("Get Started"))))) {
                        cy->contains((new RegExp(std::string("get started|start|begin"))))->first()->click();
                    }
                }
                );
                cy->get(std::string("a[href*="agent"], button:contains("agent"), a[href*="chat"], button:contains("chat")"))->first()->click(object{
                    object::pair{std::string("force"), true}
                });
                cy->wait(1000);
                cy->get(std::string("input[type="text"], textarea, [contenteditable="true"]"))->filter(std::string(":visible"))->first()->type(std::string("Hello, I am a new user{enter}"));
                cy->get(std::string("[data-testid*="message"], [class*="message"], [role="article"]"), object{
                    object::pair{std::string("timeout"), 15000}
                })->should(std::string("have.length.greaterThan"), 0);
                cy->get(std::string("input[type="text"], textarea, [contenteditable="true"]"))->filter(std::string(":visible"))->first()->type(std::string("What can you help me with?{enter}"));
                cy->get(std::string("[data-testid*="message"], [class*="message"], [role="article"]"), object{
                    object::pair{std::string("timeout"), 15000}
                })->should(std::string("have.length.greaterThan"), 1);
            }
            );
        }
        );
        describe(std::string("Agent Configuration Workflow"), [=]() mutable
        {
            it(std::string("should configure and interact with an agent"), [=]() mutable
            {
                cy->visit(std::string("/"));
                cy->get(std::string("a[href*="settings"], button:contains("settings"), a[href*="config"], button:contains("config")"))->first()->then([=](auto $elem) mutable
                {
                    if ($elem["length"]) {
                        cy->wrap($elem)->click(object{
                            object::pair{std::string("force"), true}
                        });
                        cy->contains((new RegExp(std::string("agent|model|personality"))))->should(std::string("be.visible"));
                        cy->get(std::string("a[href*="chat"], button:contains("chat")"))->first()->click(object{
                            object::pair{std::string("force"), true}
                        });
                    }
                }
                );
                auto queries = array<string>{ std::string("What is your name?"), std::string("Tell me a joke"), std::string("What is 2 + 2?") };
                queries->forEach([=](auto query, auto index) mutable
                {
                    cy->get(std::string("input[type="text"], textarea, [contenteditable="true"]"))->filter(std::string(":visible"))->first()->type(string_empty + query + std::string("{enter}"));
                    cy->wait(2000);
                    cy->get(std::string("[data-testid*="message"], [class*="message"], [role="article"]"))->should(std::string("have.length.greaterThan"), index * 2);
                }
                );
            }
            );
        }
        );
        describe(std::string("Multi-Session Workflow"), [=]() mutable
        {
            it(std::string("should maintain state across page refreshes"), [=]() mutable
            {
                cy->visit(std::string("/"));
                cy->get(std::string("a[href*="chat"], a[href*="agent"], button:contains("chat"), button:contains("agent")"))->first()->click(object{
                    object::pair{std::string("force"), true}
                });
                auto testMessage = std::string("Remember this message for testing");
                cy->get(std::string("input[type="text"], textarea, [contenteditable="true"]"))->filter(std::string(":visible"))->first()->type(string_empty + testMessage + std::string("{enter}"));
                cy->wait(3000);
                cy->reload();
                cy->contains(testMessage, object{
                    object::pair{std::string("timeout"), 10000}
                })->should(std::string("be.visible"));
            }
            );
            it(std::string("should handle multiple chat sessions"), [=]() mutable
            {
                cy->visit(std::string("/"));
                cy->get(std::string("a[href*="chat"], button:contains("chat")"))->first()->click(object{
                    object::pair{std::string("force"), true}
                });
                cy->get(std::string("input[type="text"], textarea, [contenteditable="true"]"))->filter(std::string(":visible"))->first()->type(std::string("First session message{enter}"));
                cy->wait(2000);
                cy->get(std::string("button"))->filter(std::string(":contains("New"), :contains("new"), [aria-label*="new"]"))->first()->then([=](auto $btn) mutable
                {
                    if ($btn["length"]) {
                        cy->wrap($btn)->click();
                        cy->get(std::string("input[type="text"], textarea, [contenteditable="true"]"))->filter(std::string(":visible"))->first()->type(std::string("Second session message{enter}"));
                        cy->contains(std::string("Second session message"))->should(std::string("be.visible"));
                        cy->contains(std::string("First session message"))->should(std::string("not.be.visible"));
                    }
                }
                );
            }
            );
        }
        );
        describe(std::string("Error Recovery Workflow"), [=]() mutable
        {
            it(std::string("should recover from errors and continue working"), [=]() mutable
            {
                cy->visit(std::string("/"));
                cy->intercept(std::string("*"), object{
                    object::pair{std::string("forceNetworkError"), true}
                })->as(std::string("offlineMode"));
                cy->get(std::string("a[href*="chat"], button:contains("chat")"))->first()->click(object{
                    object::pair{std::string("force"), true}
                });
                cy->get(std::string("input[type="text"], textarea, [contenteditable="true"]"))->filter(std::string(":visible"))->first()->type(std::string("Offline message{enter}"));
                cy->contains((new RegExp(std::string("offline|error|connection|failed"))), object{
                    object::pair{std::string("timeout"), 5000}
                })->should(std::string("be.visible"));
                cy->intercept(std::string("*"), [=](auto req) mutable
                {
                    req["_continue"]();
                }
                )->as(std::string("onlineMode"));
                cy->get(std::string("input[type="text"], textarea, [contenteditable="true"]"))->filter(std::string(":visible"))->first()->type(std::string("Online message{enter}"));
                cy->contains(std::string("Online message"), object{
                    object::pair{std::string("timeout"), 10000}
                })->should(std::string("be.visible"));
            }
            );
        }
        );
        describe(std::string("Performance Workflow"), [=]() mutable
        {
            it(std::string("should handle rapid message sending"), [=]() mutable
            {
                cy->visit(std::string("/"));
                cy->get(std::string("a[href*="chat"], button:contains("chat")"))->first()->click(object{
                    object::pair{std::string("force"), true}
                });
                for (auto i = 0; i < 5; i++)
                {
                    cy->get(std::string("input[type="text"], textarea, [contenteditable="true"]"))->filter(std::string(":visible"))->first()->type(std::string("Rapid message ") + i + std::string("{enter}"));
                    cy->wait(100);
                }
                for (auto i = 0; i < 5; i++)
                {
                    cy->contains(std::string("Rapid message ") + i + string_empty)->should(std::string("be.visible"));
                }
                cy->get(std::string("input[type="text"], textarea, [contenteditable="true"]"))->filter(std::string(":visible"))->first()->should(std::string("not.be.disabled"));
            }
            );
        }
        );
    }
    );
}

MAIN
