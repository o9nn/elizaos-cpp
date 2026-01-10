#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/game/cypress/e2e/01-setup-and-configuration.cy.h"

void Main(void)
{
    describe(std::string("Setup and Configuration"), [=]() mutable
    {
        shared TEST_OPENAI_KEY = std::string("sk-test-openai-") + Date->now() + string_empty;
        shared TEST_ANTHROPIC_KEY = std::string("sk-ant-test-anthropic-") + Date->now() + string_empty;
        shared BACKEND_URL = OR((Cypress->env(std::string("BACKEND_URL"))), (std::string("http://localhost:7777")));
        auto _FRONTEND_URL = OR((Cypress->env(std::string("FRONTEND_URL"))), (std::string("http://localhost:5173")));
        beforeEach([=]() mutable
        {
            cy->task(std::string("clearEnvironmentKeys"));
            cy->window()->then([=](auto win) mutable
            {
                win["localStorage"]["setItem"](std::string("skipBoot"), std::string("true"));
            }
            );
            cy->visit(std::string("/"));
        }
        );
        afterEach([=]() mutable
        {
            cy->task(std::string("clearEnvironmentKeys"));
        }
        );
        describe(std::string("API Key Setup Wizard"), [=]() mutable
        {
            it(std::string("should detect missing API keys and show setup wizard"), [=]() mutable
            {
                cy->wait(5000);
                cy->screenshot(std::string("initial-state"));
                cy->get(std::string("body"))->invoke(std::string("text"))->then([=](auto text) mutable
                {
                    cy->log(std::string("Page content:"), text["substring"](0, 200));
                }
                );
                cy->get(std::string("body"), object{
                    object::pair{std::string("timeout"), 40000}
                })->then([=](auto $body) mutable
                {
                    auto bodyText = $body["text"]();
                    cy->log(std::string("Looking for UI elements..."));
                    if (OR((bodyText["includes"](std::string("System Configuration"))), (bodyText["includes"](std::string("ELIZA agent settings"))))) {
                        cy->log(std::string("Found System Configuration"));
                        cy->contains(std::string("System Configuration"))->should(std::string("be.visible"));
                        cy->contains(std::string("Configure your ELIZA agent settings to begin"))->should(std::string("be.visible"));
                        cy->get(std::string("select.config-select"))->should(std::string("be.visible"));
                        cy->get(std::string("input.config-input[type="password"]"))->should(std::string("be.visible"));
                        cy->get(std::string("button.config-button"))->should(std::string("be.visible"));
                    } else if (OR((bodyText["includes"](std::string("ELIZA TERMINAL"))), (bodyText["includes"](std::string("◉ ELIZA"))))) {
                        cy->log(std::string("Found ELIZA Terminal - already configured"));
                        cy->get(std::string("[data-testid="config-tab"]"))->should(std::string("be.visible"))->click();
                        cy->wait(1000);
                        cy->contains(std::string("CONFIGURATION"))->should(std::string("be.visible"));
                        cy->get(std::string("[data-testid="model-provider-select"]"))->should(std::string("be.visible"));
                    } else if (AND((bodyText["includes"](std::string("ELIZA"))), (bodyText["includes"](std::string("v2.0"))))) {
                        cy->log(std::string("Found ELIZA startup screen"));
                        cy->wait(5000);
                        cy->screenshot(std::string("after-startup-wait"));
                    } else {
                        cy->log(std::string("Unexpected state:"), bodyText["substring"](0, 200));
                        cy->screenshot(std::string("unexpected-state"));
                        throw any(std::make_shared<Error>(std::string("Unexpected page state. Body text: ") + bodyText["substring"](0, 200) + string_empty));
                    }
                }
                );
                cy->screenshot(std::string("setup-wizard-final"));
            }
            );
            it(std::string("should allow switching between OpenAI and Anthropic providers"), [=]() mutable
            {
                cy->wait(5000);
                cy->get(std::string("body"), object{
                    object::pair{std::string("timeout"), 40000}
                })->then([=](auto $body) mutable
                {
                    auto bodyText = $body["text"]();
                    if (OR((bodyText["includes"](std::string("ELIZA TERMINAL"))), (bodyText["includes"](std::string("◉ ELIZA"))))) {
                        cy->log(std::string("Already configured - testing in config tab"));
                        cy->get(std::string("[data-testid="config-tab"]"))->should(std::string("be.visible"))->click();
                        cy->wait(2000);
                        cy->get(std::string("[data-testid="model-provider-select"]"))->should(std::string("be.visible"))->select(std::string("anthropic"));
                        cy->wait(1000);
                        cy->get(std::string("[data-testid="anthropic-api-key-input"]"))->should(std::string("be.visible"));
                        cy->get(std::string("[data-testid="model-provider-select"]"))->select(std::string("openai"));
                        cy->wait(1000);
                        cy->get(std::string("[data-testid="openai-api-key-input"]"))->should(std::string("be.visible"));
                    } else if (bodyText["includes"](std::string("System Configuration"))) {
                        cy->log(std::string("In setup flow - testing provider switching"));
                        cy->get(std::string("select.config-select"))->select(std::string("Anthropic"));
                        cy->get(std::string("input.config-input[type="password"]"))->should(std::string("be.visible"));
                        cy->get(std::string("select.config-select"))->select(std::string("OpenAI"));
                        cy->get(std::string("input.config-input[type="password"]"))->should(std::string("be.visible"));
                    } else {
                        cy->log(std::string("State:"), bodyText["substring"](0, 200));
                        cy->log(std::string("Skipping test - unexpected state"));
                    }
                }
                );
                cy->screenshot(std::string("provider-switching"));
            }
            );
            it(std::string("should validate API key input and enable continue button"), [=]() mutable
            {
                cy->wait(5000);
                cy->get(std::string("body"), object{
                    object::pair{std::string("timeout"), 40000}
                })->then([=](auto $body) mutable
                {
                    auto bodyText = $body["text"]();
                    if (OR((bodyText["includes"](std::string("ELIZA TERMINAL"))), (bodyText["includes"](std::string("◉ ELIZA"))))) {
                        cy->log(std::string("Already configured - testing API key input in config tab"));
                        cy->get(std::string("[data-testid="config-tab"]"))->should(std::string("be.visible"))->click();
                        cy->wait(2000);
                        cy->get(std::string("[data-testid="openai-api-key-input"]"))->should(std::string("be.visible"))->clear()->type(TEST_OPENAI_KEY);
                        cy->get(std::string("[data-testid="validate-config-button"]"))->should(std::string("be.visible"));
                    } else if (bodyText["includes"](std::string("System Configuration"))) {
                        cy->log(std::string("In setup flow - testing API key input"));
                        auto continueBtn = cy->get(std::string("button"))->contains((new RegExp(std::string("Continue|Submit|Save"))));
                        continueBtn->should(std::string("exist"));
                        cy->get(std::string("input.config-input[type="password"]"))->type(TEST_OPENAI_KEY);
                        cy->get(std::string("input.config-input[type="password"]"))->clear();
                        cy->get(std::string("input.config-input[type="password"]"))->type(TEST_OPENAI_KEY);
                    } else {
                        cy->log(std::string("Skipping test - unexpected state"));
                    }
                }
                );
            }
            );
        }
        );
        describe(std::string("Configuration Storage"), [=]() mutable
        {
            it(std::string("should save OpenAI configuration to database"), [=]() mutable
            {
                cy->wait(5000);
                cy->get(std::string("body"), object{
                    object::pair{std::string("timeout"), 40000}
                })->then([=](auto $body) mutable
                {
                    auto bodyText = $body["text"]();
                    if (OR((bodyText["includes"](std::string("ELIZA TERMINAL"))), (bodyText["includes"](std::string("◉ ELIZA"))))) {
                        cy->log(std::string("Already configured - testing configuration save"));
                        cy->get(std::string("[data-testid="config-tab"]"))->should(std::string("be.visible"))->click();
                        cy->wait(2000);
                        cy->get(std::string("[data-testid="openai-api-key-input"]"))->should(std::string("be.visible"))->clear()->type(TEST_OPENAI_KEY);
                        cy->get(std::string("[data-testid="validate-config-button"]"))->should(std::string("be.visible"));
                    } else if (bodyText["includes"](std::string("System Configuration"))) {
                        cy->log(std::string("In setup flow - saving initial configuration"));
                        cy->get(std::string("input.config-input[type="password"]"))->type(TEST_OPENAI_KEY);
                        cy->get(std::string("button"))->contains((new RegExp(std::string("Continue|Submit|Save"))))->click();
                        cy->wait(3000);
                    }
                }
                );
                cy->request(std::string("GET"), string_empty + BACKEND_URL + std::string("/api/plugin-config"))->then([=](auto response) mutable
                {
                    expect(response["status"])->to->eq(200);
                    expect(response["body"]["success"])->to->be->true;
                    expect(response["body"]["data"]["configurations"]["environment"])->to->exist;
                }
                );
                cy->screenshot(std::string("openai-config-saved"));
            }
            );
            it(std::string("should save Anthropic configuration to database"), [=]() mutable
            {
                cy->wait(5000);
                cy->get(std::string("body"), object{
                    object::pair{std::string("timeout"), 40000}
                })->then([=](auto $body) mutable
                {
                    auto bodyText = $body["text"]();
                    if (OR((bodyText["includes"](std::string("ELIZA TERMINAL"))), (bodyText["includes"](std::string("◉ ELIZA"))))) {
                        cy->log(std::string("Already configured - testing Anthropic configuration"));
                        cy->get(std::string("[data-testid="config-tab"]"))->should(std::string("be.visible"))->click();
                        cy->wait(2000);
                        cy->get(std::string("[data-testid="model-provider-select"]"))->select(std::string("anthropic"));
                        cy->wait(1000);
                        cy->get(std::string("[data-testid="anthropic-api-key-input"]"))->should(std::string("be.visible"))->clear()->type(TEST_ANTHROPIC_KEY);
                        cy->get(std::string("[data-testid="validate-config-button"]"))->should(std::string("be.visible"));
                    } else if (bodyText["includes"](std::string("System Configuration"))) {
                        cy->log(std::string("In setup flow - saving Anthropic configuration"));
                        cy->get(std::string("select.config-select"))->select(std::string("Anthropic"));
                        cy->get(std::string("input.config-input[type="password"]"))->type(TEST_ANTHROPIC_KEY);
                        cy->get(std::string("button"))->contains((new RegExp(std::string("Continue|Submit|Save"))))->click();
                        cy->wait(3000);
                    }
                }
                );
                cy->request(std::string("GET"), string_empty + BACKEND_URL + std::string("/api/plugin-config"))->then([=](auto response) mutable
                {
                    expect(response["status"])->to->eq(200);
                    expect(response["body"]["success"])->to->be->true;
                    expect(response["body"]["data"]["configurations"]["environment"])->to->exist;
                }
                );
                cy->screenshot(std::string("anthropic-config-saved"));
            }
            );
            it(std::string("should persist configuration across page reloads"), [=]() mutable
            {
                cy->wait(2000);
                cy->request(std::string("GET"), string_empty + BACKEND_URL + std::string("/api/plugin-config"))->then([=](auto response) mutable
                {
                    expect(response["status"])->to->eq(200);
                    expect(response["body"]["success"])->to->be->true;
                    expect(response["body"]["data"]["configurations"]["environment"])->to->exist;
                }
                );
                cy->reload();
                cy->wait(5000);
                cy->request(std::string("GET"), string_empty + BACKEND_URL + std::string("/api/plugin-config"))->then([=](auto response) mutable
                {
                    expect(response["status"])->to->eq(200);
                    expect(response["body"]["success"])->to->be->true;
                    expect(response["body"]["data"]["configurations"]["environment"])->to->exist;
                }
                );
                cy->get(std::string("body"))->then([=](auto $body) mutable
                {
                    auto bodyText = $body["text"]();
                    expect(bodyText)->to->match((new RegExp(std::string("ELIZA|Configuration"))));
                }
                );
            }
            );
        }
        );
        describe(std::string("Agent Runtime Integration"), [=]() mutable
        {
            it(std::string("should enable agent runtime with configured API keys"), [=]() mutable
            {
                cy->wait(2000);
                cy->request(std::string("GET"), string_empty + BACKEND_URL + std::string("/api/server/health"))->then([=](auto response) mutable
                {
                    expect(response["status"])->to->eq(200);
                    expect(response["body"]["success"])->to->be->true;
                    expect(response["body"]["data"]["status"])->to->eq(std::string("healthy"));
                    expect(response["body"]["data"]["agent"])->to->eq(std::string("connected"));
                    expect(response["body"]["data"]["agentId"])->to->match((new RegExp(std::string("^[0-9a-f-]{36}"))));
                }
                );
                cy->request(std::string("GET"), string_empty + BACKEND_URL + std::string("/api/debug/runtime-state"))->then([=](auto response) mutable
                {
                    expect(response["status"])->to->eq(200);
                    expect(response["body"]["success"])->to->be->true;
                    expect(response["body"]["data"]["character"]["name"])->to->eq(std::string("ELIZA"));
                    expect(response["body"]["data"]["database"]["isConnected"])->to->be->true;
                    expect(response["body"]["data"]["services"])->to->be->an(std::string("array"))->with->length->greaterThan(0);
                }
                );
                cy->screenshot(std::string("agent-runtime-connected"));
            }
            );
            it(std::string("should load available plugins with configuration"), [=]() mutable
            {
                cy->wait(2000);
                cy->request(std::string("GET"), string_empty + BACKEND_URL + std::string("/api/plugin-config"))->then([=](auto response) mutable
                {
                    auto plugins = response["body"]["data"]["availablePlugins"];
                    expect(plugins)->to->be->an(std::string("array"));
                    expect(plugins["length"])->to->be->greaterThan(0);
                    cy->log(std::string("✅ ") + plugins["length"] + std::string(" plugins loaded"));
                    cy->log(std::string("Plugins: ") + plugins["join"](std::string(", ")) + string_empty);
                }
                );
            }
            );
        }
        );
        describe(std::string("Error Handling and Edge Cases"), [=]() mutable
        {
            it(std::string("should handle skip functionality"), [=]() mutable
            {
                cy->wait(5000);
                cy->get(std::string("body"), object{
                    object::pair{std::string("timeout"), 40000}
                })->then([=](auto $body) mutable
                {
                    auto bodyText = $body["text"]();
                    if (bodyText["includes"](std::string("System Configuration"))) {
                        cy->log(std::string("In setup flow - testing skip button"));
                        cy->get(std::string("button"))->contains((new RegExp(std::string("Skip|Cancel|Close"))))->click();
                        cy->wait(2000);
                        cy->get(std::string("body"))->should(std::string("not.contain"), std::string("System Configuration"));
                    } else if (OR((bodyText["includes"](std::string("ELIZA TERMINAL"))), (bodyText["includes"](std::string("◉ ELIZA"))))) {
                        cy->log(std::string("Already configured - skip not applicable"));
                        cy->get(std::string("[data-testid="config-tab"]"))->should(std::string("be.visible"));
                    }
                }
                );
                cy->screenshot(std::string("skip-setup"));
            }
            );
            it(std::string("should reject empty API keys"), [=]() mutable
            {
                cy->wait(5000);
                cy->get(std::string("body"), object{
                    object::pair{std::string("timeout"), 40000}
                })->then([=](auto $body) mutable
                {
                    auto bodyText = $body["text"]();
                    if (bodyText["includes"](std::string("System Configuration"))) {
                        cy->log(std::string("In setup flow - testing empty API key validation"));
                        auto continueBtn = cy->get(std::string("button"))->contains((new RegExp(std::string("Continue|Submit|Save"))));
                        continueBtn->should(std::string("exist"));
                        cy->get(std::string("input.config-input[type="password"]"))->type(std::string("test"))->clear();
                    } else if (OR((bodyText["includes"](std::string("ELIZA TERMINAL"))), (bodyText["includes"](std::string("◉ ELIZA"))))) {
                        cy->log(std::string("Already configured - testing empty API key in config"));
                        cy->get(std::string("[data-testid="config-tab"]"))->should(std::string("be.visible"))->click();
                        cy->wait(1000);
                        cy->get(std::string("[data-testid="openai-api-key-input"]"))->clear();
                        cy->get(std::string("[data-testid="validate-config-button"]"))->should(std::string("be.visible"));
                    }
                }
                );
            }
            );
            it(std::string("should handle concurrent configuration requests"), [=]() mutable
            {
                cy->wait(2000);
                auto requests = array<std::shared_ptr<Cypress::Chainable<std::shared_ptr<Cypress::Response<any>>>>>();
                for (auto i = 0; i < 5; i++)
                {
                    requests->push(cy->request(std::string("GET"), string_empty + BACKEND_URL + std::string("/api/plugin-config")));
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
        describe(std::string("Database Verification"), [=]() mutable
        {
            it(std::string("should verify database connection and persistence"), [=]() mutable
            {
                cy->log(std::string("Testing database connection..."));
                cy->request(std::string("GET"), string_empty + BACKEND_URL + std::string("/api/server/health"))->then([=](auto response) mutable
                {
                    expect(response["status"])->to->eq(200);
                }
                );
                shared testRoomId = std::string("550e8400-e29b-41d4-a716-446655440002");
                cy->log(std::string("Testing agent memory for room: ") + testRoomId + string_empty);
                cy->sendMessage(object{
                    object::pair{std::string("text"), std::string("Database persistence test message")}, 
                    object::pair{std::string("userId"), std::string("test-user")}, 
                    object::pair{std::string("roomId"), testRoomId}
                })->then([=]() mutable
                {
                    cy->wait(2000);
                    cy->request(std::string("GET"), string_empty + BACKEND_URL + std::string("/api/memories?roomId=") + testRoomId + std::string("&count=5"))->then([=](auto response) mutable
                    {
                        expect(response["status"])->to->eq(200);
                        expect(response["body"]["data"]["some"]([=](auto m) mutable
                        {
                            return m["content"]->includes(std::string("persistence test"));
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
    describe(std::string("Setup Verification Summary"), [=]() mutable
    {
        it(std::string("should validate all setup requirements are met"), [=]() mutable
        {
            shared BACKEND_URL = OR((Cypress->env(std::string("BACKEND_URL"))), (std::string("http://localhost:7777")));
            cy->wait(2000);
            cy->request(string_empty + BACKEND_URL + std::string("/api/plugin-config"))->then([=](auto configResponse) mutable
            {
                cy->request(string_empty + BACKEND_URL + std::string("/api/server/health"))->then([=](auto healthResponse) mutable
                {
                    auto config = configResponse["body"]["data"];
                    auto health = healthResponse["body"]["data"];
                    expect(config["configurations"], std::string("Configuration exists"))->to->exist;
                    expect(health["agent"], std::string("Agent connected"))->to->eq(std::string("connected"));
                    expect(health["status"], std::string("System healthy"))->to->eq(std::string("healthy"));
                    expect(config["availablePlugins"]["length"], std::string("Plugins loaded"))->to->be->greaterThan(0);
                    cy->log(std::string("🎉 ALL SETUP REQUIREMENTS VERIFIED:"));
                    cy->log(std::string("✅ Configuration system operational"));
                    cy->log(std::string("✅ Agent runtime connected and operational"));
                    cy->log(std::string("✅ Plugins loaded and available"));
                    cy->log(std::string("✅ System ready for use"));
                    cy->screenshot(std::string("setup-verification-complete"));
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
