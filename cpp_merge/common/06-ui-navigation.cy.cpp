#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/game/cypress/e2e/06-ui-navigation.cy.h"

void Main(void)
{
    describe(std:("UI Navigation"), [=]() mutable
    {
        auto _BACKEND_URL = OR((Cypress->env(std:("BACKEND_URL"))), (std:("http://localhost:7777")));
        before([=]() mutable
        {
            cy->waitForBackend();
        }
        );
        beforeEach([=]() mutable
        {
            cy->window()->then([=](auto win) mutable
            {
                win["localStorage"]["setItem"](std:("skipBoot"), std:("true"));
                win["localStorage"]["setItem"](std:("disableWebSocket"), std:("true"));
            }
            );
            cy->visit(std:("/"), object{
                object::pair{std:("timeout"), 30000}, 
                object::pair{std:("failOnStatusCode"), false}
            });
            cy->wait(3000);
        }
        );
        describe(std:("Main Interface"), [=]() mutable
        {
            it(std:("should display the game interface"), [=]() mutable
            {
                cy->get(std:("[data-testid="game-interface"], .game-interface, body"))->should(std:("be.visible"));
                cy->contains(std:("ELIZA"))->should(std:("be.visible"));
                cy->screenshot(std:("main-interface"));
            }
            );
            it(std:("should display terminal header"), [=]() mutable
            {
                cy->get(std:(".terminal-header, [data-testid="terminal-header"], header"))->should(std:("exist"));
                cy->contains((new RegExp(std:("ELIZA|Terminal"))))->should(std:("be.visible"));
                cy->log(std:("✅ Terminal header displayed"));
            }
            );
            it(std:("should have responsive layout"), [=]() mutable
            {
                auto viewports = array<object>{ object{
                    object::pair{std:("width"), 1920}, 
                    object::pair{std:("height"), 1080}, 
                    object::pair{std:("name"), std:("desktop")}
                }, object{
                    object::pair{std:("width"), 1280}, 
                    object::pair{std:("height"), 720}, 
                    object::pair{std:("name"), std:("laptop")}
                }, object{
                    object::pair{std:("width"), 768}, 
                    object::pair{std:("height"), 1024}, 
                    object::pair{std:("name"), std:("tablet")}
                }, object{
                    object::pair{std:("width"), 375}, 
                    object::pair{std:("height"), 667}, 
                    object::pair{std:("name"), std:("mobile")}
                } };
                viewports->forEach([=](auto viewport) mutable
                {
                    cy->viewport(viewport["width"], viewport["height"]);
                    cy->wait(500);
                    cy->get(std:("body"))->should(std:("be.visible"));
                    cy->screenshot(std:("responsive-") + viewport["name"] + string_empty);
                }
                );
            }
            );
        }
        );
        describe(std:("Tab Navigation"), [=]() mutable
        {
            auto tabs = array<object>{ object{
                object::pair{std:("name"), std:("CHAT")}, 
                object::pair{std:("selector"), std:("chat")}, 
                object::pair{std:("testId"), std:("chat-tab")}
            }, object{
                object::pair{std:("name"), std:("GOALS")}, 
                object::pair{std:("selector"), std:("goals")}, 
                object::pair{std:("testId"), std:("goals-tab")}
            }, object{
                object::pair{std:("name"), std:("TODOS")}, 
                object::pair{std:("selector"), std:("todos")}, 
                object::pair{std:("testId"), std:("todos-tab")}
            }, object{
                object::pair{std:("name"), std:("MONOLOGUE")}, 
                object::pair{std:("selector"), std:("monologue")}, 
                object::pair{std:("testId"), std:("monologue-tab")}
            }, object{
                object::pair{std:("name"), std:("FILES")}, 
                object::pair{std:("selector"), std:("files")}, 
                object::pair{std:("testId"), std:("files-tab")}
            }, object{
                object::pair{std:("name"), std:("CONFIG")}, 
                object::pair{std:("selector"), std:("config")}, 
                object::pair{std:("testId"), std:("config-tab")}
            } };
            tabs->forEach([=](auto tab) mutable
            {
                it(std:("should navigate to ") + tab["name"] + std:(" tab"), [=]() mutable
                {
                    cy->get(std:("[data-testid="") + tab["testId"] + std:(""]"))->click();
                    cy->wait(1000);
                    cy->get(std:("[data-testid="") + tab["selector"] + std:("-content"]"))->should(std:("be.visible"));
                    cy->screenshot(std:("tab-") + tab["selector"] + string_empty);
                    cy->log(std:("✅ ") + tab["name"] + std:(" tab displayed"));
                }
                );
            }
            );
            it(std:("should maintain tab state when switching"), [=]() mutable
            {
                cy->get(std:("[data-testid="goals-tab"]"))->click();
                cy->wait(500);
                cy->get(std:("[data-testid="todos-tab"]"))->click();
                cy->wait(500);
                cy->get(std:("[data-testid="goals-tab"]"))->click();
                cy->get(std:("[data-testid="goals-content"]"))->should(std:("be.visible"));
            }
            );
            it(std:("should highlight active tab"), [=]() mutable
            {
                cy->get(std:("[data-testid="todos-tab"]"))->click();
                cy->get(std:("[data-testid="todos-tab"]"))->should(std:("have.class"), std:("active"))->or(std:("have.css"), std:("background-color"))->or(std:("have.css"), std:("color"));
            }
            );
        }
        );
        describe(std:("Chat Interface"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                cy->get(std:("[data-testid="chat-tab"]"))->click();
                cy->wait(1000);
            }
            );
            it(std:("should display chat input and messages area"), [=]() mutable
            {
                cy->get(std:("[data-testid="message-input"]"))->should(std:("be.visible"));
                cy->get(std:("[data-testid="send-button"]"))->should(std:("be.visible"));
                cy->get(std:("[data-testid="chat-messages"]"))->should(std:("exist"));
                cy->screenshot(std:("chat-interface"));
            }
            );
            it(std:("should allow typing in chat input"), [=]() mutable
            {
                auto testMessage = std:("Test message from Cypress");
                cy->get(std:("[data-testid="message-input"]"))->clear()->type(testMessage)->should(std:("have.value"), testMessage);
            }
            );
            it(std:("should send message on button click"), [=]() mutable
            {
                auto testMessage = std:("UI test message ") + Date->now() + string_empty;
                cy->get(std:("[data-testid="message-input"]"))->clear()->type(testMessage);
                cy->get(std:("[data-testid="send-button"]"))->click();
                cy->get(std:("[data-testid="message-input"]"))->should(std:("have.value"), string_empty);
                cy->get(std:("[data-testid="chat-messages"]"))->should(std:("contain"), testMessage);
            }
            );
            it(std:("should send message on Enter key"), [=]() mutable
            {
                auto testMessage = std:("Enter key test ") + Date->now() + string_empty;
                cy->get(std:("[data-testid="message-input"]"))->clear()->type(string_empty + testMessage + std:("{enter}"));
                cy->get(std:("[data-testid="chat-messages"]"))->should(std:("contain"), testMessage);
            }
            );
            it(std:("should display user and agent messages differently"), [=]() mutable
            {
                cy->get(std:("[data-testid="message-input"]"))->type(std:("Test message{enter}"));
                cy->wait(2000);
                cy->get(std:("[data-testid="user-message"]"))->should(std:("exist"));
                cy->get(std:("[data-testid="agent-message"]"))->should(std:("exist"));
                cy->get(std:("[data-testid="user-message"]"))->then([=](auto $userMsg) mutable
                {
                    cy->get(std:("[data-testid="agent-message"]"))->then([=](auto $agentMsg) mutable
                    {
                        auto userBg = $userMsg["css"](std:("background-color"));
                        auto agentBg = $agentMsg["css"](std:("background-color"));
                        expect(userBg)->to->not->equal(agentBg);
                    }
                    );
                }
                );
            }
            );
        }
        );
        describe(std:("Capability Buttons"), [=]() mutable
        {
            shared capabilities = array<object>{ object{
                object::pair{std:("name"), std:("AUTO")}, 
                object::pair{std:("testId"), std:("autonomy-toggle")}, 
                object::pair{std:("setting"), std:("autonomy")}
            }, object{
                object::pair{std:("name"), std:("CAM")}, 
                object::pair{std:("testId"), std:("camera-toggle")}, 
                object::pair{std:("setting"), std:("camera")}
            }, object{
                object::pair{std:("name"), std:("SCR")}, 
                object::pair{std:("testId"), std:("screen-toggle")}, 
                object::pair{std:("setting"), std:("screen")}
            }, object{
                object::pair{std:("name"), std:("MIC")}, 
                object::pair{std:("testId"), std:("microphone-toggle")}, 
                object::pair{std:("setting"), std:("microphone")}
            }, object{
                object::pair{std:("name"), std:("SH")}, 
                object::pair{std:("testId"), std:("shell-toggle")}, 
                object::pair{std:("setting"), std:("shell")}
            }, object{
                object::pair{std:("name"), std:("WWW")}, 
                object::pair{std:("testId"), std:("browser-toggle")}, 
                object::pair{std:("setting"), std:("browser")}
            } };
            it(std:("should display all capability buttons"), [=]() mutable
            {
                capabilities->forEach([=](auto cap) mutable
                {
                    cy->get(std:("[data-testid="") + cap["testId"] + std:(""]"))->should(std:("be.visible"));
                    cy->get(std:("[data-testid="") + cap["testId"] + std:(""]"))->should(std:("contain"), cap["name"]);
                }
                );
                cy->screenshot(std:("capability-buttons"));
            }
            );
            capabilities->forEach([=](auto cap) mutable
            {
                it(std:("should toggle ") + cap["name"] + std:(" capability"), [=]() mutable
                {
                    cy->get(std:("[data-testid="") + cap["testId"] + std:(""]"))->then([=](auto $btn) mutable
                    {
                        shared initialClass = $btn["attr"](std:("class"));
                        cy->get(std:("[data-testid="") + cap["testId"] + std:(""]"))->click();
                        cy->wait(1000);
                        cy->get(std:("[data-testid="") + cap["testId"] + std:(""]"))->then([=](auto $newBtn) mutable
                        {
                            auto newClass = $newBtn["attr"](std:("class"));
                            expect(newClass)->to->not->equal(initialClass);
                        }
                        );
                        cy->get(std:("[data-testid="") + cap["testId"] + std:("-status"]"))->should(std:("exist"));
                    }
                    );
                }
                );
            }
            );
            it(std:("should show capability status indicators"), [=]() mutable
            {
                capabilities->forEach([=](auto cap) mutable
                {
                    cy->get(std:("[data-testid="") + cap["testId"] + std:(""]"))->within([=]() mutable
                    {
                        cy->get(std:("[data-testid*="status"], .status-indicator, .toggle-status"))->should(std:("exist"));
                    }
                    );
                }
                );
            }
            );
        }
        );
        describe(std:("Tab Content"), [=]() mutable
        {
            it(std:("should display goals content"), [=]() mutable
            {
                cy->get(std:("[data-testid="goals-tab"]"))->click();
                cy->wait(1000);
                cy->get(std:("[data-testid="goals-content"]"))->within([=]() mutable
                {
                    cy->get(std:("body"))->then([=](auto $body) mutable
                    {
                        if ($body["find"](std:(".goal-item, [data-testid="goal-item"]"))["length"] > 0) {
                            cy->log(std:("✅ Goals displayed"));
                            cy->get(std:(".goal-item, [data-testid="goal-item"]"))->should(std:("have.size().at.least"), 1);
                        } else {
                            cy->log(std:("✅ Goals empty state displayed"));
                            cy->contains((new RegExp(std:("no.*goals|empty"))))->should(std:("be.visible"));
                        }
                    }
                    );
                }
                );
            }
            );
            it(std:("should display todos content"), [=]() mutable
            {
                cy->get(std:("[data-testid="todos-tab"]"))->click();
                cy->wait(1000);
                cy->get(std:("[data-testid="todos-content"]"))->within([=]() mutable
                {
                    cy->get(std:("body"))->then([=](auto $body) mutable
                    {
                        if ($body["find"](std:(".todo-item, [data-testid="todo-item"]"))["length"] > 0) {
                            cy->log(std:("✅ Todos displayed"));
                            cy->get(std:(".todo-item, [data-testid="todo-item"]"))->should(std:("have.size().at.least"), 1);
                        } else {
                            cy->log(std:("✅ Todos empty state displayed"));
                            cy->contains((new RegExp(std:("no.*todos|no.*tasks|empty"))))->should(std:("be.visible"));
                        }
                    }
                    );
                }
                );
            }
            );
            it(std:("should display monologue content"), [=]() mutable
            {
                cy->get(std:("[data-testid="monologue-tab"]"))->click();
                cy->wait(1000);
                cy->get(std:("[data-testid="monologue-content"]"))->within([=]() mutable
                {
                    cy->get(std:("body"))->then([=](auto $body) mutable
                    {
                        if ($body["find"](std:(".thought-item, [data-testid="thought-item"]"))["length"] > 0) {
                            cy->log(std:("✅ Thoughts displayed"));
                        } else {
                            cy->log(std:("✅ Monologue empty state displayed"));
                            cy->contains((new RegExp(std:("no.*thoughts|quiet|empty"))))->should(std:("be.visible"));
                        }
                    }
                    );
                }
                );
            }
            );
            it(std:("should display files content with upload capability"), [=]() mutable
            {
                cy->get(std:("[data-testid="files-tab"]"))->click();
                cy->wait(1000);
                cy->get(std:("[data-testid="files-content"]"))->within([=]() mutable
                {
                    cy->get(std:("input[type="file"]"))->should(std:("exist"));
                    cy->contains(std:("KNOWLEDGE"))->should(std:("be.visible"));
                    cy->get(std:("body"))->then([=](auto $body) mutable
                    {
                        if ($body["find"](std:(".file-item, [data-testid="file-item"]"))["length"] > 0) {
                            cy->log(std:("✅ Files displayed"));
                            cy->get(std:(".delete-button, [data-testid="delete-file"]"))->should(std:("have.size().at.least"), 1);
                        } else {
                            cy->log(std:("✅ Files empty state displayed"));
                            cy->contains((new RegExp(std:("no.*files|empty"))))->should(std:("be.visible"));
                        }
                    }
                    );
                }
                );
            }
            );
            it(std:("should display config content with settings"), [=]() mutable
            {
                cy->get(std:("[data-testid="config-tab"]"))->click();
                cy->wait(1000);
                cy->get(std:("[data-testid="config-content"]"))->within([=]() mutable
                {
                    cy->contains(std:("CONFIGURATION"))->should(std:("be.visible"));
                    cy->get(std:("[data-testid="model-provider-select"]"))->should(std:("be.visible"));
                    cy->get(std:("[data-testid="openai-api-key-input"]"))->should(std:("exist"));
                    cy->get(std:("[data-testid="validate-config-button"]"))->should(std:("be.visible"));
                    cy->get(std:("[data-testid="test-config-button"]"))->should(std:("be.visible"));
                    cy->contains(std:("Danger Zone"))->should(std:("be.visible"));
                    cy->contains(std:("RESET"))->should(std:("be.visible"));
                }
                );
            }
            );
        }
        );
        describe(std:("Visual Consistency"), [=]() mutable
        {
            it(std:("should maintain consistent styling across tabs"), [=]() mutable
            {
                auto tabs = array<string>{ std:("chat"), std:("goals"), std:("todos"), std:("monologue"), std:("files"), std:("config") };
                tabs->forEach([=](auto tab) mutable
                {
                    cy->get(std:("[data-testid="") + tab + std:("-tab"]"))->click();
                    cy->wait(500);
                    cy->get(std:("[data-testid="") + tab + std:("-content"]"))->should(std:("have.css"), std:("padding"));
                    cy->get(std:("[data-testid="") + tab + std:("-content"]"))->then([=](auto $el) mutable
                    {
                        auto bg = $el["css"](std:("background-color"));
                        cy->log(string_empty + tab + std:(" background: ") + bg + string_empty);
                    }
                    );
                }
                );
            }
            );
            it(std:("should have consistent button styling"), [=]() mutable
            {
                cy->get(std:("button"))->each([=](auto $btn) mutable
                {
                    expect($btn["css"](std:("padding")))->to->not->equal(std:("0px"));
                    expect($btn["css"](std:("cursor")))->to->equal(std:("pointer"));
                }
                );
            }
            );
            it(std:("should handle long content with scrolling"), [=]() mutable
            {
                cy->get(std:("[data-testid="chat-tab"]"))->click();
                for (auto i = 0; i < 20; i++)
                {
                    cy->get(std:("[data-testid="message-input"]"))->type(std:("Test message ") + i + std:("{enter}"));
                    cy->wait(100);
                }
                cy->get(std:("[data-testid="chat-messages"]"))->then([=](auto $el) mutable
                {
                    auto scrollHeight = const_($el)[0]["scrollHeight"];
                    auto clientHeight = const_($el)[0]["clientHeight"];
                    if (scrollHeight > clientHeight) {
                        cy->log(std:("✅ Content is scrollable"));
                        cy->get(std:("[data-testid="chat-messages"]"))->scrollTo(std:("bottom"));
                    }
                }
                );
            }
            );
        }
        );
        describe(std:("Error States"), [=]() mutable
        {
            it(std:("should handle backend connection errors gracefully"), [=]() mutable
            {
                cy->get(std:("[data-testid="goals-tab"]"))->click();
                cy->get(std:("[data-testid="goals-content"]"))->should(std:("be.visible"));
            }
            );
            it(std:("should show loading states"), [=]() mutable
            {
                auto tabs = array<string>{ std:("goals"), std:("todos"), std:("monologue") };
                tabs->forEach([=](auto tab) mutable
                {
                    cy->get(std:("[data-testid="") + tab + std:("-tab"]"))->click();
                    cy->get(std:("[data-testid="") + tab + std:("-content"]"))->within([=]() mutable
                    {
                        cy->get(std:(".loading, [data-testid="loading"], .spinner"))->then([=](auto $loading) mutable
                        {
                            if ($loading["length"] > 0) {
                                cy->log(std:("✅ ") + tab + std:(" shows loading state"));
                            }
                        }
                        );
                    }
                    );
                }
                );
            }
            );
        }
        );
    }
    );
    describe(std:("UI Navigation Summary"), [=]() mutable
    {
        it(std:("should verify complete UI functionality"), [=]() mutable
        {
            auto tabs = array<string>{ std:("goals"), std:("todos"), std:("monologue"), std:("files"), std:("config"), std:("logs"), std:("agent-screen") };
            tabs->forEach([=](auto tab) mutable
            {
                cy->get(std:("[data-testid="") + tab + std:("-tab"]"))->should(std:("be.visible"));
            }
            );
            tabs->forEach([=](auto tab) mutable
            {
                cy->get(std:("[data-testid="") + tab + std:("-tab"]"))->click();
                cy->wait(300);
                cy->get(std:("[data-testid="") + tab + std:("-content"]"))->should(std:("be.visible"));
            }
            );
            cy->get(std:("[data-testid="message-input"]"))->should(std:("be.visible"));
            cy->get(std:("[data-testid="send-button"]"))->should(std:("be.visible"));
            cy->get(std:("[data-testid="output-container"]"))->should(std:("be.visible"));
            cy->get(std:("[data-testid="autonomy-toggle"]"))->should(std:("be.visible"));
            cy->get(std:("[data-testid="camera-toggle"]"))->should(std:("be.visible"));
            cy->get(std:("[data-testid="screen-toggle"]"))->should(std:("be.visible"));
            cy->get(std:("[data-testid="microphone-toggle"]"))->should(std:("be.visible"));
            cy->get(std:("[data-testid="shell-toggle"]"))->should(std:("be.visible"));
            cy->get(std:("[data-testid="browser-toggle"]"))->should(std:("be.visible"));
            cy->get(std:("[data-testid="game-interface"]"))->should(std:("be.visible"));
            cy->get(std:("[data-testid="connection-status"]"))->should(std:("be.visible"));
            cy->log(std:("✅ UI navigation verification complete"));
        }
        );
    }
    );
}

MAIN
