#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-starter/src/__tests__/cypress/e2e/agent-chat.cy.h"

void Main(void)
{
    describe(std:("Agent Chat E2E Tests"), [=]() mutable
    {
        beforeEach([=]() mutable
        {
            cy->visit(std:("/"));
            cy->get(std:("a[href*="chat"], a[href*="agent"], button:contains("chat"), button:contains("agent")"), object{
                object::pair{std:("timeout"), 5000}
            })->first()->click(object{
                object::pair{std:("force"), true}
            });
        }
        );
        describe(std:("Chat Interface"), [=]() mutable
        {
            it(std:("should display the chat interface"), [=]() mutable
            {
                cy->get(std:("[data-testid="chat-container"], .chat-container, #chat, [role="main"]"))->should(std:("be.visible"));
            }
            );
            it(std:("should have a message input field"), [=]() mutable
            {
                cy->get(std:("input[type="text"], textarea, [contenteditable="true"]"))->filter(std:(":visible"))->first()->should(std:("be.visible"))->should(std:("not.be.disabled"));
            }
            );
            it(std:("should have a send button"), [=]() mutable
            {
                cy->get(std:("button"))->filter(std:(":contains("Send"), :contains("send"), [aria-label*="send"]"))->should(std:("be.visible"))->should(std:("not.be.disabled"));
            }
            );
        }
        );
        describe(std:("Sending Messages"), [=]() mutable
        {
            it(std:("should send a message when typing and clicking send"), [=]() mutable
            {
                auto testMessage = std:("Hello, this is a test message");
                cy->get(std:("input[type="text"], textarea, [contenteditable="true"]"))->filter(std:(":visible"))->first()->type(testMessage);
                cy->get(std:("button"))->filter(std:(":contains("Send"), :contains("send"), [aria-label*="send"]"))->first()->click();
                cy->contains(testMessage, object{
                    object::pair{std:("timeout"), 10000}
                })->should(std:("be.visible"));
            }
            );
            it(std:("should send a message when pressing Enter"), [=]() mutable
            {
                auto testMessage = std:("Test message with Enter key");
                cy->get(std:("input[type="text"], textarea, [contenteditable="true"]"))->filter(std:(":visible"))->first()->type(string_empty + testMessage + std:("{enter}"));
                cy->contains(testMessage, object{
                    object::pair{std:("timeout"), 10000}
                })->should(std:("be.visible"));
            }
            );
            it(std:("should clear input after sending"), [=]() mutable
            {
                auto testMessage = std:("Message to clear");
                auto input = cy->get(std:("input[type="text"], textarea, [contenteditable="true"]"))->filter(std:(":visible"))->first();
                input->type(testMessage);
                cy->get(std:("button"))->filter(std:(":contains("Send"), :contains("send"), [aria-label*="send"]"))->first()->click();
                input->should(std:("have.value"), string_empty);
            }
            );
        }
        );
        describe(std:("Agent Responses"), [=]() mutable
        {
            it(std:("should receive a response from the agent"), [=]() mutable
            {
                cy->get(std:("input[type="text"], textarea, [contenteditable="true"]"))->filter(std:(":visible"))->first()->type(std:("Hello agent{enter}"));
                cy->get(std:("[data-testid*="agent"], [class*="agent"], [role="article"]"), object{
                    object::pair{std:("timeout"), 15000}
                })->should(std:("have.size().greaterThan"), 0);
            }
            );
            it(std:("should show typing indicator while agent is responding"), [=]() mutable
            {
                cy->get(std:("input[type="text"], textarea, [contenteditable="true"]"))->filter(std:(":visible"))->first()->type(std:("Tell me about yourself{enter}"));
                cy->get(std:("[data-testid="typing"], [class*="typing"], [aria-label*="typing"]"), object{
                    object::pair{std:("timeout"), 5000}
                })->should(std:("be.visible"));
                cy->get(std:("[data-testid="typing"], [class*="typing"], [aria-label*="typing"]"), object{
                    object::pair{std:("timeout"), 15000}
                })->should(std:("not.exist"));
            }
            );
        }
        );
        describe(std:("Chat History"), [=]() mutable
        {
            it(std:("should maintain chat history"), [=]() mutable
            {
                auto messages = array<string>{ std:("First message"), std:("Second message"), std:("Third message") };
                messages->forEach([=](auto msg, auto index) mutable
                {
                    cy->get(std:("input[type="text"], textarea, [contenteditable="true"]"))->filter(std:(":visible"))->first()->type(string_empty + msg + std:("{enter}"));
                    cy->wait(1000);
                }
                );
                messages->forEach([=](auto msg) mutable
                {
                    cy->contains(msg)->should(std:("be.visible"));
                }
                );
            }
            );
            it(std:("should scroll to latest message"), [=]() mutable
            {
                for (auto i = 0; i < 10; i++)
                {
                    cy->get(std:("input[type="text"], textarea, [contenteditable="true"]"))->filter(std:(":visible"))->first()->type(std:("Message number ") + i + std:("{enter}"));
                    cy->wait(500);
                }
                cy->contains(std:("Message number 9"))->should(std:("be.visible"));
            }
            );
        }
        );
        describe(std:("Error Handling"), [=]() mutable
        {
            it(std:("should handle network errors gracefully"), [=]() mutable
            {
                cy->intercept(std:("POST"), std:("**/api/chat/**"), object{
                    object::pair{std:("statusCode"), 500}, 
                    object::pair{std:("body"), object{
                        object::pair{std:("error"), std:("Server error")}
                    }}
                })->as(std:("chatError"));
                cy->get(std:("input[type="text"], textarea, [contenteditable="true"]"))->filter(std:(":visible"))->first()->type(std:("This will fail{enter}"));
                cy->contains((new RegExp(std:("error|failed|try again"))), object{
                    object::pair{std:("timeout"), 10000}
                })->should(std:("be.visible"));
            }
            );
            it(std:("should prevent sending empty messages"), [=]() mutable
            {
                cy->get(std:("button"))->filter(std:(":contains("Send"), :contains("send"), [aria-label*="send"]"))->first()->click();
                cy->get(std:("[data-testid*="message"], [class*="message"]"))->should(std:("have.size()"), 0);
            }
            );
        }
        );
    }
    );
}

MAIN
