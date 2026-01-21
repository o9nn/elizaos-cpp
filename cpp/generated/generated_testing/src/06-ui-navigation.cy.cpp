#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/game/cypress/e2e/06-ui-navigation.cy.h"

void Main(void)
{
    describe(std::string("UI Navigation"), [=]() mutable
    {
        auto _BACKEND_URL = OR((Cypress->env(std::string("BACKEND_URL"))), (std::string("http://localhost:7777")));
        before([=]() mutable
        {
            cy->waitForBackend();
        }
        );
        beforeEach([=]() mutable
        {
            cy->window()->then([=](auto win) mutable
            {
                win["localStorage"]["setItem"](std::string("skipBoot"), std::string("true"));
                win["localStorage"]["setItem"](std::string("disableWebSocket"), std::string("true"));
            }
            );
            cy->visit(std::string("/"), object{
                object::pair{std::string("timeout"), 30000}, 
                object::pair{std::string("failOnStatusCode"), false}
            });
            cy->wait(3000);
        }
        );
        describe(std::string("Main Interface"), [=]() mutable
        {
            it(std::string("should display the game interface"), [=]() mutable
            {
                cy->get(std::string("[data-testid="game-interface"], .game-interface, body"))->should(std::string("be.visible"));
                cy->contains(std::string("ELIZA"))->should(std::string("be.visible"));
                cy->screenshot(std::string("main-interface"));
            }
            );
            it(std::string("should display terminal header"), [=]() mutable
            {
                cy->get(std::string(".terminal-header, [data-testid="terminal-header"], header"))->should(std::string("exist"));
                cy->contains((new RegExp(std::string("ELIZA|Terminal"))))->should(std::string("be.visible"));
                cy->log(std::string("✅ Terminal header displayed"));
            }
            );
            it(std::string("should have responsive layout"), [=]() mutable
            {
                auto viewports = array<object>{ object{
                    object::pair{std::string("width"), 1920}, 
                    object::pair{std::string("height"), 1080}, 
                    object::pair{std::string("name"), std::string("desktop")}
                }, object{
                    object::pair{std::string("width"), 1280}, 
                    object::pair{std::string("height"), 720}, 
                    object::pair{std::string("name"), std::string("laptop")}
                }, object{
                    object::pair{std::string("width"), 768}, 
                    object::pair{std::string("height"), 1024}, 
                    object::pair{std::string("name"), std::string("tablet")}
                }, object{
                    object::pair{std::string("width"), 375}, 
                    object::pair{std::string("height"), 667}, 
                    object::pair{std::string("name"), std::string("mobile")}
                } };
                viewports->forEach([=](auto viewport) mutable
                {
                    cy->viewport(viewport["width"], viewport["height"]);
                    cy->wait(500);
                    cy->get(std::string("body"))->should(std::string("be.visible"));
                    cy->screenshot(std::string("responsive-") + viewport["name"] + string_empty);
                }
                );
            }
            );
        }
        );
        describe(std::string("Tab Navigation"), [=]() mutable
        {
            auto tabs = array<object>{ object{
                object::pair{std::string("name"), std::string("CHAT")}, 
                object::pair{std::string("selector"), std::string("chat")}, 
                object::pair{std::string("testId"), std::string("chat-tab")}
            }, object{
                object::pair{std::string("name"), std::string("GOALS")}, 
                object::pair{std::string("selector"), std::string("goals")}, 
                object::pair{std::string("testId"), std::string("goals-tab")}
            }, object{
                object::pair{std::string("name"), std::string("TODOS")}, 
                object::pair{std::string("selector"), std::string("todos")}, 
                object::pair{std::string("testId"), std::string("todos-tab")}
            }, object{
                object::pair{std::string("name"), std::string("MONOLOGUE")}, 
                object::pair{std::string("selector"), std::string("monologue")}, 
                object::pair{std::string("testId"), std::string("monologue-tab")}
            }, object{
                object::pair{std::string("name"), std::string("FILES")}, 
                object::pair{std::string("selector"), std::string("files")}, 
                object::pair{std::string("testId"), std::string("files-tab")}
            }, object{
                object::pair{std::string("name"), std::string("CONFIG")}, 
                object::pair{std::string("selector"), std::string("config")}, 
                object::pair{std::string("testId"), std::string("config-tab")}
            } };
            tabs->forEach([=](auto tab) mutable
            {
                it(std::string("should navigate to ") + tab["name"] + std::string(" tab"), [=]() mutable
                {
                    cy->get(std::string("[data-testid="") + tab["testId"] + std::string(""]"))->click();
                    cy->wait(1000);
                    cy->get(std::string("[data-testid="") + tab["selector"] + std::string("-content"]"))->should(std::string("be.visible"));
                    cy->screenshot(std::string("tab-") + tab["selector"] + string_empty);
                    cy->log(std::string("✅ ") + tab["name"] + std::string(" tab displayed"));
                }
                );
            }
            );
            it(std::string("should maintain tab state when switching"), [=]() mutable
            {
                cy->get(std::string("[data-testid="goals-tab"]"))->click();
                cy->wait(500);
                cy->get(std::string("[data-testid="todos-tab"]"))->click();
                cy->wait(500);
                cy->get(std::string("[data-testid="goals-tab"]"))->click();
                cy->get(std::string("[data-testid="goals-content"]"))->should(std::string("be.visible"));
            }
            );
            it(std::string("should highlight active tab"), [=]() mutable
            {
                cy->get(std::string("[data-testid="todos-tab"]"))->click();
                cy->get(std::string("[data-testid="todos-tab"]"))->should(std::string("have.class"), std::string("active"))->or(std::string("have.css"), std::string("background-color"))->or(std::string("have.css"), std::string("color"));
            }
            );
        }
        );
        describe(std::string("Chat Interface"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                cy->get(std::string("[data-testid="chat-tab"]"))->click();
                cy->wait(1000);
            }
            );
            it(std::string("should display chat input and messages area"), [=]() mutable
            {
                cy->get(std::string("[data-testid="message-input"]"))->should(std::string("be.visible"));
                cy->get(std::string("[data-testid="send-button"]"))->should(std::string("be.visible"));
                cy->get(std::string("[data-testid="chat-messages"]"))->should(std::string("exist"));
                cy->screenshot(std::string("chat-interface"));
            }
            );
            it(std::string("should allow typing in chat input"), [=]() mutable
            {
                auto testMessage = std::string("Test message from Cypress");
                cy->get(std::string("[data-testid="message-input"]"))->clear()->type(testMessage)->should(std::string("have.value"), testMessage);
            }
            );
            it(std::string("should send message on button click"), [=]() mutable
            {
                auto testMessage = std::string("UI test message ") + Date->now() + string_empty;
                cy->get(std::string("[data-testid="message-input"]"))->clear()->type(testMessage);
                cy->get(std::string("[data-testid="send-button"]"))->click();
                cy->get(std::string("[data-testid="message-input"]"))->should(std::string("have.value"), string_empty);
                cy->get(std::string("[data-testid="chat-messages"]"))->should(std::string("contain"), testMessage);
            }
            );
            it(std::string("should send message on Enter key"), [=]() mutable
            {
                auto testMessage = std::string("Enter key test ") + Date->now() + string_empty;
                cy->get(std::string("[data-testid="message-input"]"))->clear()->type(string_empty + testMessage + std::string("{enter}"));
                cy->get(std::string("[data-testid="chat-messages"]"))->should(std::string("contain"), testMessage);
            }
            );
            it(std::string("should display user and agent messages differently"), [=]() mutable
            {
                cy->get(std::string("[data-testid="message-input"]"))->type(std::string("Test message{enter}"));
                cy->wait(2000);
                cy->get(std::string("[data-testid="user-message"]"))->should(std::string("exist"));
                cy->get(std::string("[data-testid="agent-message"]"))->should(std::string("exist"));
                cy->get(std::string("[data-testid="user-message"]"))->then([=](auto $userMsg) mutable
                {
                    cy->get(std::string("[data-testid="agent-message"]"))->then([=](auto $agentMsg) mutable
                    {
                        auto userBg = $userMsg["css"](std::string("background-color"));
                        auto agentBg = $agentMsg["css"](std::string("background-color"));
                        expect(userBg)->to->not->equal(agentBg);
                    }
                    );
                }
                );
            }
            );
        }
        );
        describe(std::string("Capability Buttons"), [=]() mutable
        {
            shared capabilities = array<object>{ object{
                object::pair{std::string("name"), std::string("AUTO")}, 
                object::pair{std::string("testId"), std::string("autonomy-toggle")}, 
                object::pair{std::string("setting"), std::string("autonomy")}
            }, object{
                object::pair{std::string("name"), std::string("CAM")}, 
                object::pair{std::string("testId"), std::string("camera-toggle")}, 
                object::pair{std::string("setting"), std::string("camera")}
            }, object{
                object::pair{std::string("name"), std::string("SCR")}, 
                object::pair{std::string("testId"), std::string("screen-toggle")}, 
                object::pair{std::string("setting"), std::string("screen")}
            }, object{
                object::pair{std::string("name"), std::string("MIC")}, 
                object::pair{std::string("testId"), std::string("microphone-toggle")}, 
                object::pair{std::string("setting"), std::string("microphone")}
            }, object{
                object::pair{std::string("name"), std::string("SH")}, 
                object::pair{std::string("testId"), std::string("shell-toggle")}, 
                object::pair{std::string("setting"), std::string("shell")}
            }, object{
                object::pair{std::string("name"), std::string("WWW")}, 
                object::pair{std::string("testId"), std::string("browser-toggle")}, 
                object::pair{std::string("setting"), std::string("browser")}
            } };
            it(std::string("should display all capability buttons"), [=]() mutable
            {
                capabilities->forEach([=](auto cap) mutable
                {
                    cy->get(std::string("[data-testid="") + cap["testId"] + std::string(""]"))->should(std::string("be.visible"));
                    cy->get(std::string("[data-testid="") + cap["testId"] + std::string(""]"))->should(std::string("contain"), cap["name"]);
                }
                );
                cy->screenshot(std::string("capability-buttons"));
            }
            );
            capabilities->forEach([=](auto cap) mutable
            {
                it(std::string("should toggle ") + cap["name"] + std::string(" capability"), [=]() mutable
                {
                    cy->get(std::string("[data-testid="") + cap["testId"] + std::string(""]"))->then([=](auto $btn) mutable
                    {
                        shared initialClass = $btn["attr"](std::string("class"));
                        cy->get(std::string("[data-testid="") + cap["testId"] + std::string(""]"))->click();
                        cy->wait(1000);
                        cy->get(std::string("[data-testid="") + cap["testId"] + std::string(""]"))->then([=](auto $newBtn) mutable
                        {
                            auto newClass = $newBtn["attr"](std::string("class"));
                            expect(newClass)->to->not->equal(initialClass);
                        }
                        );
                        cy->get(std::string("[data-testid="") + cap["testId"] + std::string("-status"]"))->should(std::string("exist"));
                    }
                    );
                }
                );
            }
            );
            it(std::string("should show capability status indicators"), [=]() mutable
            {
                capabilities->forEach([=](auto cap) mutable
                {
                    cy->get(std::string("[data-testid="") + cap["testId"] + std::string(""]"))->within([=]() mutable
                    {
                        cy->get(std::string("[data-testid*="status"], .status-indicator, .toggle-status"))->should(std::string("exist"));
                    }
                    );
                }
                );
            }
            );
        }
        );
        describe(std::string("Tab Content"), [=]() mutable
        {
            it(std::string("should display goals content"), [=]() mutable
            {
                cy->get(std::string("[data-testid="goals-tab"]"))->click();
                cy->wait(1000);
                cy->get(std::string("[data-testid="goals-content"]"))->within([=]() mutable
                {
                    cy->get(std::string("body"))->then([=](auto $body) mutable
                    {
                        if ($body["find"](std::string(".goal-item, [data-testid="goal-item"]"))["length"] > 0) {
                            cy->log(std::string("✅ Goals displayed"));
                            cy->get(std::string(".goal-item, [data-testid="goal-item"]"))->should(std::string("have.length.at.least"), 1);
                        } else {
                            cy->log(std::string("✅ Goals empty state displayed"));
                            cy->contains((new RegExp(std::string("no.*goals|empty"))))->should(std::string("be.visible"));
                        }
                    }
                    );
                }
                );
            }
            );
            it(std::string("should display todos content"), [=]() mutable
            {
                cy->get(std::string("[data-testid="todos-tab"]"))->click();
                cy->wait(1000);
                cy->get(std::string("[data-testid="todos-content"]"))->within([=]() mutable
                {
                    cy->get(std::string("body"))->then([=](auto $body) mutable
                    {
                        if ($body["find"](std::string(".todo-item, [data-testid="todo-item"]"))["length"] > 0) {
                            cy->log(std::string("✅ Todos displayed"));
                            cy->get(std::string(".todo-item, [data-testid="todo-item"]"))->should(std::string("have.length.at.least"), 1);
                        } else {
                            cy->log(std::string("✅ Todos empty state displayed"));
                            cy->contains((new RegExp(std::string("no.*todos|no.*tasks|empty"))))->should(std::string("be.visible"));
                        }
                    }
                    );
                }
                );
            }
            );
            it(std::string("should display monologue content"), [=]() mutable
            {
                cy->get(std::string("[data-testid="monologue-tab"]"))->click();
                cy->wait(1000);
                cy->get(std::string("[data-testid="monologue-content"]"))->within([=]() mutable
                {
                    cy->get(std::string("body"))->then([=](auto $body) mutable
                    {
                        if ($body["find"](std::string(".thought-item, [data-testid="thought-item"]"))["length"] > 0) {
                            cy->log(std::string("✅ Thoughts displayed"));
                        } else {
                            cy->log(std::string("✅ Monologue empty state displayed"));
                            cy->contains((new RegExp(std::string("no.*thoughts|quiet|empty"))))->should(std::string("be.visible"));
                        }
                    }
                    );
                }
                );
            }
            );
            it(std::string("should display files content with upload capability"), [=]() mutable
            {
                cy->get(std::string("[data-testid="files-tab"]"))->click();
                cy->wait(1000);
                cy->get(std::string("[data-testid="files-content"]"))->within([=]() mutable
                {
                    cy->get(std::string("input[type="file"]"))->should(std::string("exist"));
                    cy->contains(std::string("KNOWLEDGE"))->should(std::string("be.visible"));
                    cy->get(std::string("body"))->then([=](auto $body) mutable
                    {
                        if ($body["find"](std::string(".file-item, [data-testid="file-item"]"))["length"] > 0) {
                            cy->log(std::string("✅ Files displayed"));
                            cy->get(std::string(".delete-button, [data-testid="delete-file"]"))->should(std::string("have.length.at.least"), 1);
                        } else {
                            cy->log(std::string("✅ Files empty state displayed"));
                            cy->contains((new RegExp(std::string("no.*files|empty"))))->should(std::string("be.visible"));
                        }
                    }
                    );
                }
                );
            }
            );
            it(std::string("should display config content with settings"), [=]() mutable
            {
                cy->get(std::string("[data-testid="config-tab"]"))->click();
                cy->wait(1000);
                cy->get(std::string("[data-testid="config-content"]"))->within([=]() mutable
                {
                    cy->contains(std::string("CONFIGURATION"))->should(std::string("be.visible"));
                    cy->get(std::string("[data-testid="model-provider-select"]"))->should(std::string("be.visible"));
                    cy->get(std::string("[data-testid="openai-api-key-input"]"))->should(std::string("exist"));
                    cy->get(std::string("[data-testid="validate-config-button"]"))->should(std::string("be.visible"));
                    cy->get(std::string("[data-testid="test-config-button"]"))->should(std::string("be.visible"));
                    cy->contains(std::string("Danger Zone"))->should(std::string("be.visible"));
                    cy->contains(std::string("RESET"))->should(std::string("be.visible"));
                }
                );
            }
            );
        }
        );
        describe(std::string("Visual Consistency"), [=]() mutable
        {
            it(std::string("should maintain consistent styling across tabs"), [=]() mutable
            {
                auto tabs = array<string>{ std::string("chat"), std::string("goals"), std::string("todos"), std::string("monologue"), std::string("files"), std::string("config") };
                tabs->forEach([=](auto tab) mutable
                {
                    cy->get(std::string("[data-testid="") + tab + std::string("-tab"]"))->click();
                    cy->wait(500);
                    cy->get(std::string("[data-testid="") + tab + std::string("-content"]"))->should(std::string("have.css"), std::string("padding"));
                    cy->get(std::string("[data-testid="") + tab + std::string("-content"]"))->then([=](auto $el) mutable
                    {
                        auto bg = $el["css"](std::string("background-color"));
                        cy->log(string_empty + tab + std::string(" background: ") + bg + string_empty);
                    }
                    );
                }
                );
            }
            );
            it(std::string("should have consistent button styling"), [=]() mutable
            {
                cy->get(std::string("button"))->each([=](auto $btn) mutable
                {
                    expect($btn["css"](std::string("padding")))->to->not->equal(std::string("0px"));
                    expect($btn["css"](std::string("cursor")))->to->equal(std::string("pointer"));
                }
                );
            }
            );
            it(std::string("should handle long content with scrolling"), [=]() mutable
            {
                cy->get(std::string("[data-testid="chat-tab"]"))->click();
                for (auto i = 0; i < 20; i++)
                {
                    cy->get(std::string("[data-testid="message-input"]"))->type(std::string("Test message ") + i + std::string("{enter}"));
                    cy->wait(100);
                }
                cy->get(std::string("[data-testid="chat-messages"]"))->then([=](auto $el) mutable
                {
                    auto scrollHeight = const_($el)[0]["scrollHeight"];
                    auto clientHeight = const_($el)[0]["clientHeight"];
                    if (scrollHeight > clientHeight) {
                        cy->log(std::string("✅ Content is scrollable"));
                        cy->get(std::string("[data-testid="chat-messages"]"))->scrollTo(std::string("bottom"));
                    }
                }
                );
            }
            );
        }
        );
        describe(std::string("Error States"), [=]() mutable
        {
            it(std::string("should handle backend connection errors gracefully"), [=]() mutable
            {
                cy->get(std::string("[data-testid="goals-tab"]"))->click();
                cy->get(std::string("[data-testid="goals-content"]"))->should(std::string("be.visible"));
            }
            );
            it(std::string("should show loading states"), [=]() mutable
            {
                auto tabs = array<string>{ std::string("goals"), std::string("todos"), std::string("monologue") };
                tabs->forEach([=](auto tab) mutable
                {
                    cy->get(std::string("[data-testid="") + tab + std::string("-tab"]"))->click();
                    cy->get(std::string("[data-testid="") + tab + std::string("-content"]"))->within([=]() mutable
                    {
                        cy->get(std::string(".loading, [data-testid="loading"], .spinner"))->then([=](auto $loading) mutable
                        {
                            if ($loading["length"] > 0) {
                                cy->log(std::string("✅ ") + tab + std::string(" shows loading state"));
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
    describe(std::string("UI Navigation Summary"), [=]() mutable
    {
        it(std::string("should verify complete UI functionality"), [=]() mutable
        {
            auto tabs = array<string>{ std::string("goals"), std::string("todos"), std::string("monologue"), std::string("files"), std::string("config"), std::string("logs"), std::string("agent-screen") };
            tabs->forEach([=](auto tab) mutable
            {
                cy->get(std::string("[data-testid="") + tab + std::string("-tab"]"))->should(std::string("be.visible"));
            }
            );
            tabs->forEach([=](auto tab) mutable
            {
                cy->get(std::string("[data-testid="") + tab + std::string("-tab"]"))->click();
                cy->wait(300);
                cy->get(std::string("[data-testid="") + tab + std::string("-content"]"))->should(std::string("be.visible"));
            }
            );
            cy->get(std::string("[data-testid="message-input"]"))->should(std::string("be.visible"));
            cy->get(std::string("[data-testid="send-button"]"))->should(std::string("be.visible"));
            cy->get(std::string("[data-testid="output-container"]"))->should(std::string("be.visible"));
            cy->get(std::string("[data-testid="autonomy-toggle"]"))->should(std::string("be.visible"));
            cy->get(std::string("[data-testid="camera-toggle"]"))->should(std::string("be.visible"));
            cy->get(std::string("[data-testid="screen-toggle"]"))->should(std::string("be.visible"));
            cy->get(std::string("[data-testid="microphone-toggle"]"))->should(std::string("be.visible"));
            cy->get(std::string("[data-testid="shell-toggle"]"))->should(std::string("be.visible"));
            cy->get(std::string("[data-testid="browser-toggle"]"))->should(std::string("be.visible"));
            cy->get(std::string("[data-testid="game-interface"]"))->should(std::string("be.visible"));
            cy->get(std::string("[data-testid="connection-status"]"))->should(std::string("be.visible"));
            cy->log(std::string("✅ UI navigation verification complete"));
        }
        );
    }
    );
}

MAIN
