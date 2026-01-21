#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-starter/src/__tests__/cypress/e2e/agent-chat.cy.h"

void Main(void)
{
    describe(std::string("Agent Chat E2E Tests"), [=]() mutable
    {
        beforeEach([=]() mutable
        {
            cy->visit(std::string("/"));
            cy->get(std::string("a[href*="chat"], a[href*="agent"], button:contains("chat"), button:contains("agent")"), object{
                object::pair{std::string("timeout"), 5000}
            })->first()->click(object{
                object::pair{std::string("force"), true}
            });
        }
        );
        describe(std::string("Chat Interface"), [=]() mutable
        {
            it(std::string("should display the chat interface"), [=]() mutable
            {
                cy->get(std::string("[data-testid="chat-container"], .chat-container, #chat, [role="main"]"))->should(std::string("be.visible"));
            }
            );
            it(std::string("should have a message input field"), [=]() mutable
            {
                cy->get(std::string("input[type="text"], textarea, [contenteditable="true"]"))->filter(std::string(":visible"))->first()->should(std::string("be.visible"))->should(std::string("not.be.disabled"));
            }
            );
            it(std::string("should have a send button"), [=]() mutable
            {
                cy->get(std::string("button"))->filter(std::string(":contains("Send"), :contains("send"), [aria-label*="send"]"))->should(std::string("be.visible"))->should(std::string("not.be.disabled"));
            }
            );
        }
        );
        describe(std::string("Sending Messages"), [=]() mutable
        {
            it(std::string("should send a message when typing and clicking send"), [=]() mutable
            {
                auto testMessage = std::string("Hello, this is a test message");
                cy->get(std::string("input[type="text"], textarea, [contenteditable="true"]"))->filter(std::string(":visible"))->first()->type(testMessage);
                cy->get(std::string("button"))->filter(std::string(":contains("Send"), :contains("send"), [aria-label*="send"]"))->first()->click();
                cy->contains(testMessage, object{
                    object::pair{std::string("timeout"), 10000}
                })->should(std::string("be.visible"));
            }
            );
            it(std::string("should send a message when pressing Enter"), [=]() mutable
            {
                auto testMessage = std::string("Test message with Enter key");
                cy->get(std::string("input[type="text"], textarea, [contenteditable="true"]"))->filter(std::string(":visible"))->first()->type(string_empty + testMessage + std::string("{enter}"));
                cy->contains(testMessage, object{
                    object::pair{std::string("timeout"), 10000}
                })->should(std::string("be.visible"));
            }
            );
            it(std::string("should clear input after sending"), [=]() mutable
            {
                auto testMessage = std::string("Message to clear");
                auto input = cy->get(std::string("input[type="text"], textarea, [contenteditable="true"]"))->filter(std::string(":visible"))->first();
                input->type(testMessage);
                cy->get(std::string("button"))->filter(std::string(":contains("Send"), :contains("send"), [aria-label*="send"]"))->first()->click();
                input->should(std::string("have.value"), string_empty);
            }
            );
        }
        );
        describe(std::string("Agent Responses"), [=]() mutable
        {
            it(std::string("should receive a response from the agent"), [=]() mutable
            {
                cy->get(std::string("input[type="text"], textarea, [contenteditable="true"]"))->filter(std::string(":visible"))->first()->type(std::string("Hello agent{enter}"));
                cy->get(std::string("[data-testid*="agent"], [class*="agent"], [role="article"]"), object{
                    object::pair{std::string("timeout"), 15000}
                })->should(std::string("have.length.greaterThan"), 0);
            }
            );
            it(std::string("should show typing indicator while agent is responding"), [=]() mutable
            {
                cy->get(std::string("input[type="text"], textarea, [contenteditable="true"]"))->filter(std::string(":visible"))->first()->type(std::string("Tell me about yourself{enter}"));
                cy->get(std::string("[data-testid="typing"], [class*="typing"], [aria-label*="typing"]"), object{
                    object::pair{std::string("timeout"), 5000}
                })->should(std::string("be.visible"));
                cy->get(std::string("[data-testid="typing"], [class*="typing"], [aria-label*="typing"]"), object{
                    object::pair{std::string("timeout"), 15000}
                })->should(std::string("not.exist"));
            }
            );
        }
        );
        describe(std::string("Chat History"), [=]() mutable
        {
            it(std::string("should maintain chat history"), [=]() mutable
            {
                auto messages = array<string>{ std::string("First message"), std::string("Second message"), std::string("Third message") };
                messages->forEach([=](auto msg, auto index) mutable
                {
                    cy->get(std::string("input[type="text"], textarea, [contenteditable="true"]"))->filter(std::string(":visible"))->first()->type(string_empty + msg + std::string("{enter}"));
                    cy->wait(1000);
                }
                );
                messages->forEach([=](auto msg) mutable
                {
                    cy->contains(msg)->should(std::string("be.visible"));
                }
                );
            }
            );
            it(std::string("should scroll to latest message"), [=]() mutable
            {
                for (auto i = 0; i < 10; i++)
                {
                    cy->get(std::string("input[type="text"], textarea, [contenteditable="true"]"))->filter(std::string(":visible"))->first()->type(std::string("Message number ") + i + std::string("{enter}"));
                    cy->wait(500);
                }
                cy->contains(std::string("Message number 9"))->should(std::string("be.visible"));
            }
            );
        }
        );
        describe(std::string("Error Handling"), [=]() mutable
        {
            it(std::string("should handle network errors gracefully"), [=]() mutable
            {
                cy->intercept(std::string("POST"), std::string("**/api/chat/**"), object{
                    object::pair{std::string("statusCode"), 500}, 
                    object::pair{std::string("body"), object{
                        object::pair{std::string("error"), std::string("Server error")}
                    }}
                })->as(std::string("chatError"));
                cy->get(std::string("input[type="text"], textarea, [contenteditable="true"]"))->filter(std::string(":visible"))->first()->type(std::string("This will fail{enter}"));
                cy->contains((new RegExp(std::string("error|failed|try again"))), object{
                    object::pair{std::string("timeout"), 10000}
                })->should(std::string("be.visible"));
            }
            );
            it(std::string("should prevent sending empty messages"), [=]() mutable
            {
                cy->get(std::string("button"))->filter(std::string(":contains("Send"), :contains("send"), [aria-label*="send"]"))->first()->click();
                cy->get(std::string("[data-testid*="message"], [class*="message"]"))->should(std::string("have.length"), 0);
            }
            );
        }
        );
    }
    );
}

MAIN
