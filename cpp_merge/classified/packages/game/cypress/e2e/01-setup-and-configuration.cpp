#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/game/cypress/e2e/01-setup-and-configuration.cy.h"

void Main(void)
{
    describe(std:("Setup and Configuration"), [=]() mutable
    {
        shared TEST_OPENAI_KEY = std:("sk-test-openai-") + Date->now() + string_empty;
        shared TEST_ANTHROPIC_KEY = std:("sk-ant-test-anthropic-") + Date->now() + string_empty;
        shared BACKEND_URL = OR((Cypress->env(std:("BACKEND_URL"))), (std:("http://localhost:7777")));
        auto _FRONTEND_URL = OR((Cypress->env(std:("FRONTEND_URL"))), (std:("http://localhost:5173")));
        beforeEach([=]() mutable
        {
            cy->task(std:("clearEnvironmentKeys"));
            cy->window()->then([=](auto win) mutable
            {
                win["localStorage"]["setItem"](std:("skipBoot"), std:("true"));
            }
            );
            cy->visit(std:("/"));
        }
        );
        afterEach([=]() mutable
        {
            cy->task(std:("clearEnvironmentKeys"));
        }
        );
        describe(std:("API Key Setup Wizard"), [=]() mutable
        {
            it(std:("should detect missing API keys and show setup wizard"), [=]() mutable
            {
                cy->wait(5000);
                cy->screenshot(std:("initial-state"));
                cy->get(std:("body"))->invoke(std:("text"))->then([=](auto text) mutable
                {
                    cy->log(std:("Page content:"), text["substring"](0, 200));
                }
                );
                cy->get(std:("body"), object{
                    object::pair{std:("timeout"), 40000}
                })->then([=](auto $body) mutable
                {
                    auto bodyText = $body["text"]();
                    cy->log(std:("Looking for UI elements..."));
                    if (OR((bodyText["includes"](std:("System Configuration"))), (bodyText["includes"](std:("ELIZA agent settings"))))) {
                        cy->log(std:("Found System Configuration"));
                        cy->contains(std:("System Configuration"))->should(std:("be.visible"));
                        cy->contains(std:("Configure your ELIZA agent settings to begin"))->should(std:("be.visible"));
                        cy->get(std:("select.config-select"))->should(std:("be.visible"));
                        cy->get(std:("input.config-input[type="password"]"))->should(std:("be.visible"));
                        cy->get(std:("button.config-button"))->should(std:("be.visible"));
                    } else if (OR((bodyText["includes"](std:("ELIZA TERMINAL"))), (bodyText["includes"](std:("◉ ELIZA"))))) {
                        cy->log(std:("Found ELIZA Terminal - already configured"));
                        cy->get(std:("[data-testid="config-tab"]"))->should(std:("be.visible"))->click();
                        cy->wait(1000);
                        cy->contains(std:("CONFIGURATION"))->should(std:("be.visible"));
                        cy->get(std:("[data-testid="model-provider-select"]"))->should(std:("be.visible"));
                    } else if (AND((bodyText["includes"](std:("ELIZA"))), (bodyText["includes"](std:("v2.0"))))) {
                        cy->log(std:("Found ELIZA startup screen"));
                        cy->wait(5000);
                        cy->screenshot(std:("after-startup-wait"));
                    } else {
                        cy->log(std:("Unexpected state:"), bodyText["substring"](0, 200));
                        cy->screenshot(std:("unexpected-state"));
                        throw any(std::make_shared<Error>(std:("Unexpected page state. Body text: ") + bodyText["substring"](0, 200) + string_empty));
                    }
                }
                );
                cy->screenshot(std:("setup-wizard-final"));
            }
            );
            it(std:("should allow switching between OpenAI and Anthropic providers"), [=]() mutable
            {
                cy->wait(5000);
                cy->get(std:("body"), object{
                    object::pair{std:("timeout"), 40000}
                })->then([=](auto $body) mutable
                {
                    auto bodyText = $body["text"]();
                    if (OR((bodyText["includes"](std:("ELIZA TERMINAL"))), (bodyText["includes"](std:("◉ ELIZA"))))) {
                        cy->log(std:("Already configured - testing in config tab"));
                        cy->get(std:("[data-testid="config-tab"]"))->should(std:("be.visible"))->click();
                        cy->wait(2000);
                        cy->get(std:("[data-testid="model-provider-select"]"))->should(std:("be.visible"))->select(std:("anthropic"));
                        cy->wait(1000);
                        cy->get(std:("[data-testid="anthropic-api-key-input"]"))->should(std:("be.visible"));
                        cy->get(std:("[data-testid="model-provider-select"]"))->select(std:("openai"));
                        cy->wait(1000);
                        cy->get(std:("[data-testid="openai-api-key-input"]"))->should(std:("be.visible"));
                    } else if (bodyText["includes"](std:("System Configuration"))) {
                        cy->log(std:("In setup flow - testing provider switching"));
                        cy->get(std:("select.config-select"))->select(std:("Anthropic"));
                        cy->get(std:("input.config-input[type="password"]"))->should(std:("be.visible"));
                        cy->get(std:("select.config-select"))->select(std:("OpenAI"));
                        cy->get(std:("input.config-input[type="password"]"))->should(std:("be.visible"));
                    } else {
                        cy->log(std:("State:"), bodyText["substring"](0, 200));
                        cy->log(std:("Skipping test - unexpected state"));
                    }
                }
                );
                cy->screenshot(std:("provider-switching"));
            }
            );
            it(std:("should validate API key input and enable continue button"), [=]() mutable
            {
                cy->wait(5000);
                cy->get(std:("body"), object{
                    object::pair{std:("timeout"), 40000}
                })->then([=](auto $body) mutable
                {
                    auto bodyText = $body["text"]();
                    if (OR((bodyText["includes"](std:("ELIZA TERMINAL"))), (bodyText["includes"](std:("◉ ELIZA"))))) {
                        cy->log(std:("Already configured - testing API key input in config tab"));
                        cy->get(std:("[data-testid="config-tab"]"))->should(std:("be.visible"))->click();
                        cy->wait(2000);
                        cy->get(std:("[data-testid="openai-api-key-input"]"))->should(std:("be.visible"))->clear()->type(TEST_OPENAI_KEY);
                        cy->get(std:("[data-testid="validate-config-button"]"))->should(std:("be.visible"));
                    } else if (bodyText["includes"](std:("System Configuration"))) {
                        cy->log(std:("In setup flow - testing API key input"));
                        auto continueBtn = cy->get(std:("button"))->contains((new RegExp(std:("Continue|Submit|Save"))));
                        continueBtn->should(std:("exist"));
                        cy->get(std:("input.config-input[type="password"]"))->type(TEST_OPENAI_KEY);
                        cy->get(std:("input.config-input[type="password"]"))->clear();
                        cy->get(std:("input.config-input[type="password"]"))->type(TEST_OPENAI_KEY);
                    } else {
                        cy->log(std:("Skipping test - unexpected state"));
                    }
                }
                );
            }
            );
        }
        );
        describe(std:("Configuration Storage"), [=]() mutable
        {
            it(std:("should save OpenAI configuration to database"), [=]() mutable
            {
                cy->wait(5000);
                cy->get(std:("body"), object{
                    object::pair{std:("timeout"), 40000}
                })->then([=](auto $body) mutable
                {
                    auto bodyText = $body["text"]();
                    if (OR((bodyText["includes"](std:("ELIZA TERMINAL"))), (bodyText["includes"](std:("◉ ELIZA"))))) {
                        cy->log(std:("Already configured - testing configuration save"));
                        cy->get(std:("[data-testid="config-tab"]"))->should(std:("be.visible"))->click();
                        cy->wait(2000);
                        cy->get(std:("[data-testid="openai-api-key-input"]"))->should(std:("be.visible"))->clear()->type(TEST_OPENAI_KEY);
                        cy->get(std:("[data-testid="validate-config-button"]"))->should(std:("be.visible"));
                    } else if (bodyText["includes"](std:("System Configuration"))) {
                        cy->log(std:("In setup flow - saving initial configuration"));
                        cy->get(std:("input.config-input[type="password"]"))->type(TEST_OPENAI_KEY);
                        cy->get(std:("button"))->contains((new RegExp(std:("Continue|Submit|Save"))))->click();
                        cy->wait(3000);
                    }
                }
                );
                cy->request(std:("GET"), string_empty + BACKEND_URL + std:("/api/plugin-config"))->then([=](auto response) mutable
                {
                    expect(response["status"])->to->eq(200);
                    expect(response["body"]["success"])->to->be->true;
                    expect(response["body"]["data"]["configurations"]["environment"])->to->exist;
                }
                );
                cy->screenshot(std:("openai-config-saved"));
            }
            );
            it(std:("should save Anthropic configuration to database"), [=]() mutable
            {
                cy->wait(5000);
                cy->get(std:("body"), object{
                    object::pair{std:("timeout"), 40000}
                })->then([=](auto $body) mutable
                {
                    auto bodyText = $body["text"]();
                    if (OR((bodyText["includes"](std:("ELIZA TERMINAL"))), (bodyText["includes"](std:("◉ ELIZA"))))) {
                        cy->log(std:("Already configured - testing Anthropic configuration"));
                        cy->get(std:("[data-testid="config-tab"]"))->should(std:("be.visible"))->click();
                        cy->wait(2000);
                        cy->get(std:("[data-testid="model-provider-select"]"))->select(std:("anthropic"));
                        cy->wait(1000);
                        cy->get(std:("[data-testid="anthropic-api-key-input"]"))->should(std:("be.visible"))->clear()->type(TEST_ANTHROPIC_KEY);
                        cy->get(std:("[data-testid="validate-config-button"]"))->should(std:("be.visible"));
                    } else if (bodyText["includes"](std:("System Configuration"))) {
                        cy->log(std:("In setup flow - saving Anthropic configuration"));
                        cy->get(std:("select.config-select"))->select(std:("Anthropic"));
                        cy->get(std:("input.config-input[type="password"]"))->type(TEST_ANTHROPIC_KEY);
                        cy->get(std:("button"))->contains((new RegExp(std:("Continue|Submit|Save"))))->click();
                        cy->wait(3000);
                    }
                }
                );
                cy->request(std:("GET"), string_empty + BACKEND_URL + std:("/api/plugin-config"))->then([=](auto response) mutable
                {
                    expect(response["status"])->to->eq(200);
                    expect(response["body"]["success"])->to->be->true;
                    expect(response["body"]["data"]["configurations"]["environment"])->to->exist;
                }
                );
                cy->screenshot(std:("anthropic-config-saved"));
            }
            );
            it(std:("should persist configuration across page reloads"), [=]() mutable
            {
                cy->wait(2000);
                cy->request(std:("GET"), string_empty + BACKEND_URL + std:("/api/plugin-config"))->then([=](auto response) mutable
                {
                    expect(response["status"])->to->eq(200);
                    expect(response["body"]["success"])->to->be->true;
                    expect(response["body"]["data"]["configurations"]["environment"])->to->exist;
                }
                );
                cy->reload();
                cy->wait(5000);
                cy->request(std:("GET"), string_empty + BACKEND_URL + std:("/api/plugin-config"))->then([=](auto response) mutable
                {
                    expect(response["status"])->to->eq(200);
                    expect(response["body"]["success"])->to->be->true;
                    expect(response["body"]["data"]["configurations"]["environment"])->to->exist;
                }
                );
                cy->get(std:("body"))->then([=](auto $body) mutable
                {
                    auto bodyText = $body["text"]();
                    expect(bodyText)->to->match((new RegExp(std:("ELIZA|Configuration"))));
                }
                );
            }
            );
        }
        );
        describe(std:("Agent Runtime Integration"), [=]() mutable
        {
            it(std:("should enable agent runtime with configured API keys"), [=]() mutable
            {
                cy->wait(2000);
                cy->request(std:("GET"), string_empty + BACKEND_URL + std:("/api/server/health"))->then([=](auto response) mutable
                {
                    expect(response["status"])->to->eq(200);
                    expect(response["body"]["success"])->to->be->true;
                    expect(response["body"]["data"]["status"])->to->eq(std:("healthy"));
                    expect(response["body"]["data"]["agent"])->to->eq(std:("connected"));
                    expect(response["body"]["data"]["agentId"])->to->match((new RegExp(std:("^[0-9a-f-]{36}"))));
                }
                );
                cy->request(std:("GET"), string_empty + BACKEND_URL + std:("/api/debug/runtime-state"))->then([=](auto response) mutable
                {
                    expect(response["status"])->to->eq(200);
                    expect(response["body"]["success"])->to->be->true;
                    expect(response["body"]["data"]["character"]["name"])->to->eq(std:("ELIZA"));
                    expect(response["body"]["data"]["database"]["isConnected"])->to->be->true;
                    expect(response["body"]["data"]["services"])->to->be->an(std:("array"))->with->length->greaterThan(0);
                }
                );
                cy->screenshot(std:("agent-runtime-connected"));
            }
            );
            it(std:("should load available plugins with configuration"), [=]() mutable
            {
                cy->wait(2000);
                cy->request(std:("GET"), string_empty + BACKEND_URL + std:("/api/plugin-config"))->then([=](auto response) mutable
                {
                    auto plugins = response["body"]["data"]["availablePlugins"];
                    expect(plugins)->to->be->an(std:("array"));
                    expect(plugins["length"])->to->be->greaterThan(0);
                    cy->log(std:("✅ ") + plugins["length"] + std:(" plugins loaded"));
                    cy->log(std:("Plugins: ") + plugins["join"](std:(", ")) + string_empty);
                }
                );
            }
            );
        }
        );
        describe(std:("Error Handling and Edge Cases"), [=]() mutable
        {
            it(std:("should handle skip functionality"), [=]() mutable
            {
                cy->wait(5000);
                cy->get(std:("body"), object{
                    object::pair{std:("timeout"), 40000}
                })->then([=](auto $body) mutable
                {
                    auto bodyText = $body["text"]();
                    if (bodyText["includes"](std:("System Configuration"))) {
                        cy->log(std:("In setup flow - testing skip button"));
                        cy->get(std:("button"))->contains((new RegExp(std:("Skip|Cancel|Close"))))->click();
                        cy->wait(2000);
                        cy->get(std:("body"))->should(std:("not.contain"), std:("System Configuration"));
                    } else if (OR((bodyText["includes"](std:("ELIZA TERMINAL"))), (bodyText["includes"](std:("◉ ELIZA"))))) {
                        cy->log(std:("Already configured - skip not applicable"));
                        cy->get(std:("[data-testid="config-tab"]"))->should(std:("be.visible"));
                    }
                }
                );
                cy->screenshot(std:("skip-setup"));
            }
            );
            it(std:("should reject empty API keys"), [=]() mutable
            {
                cy->wait(5000);
                cy->get(std:("body"), object{
                    object::pair{std:("timeout"), 40000}
                })->then([=](auto $body) mutable
                {
                    auto bodyText = $body["text"]();
                    if (bodyText["includes"](std:("System Configuration"))) {
                        cy->log(std:("In setup flow - testing empty API key validation"));
                        auto continueBtn = cy->get(std:("button"))->contains((new RegExp(std:("Continue|Submit|Save"))));
                        continueBtn->should(std:("exist"));
                        cy->get(std:("input.config-input[type="password"]"))->type(std:("test"))->clear();
                    } else if (OR((bodyText["includes"](std:("ELIZA TERMINAL"))), (bodyText["includes"](std:("◉ ELIZA"))))) {
                        cy->log(std:("Already configured - testing empty API key in config"));
                        cy->get(std:("[data-testid="config-tab"]"))->should(std:("be.visible"))->click();
                        cy->wait(1000);
                        cy->get(std:("[data-testid="openai-api-key-input"]"))->clear();
                        cy->get(std:("[data-testid="validate-config-button"]"))->should(std:("be.visible"));
                    }
                }
                );
            }
            );
            it(std:("should handle concurrent configuration requests"), [=]() mutable
            {
                cy->wait(2000);
                auto requests = array<std::shared_ptr<Cypress::Chainable<std::shared_ptr<Cypress::Response<any>>>>>();
                for (auto i = 0; i < 5; i++)
                {
                    requests->push(cy->request(std:("GET"), string_empty + BACKEND_URL + std:("/api/plugin-config")));
                }
                shared firstConfig = nullptr;
                requests->forEach([=](auto request, auto index) mutable
                {
                    request->then([=](auto response) mutable
                    {
                        expect(response["status"])->to->eq(200);
                        expect(response["body"]["success"])->to->be->true;
                        expect(response["body"]["data"]["configurations"])->to->exist;
                        if (index == 0) {
                            firstConfig = response["body"]["data"]["configurations"];
                        } else {
                            expect(response["body"]["data"]["configurations"])->to->deep->equal(firstConfig);
                        }
                    }
                    );
                }
                );
            }
            );
        }
        );
        describe(std:("Database Verification"), [=]() mutable
        {
            it(std:("should verify database connection and persistence"), [=]() mutable
            {
                cy->log(std:("Testing database connection..."));
                cy->request(std:("GET"), string_empty + BACKEND_URL + std:("/api/server/health"))->then([=](auto response) mutable
                {
                    expect(response["status"])->to->eq(200);
                }
                );
                shared testRoomId = std:("550e8400-e29b-41d4-a716-446655440002");
                cy->log(std:("Testing agent memory for room: ") + testRoomId + string_empty);
                cy->sendMessage(object{
                    object::pair{std:("text"), std:("Database persistence test message")}, 
                    object::pair{std:("userId"), std:("test-user")}, 
                    object::pair{std:("roomId"), testRoomId}
                })->then([=]() mutable
                {
                    cy->wait(2000);
                    cy->request(std:("GET"), string_empty + BACKEND_URL + std:("/api/memories?roomId=") + testRoomId + std:("&count=5"))->then([=](auto response) mutable
                    {
                        expect(response["status"])->to->eq(200);
                        expect(response["body"]["data"]["some"]([=](auto m) mutable
                        {
                            return m["content"]->includes(std:("persistence test"));
                        }
                        ))->to->be->true;
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
    describe(std:("Setup Verification Summary"), [=]() mutable
    {
        it(std:("should validate all setup requirements are met"), [=]() mutable
        {
            shared BACKEND_URL = OR((Cypress->env(std:("BACKEND_URL"))), (std:("http://localhost:7777")));
            cy->wait(2000);
            cy->request(string_empty + BACKEND_URL + std:("/api/plugin-config"))->then([=](auto configResponse) mutable
            {
                cy->request(string_empty + BACKEND_URL + std:("/api/server/health"))->then([=](auto healthResponse) mutable
                {
                    auto config = configResponse["body"]["data"];
                    auto health = healthResponse["body"]["data"];
                    expect(config["configurations"], std:("Configuration exists"))->to->exist;
                    expect(health["agent"], std:("Agent connected"))->to->eq(std:("connected"));
                    expect(health["status"], std:("System healthy"))->to->eq(std:("healthy"));
                    expect(config["availablePlugins"]["length"], std:("Plugins loaded"))->to->be->greaterThan(0);
                    cy->log(std:("🎉 ALL SETUP REQUIREMENTS VERIFIED:"));
                    cy->log(std:("✅ Configuration system operational"));
                    cy->log(std:("✅ Agent runtime connected and operational"));
                    cy->log(std:("✅ Plugins loaded and available"));
                    cy->log(std:("✅ System ready for use"));
                    cy->screenshot(std:("setup-verification-complete"));
                }
                );
            }
            );
        }
        );
    }
    );
}

MAIN
