#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-starter/src/__tests__/cypress/e2e/user-workflow.cy.h"

void Main(void)
{
    describe(std:("Complete User Workflow E2E Tests"), [=]() mutable
    {
        describe(std:("New User Onboarding"), [=]() mutable
        {
            it(std:("should complete the full onboarding flow"), [=]() mutable
            {
                cy->visit(std:("/"));
                cy->get(std:("body"))->then([=](auto $body) mutable
                {
                    if (OR(($body["text"]()["includes"](std:("Welcome"))), ($body["text"]()["includes"](std:("Get Started"))))) {
                        cy->contains((new RegExp(std:("get started|start|begin"))))->first()->click();
                    }
                }
                );
                cy->get(std:("a[href*="agent"], button:contains("agent"), a[href*="chat"], button:contains("chat")"))->first()->click(object{
                    object::pair{std:("force"), true}
                });
                cy->wait(1000);
                cy->get(std:("input[type="text"], textarea, [contenteditable="true"]"))->filter(std:(":visible"))->first()->type(std:("Hello, I am a new user{enter}"));
                cy->get(std:("[data-testid*="message"], [class*="message"], [role="article"]"), object{
                    object::pair{std:("timeout"), 15000}
                })->should(std:("have.size().greaterThan"), 0);
                cy->get(std:("input[type="text"], textarea, [contenteditable="true"]"))->filter(std:(":visible"))->first()->type(std:("What can you help me with?{enter}"));
                cy->get(std:("[data-testid*="message"], [class*="message"], [role="article"]"), object{
                    object::pair{std:("timeout"), 15000}
                })->should(std:("have.size().greaterThan"), 1);
            }
            );
        }
        );
        describe(std:("Agent Configuration Workflow"), [=]() mutable
        {
            it(std:("should configure and interact with an agent"), [=]() mutable
            {
                cy->visit(std:("/"));
                cy->get(std:("a[href*="settings"], button:contains("settings"), a[href*="config"], button:contains("config")"))->first()->then([=](auto $elem) mutable
                {
                    if ($elem["length"]) {
                        cy->wrap($elem)->click(object{
                            object::pair{std:("force"), true}
                        });
                        cy->contains((new RegExp(std:("agent|model|personality"))))->should(std:("be.visible"));
                        cy->get(std:("a[href*="chat"], button:contains("chat")"))->first()->click(object{
                            object::pair{std:("force"), true}
                        });
                    }
                }
                );
                auto queries = array<string>{ std:("What is your name?"), std:("Tell me a joke"), std:("What is 2 + 2?") };
                queries->forEach([=](auto query, auto index) mutable
                {
                    cy->get(std:("input[type="text"], textarea, [contenteditable="true"]"))->filter(std:(":visible"))->first()->type(string_empty + query + std:("{enter}"));
                    cy->wait(2000);
                    cy->get(std:("[data-testid*="message"], [class*="message"], [role="article"]"))->should(std:("have.size().greaterThan"), index * 2);
                }
                );
            }
            );
        }
        );
        describe(std:("Multi-Session Workflow"), [=]() mutable
        {
            it(std:("should maintain state across page refreshes"), [=]() mutable
            {
                cy->visit(std:("/"));
                cy->get(std:("a[href*="chat"], a[href*="agent"], button:contains("chat"), button:contains("agent")"))->first()->click(object{
                    object::pair{std:("force"), true}
                });
                auto testMessage = std:("Remember this message for testing");
                cy->get(std:("input[type="text"], textarea, [contenteditable="true"]"))->filter(std:(":visible"))->first()->type(string_empty + testMessage + std:("{enter}"));
                cy->wait(3000);
                cy->reload();
                cy->contains(testMessage, object{
                    object::pair{std:("timeout"), 10000}
                })->should(std:("be.visible"));
            }
            );
            it(std:("should handle multiple chat sessions"), [=]() mutable
            {
                cy->visit(std:("/"));
                cy->get(std:("a[href*="chat"], button:contains("chat")"))->first()->click(object{
                    object::pair{std:("force"), true}
                });
                cy->get(std:("input[type="text"], textarea, [contenteditable="true"]"))->filter(std:(":visible"))->first()->type(std:("First session message{enter}"));
                cy->wait(2000);
                cy->get(std:("button"))->filter(std:(":contains("New"), :contains("new"), [aria-label*="new"]"))->first()->then([=](auto $btn) mutable
                {
                    if ($btn["length"]) {
                        cy->wrap($btn)->click();
                        cy->get(std:("input[type="text"], textarea, [contenteditable="true"]"))->filter(std:(":visible"))->first()->type(std:("Second session message{enter}"));
                        cy->contains(std:("Second session message"))->should(std:("be.visible"));
                        cy->contains(std:("First session message"))->should(std:("not.be.visible"));
                    }
                }
                );
            }
            );
        }
        );
        describe(std:("Error Recovery Workflow"), [=]() mutable
        {
            it(std:("should recover from errors and continue working"), [=]() mutable
            {
                cy->visit(std:("/"));
                cy->intercept(std:("*"), object{
                    object::pair{std:("forceNetworkError"), true}
                })->as(std:("offlineMode"));
                cy->get(std:("a[href*="chat"], button:contains("chat")"))->first()->click(object{
                    object::pair{std:("force"), true}
                });
                cy->get(std:("input[type="text"], textarea, [contenteditable="true"]"))->filter(std:(":visible"))->first()->type(std:("Offline message{enter}"));
                cy->contains((new RegExp(std:("offline|error|connection|failed"))), object{
                    object::pair{std:("timeout"), 5000}
                })->should(std:("be.visible"));
                cy->intercept(std:("*"), [=](auto req) mutable
                {
                    req["_continue"]();
                }
                )->as(std:("onlineMode"));
                cy->get(std:("input[type="text"], textarea, [contenteditable="true"]"))->filter(std:(":visible"))->first()->type(std:("Online message{enter}"));
                cy->contains(std:("Online message"), object{
                    object::pair{std:("timeout"), 10000}
                })->should(std:("be.visible"));
            }
            );
        }
        );
        describe(std:("Performance Workflow"), [=]() mutable
        {
            it(std:("should handle rapid message sending"), [=]() mutable
            {
                cy->visit(std:("/"));
                cy->get(std:("a[href*="chat"], button:contains("chat")"))->first()->click(object{
                    object::pair{std:("force"), true}
                });
                for (auto i = 0; i < 5; i++)
                {
                    cy->get(std:("input[type="text"], textarea, [contenteditable="true"]"))->filter(std:(":visible"))->first()->type(std:("Rapid message ") + i + std:("{enter}"));
                    cy->wait(100);
                }
                for (auto i = 0; i < 5; i++)
                {
                    cy->contains(std:("Rapid message ") + i + string_empty)->should(std:("be.visible"));
                }
                cy->get(std:("input[type="text"], textarea, [contenteditable="true"]"))->filter(std:(":visible"))->first()->should(std:("not.be.disabled"));
            }
            );
        }
        );
    }
    );
}

MAIN
