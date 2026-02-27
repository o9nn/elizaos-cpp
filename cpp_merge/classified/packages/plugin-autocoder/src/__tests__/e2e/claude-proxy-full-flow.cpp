#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-autocoder/src/__tests__/e2e/claude-proxy-full-flow.test.h"

void Main(void)
{
    describe(std:("Claude Proxy Full Flow Tests"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> runtime;
        shared<string> testProjectsDir;
        beforeAll([=]() mutable
        {
            testProjectsDir = path->join(process->cwd(), std:(".test-generated-plugins-claude-proxy"));
            std::async([=]() { fs::mkdir(testProjectsDir, object{
                object::pair{std:("recursive"), true}
            }); });
            process->env->FORCE_BUNSQLITE = std:("true");
            process->env->DATABASE_PATH = std:("./.eliza/.test-claude-proxy");
            process->env->ELIZA_TEST_MODE = std:("true");
            process->env->SECRET_SALT = OR((process->env->SECRET_SALT), (std:("test-salt-for-testing-only-not-secure")));
            runtime = std::make_shared<AgentRuntime>(object{
                object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("character"), object{
                    object::pair{std:("name"), std:("Test Agent")}, 
                    object::pair{std:("bio"), array<string>{ std:("Test agent for claude proxy integration") }}, 
                    object::pair{std:("system"), std:("You are a helpful assistant")}, 
                    object::pair{std:("settings"), object{
                        object::pair{std:("USE_CLAUDE_PROXY"), std:("true")}, 
                        object::pair{std:("OPENAI_API_KEY"), OR((process->env->OPENAI_API_KEY), (std:("test-key")))}, 
                        object::pair{std:("ANTHROPIC_API_KEY"), OR((process->env->ANTHROPIC_API_KEY), (std:("test-key")))}
                    }}
                }}
            });
            std::async([=]() { runtime->registerPlugin(sqlPlugin); });
            std::async([=]() { runtime->registerPlugin(inferencePlugin); });
            std::async([=]() { runtime->registerPlugin(openaiPlugin); });
            std::async([=]() { runtime->registerPlugin(formsPlugin); });
            std::async([=]() { runtime->registerPlugin(autocoderPlugin); });
            std::async([=]() { runtime->initialize(); });
        }
        );
        it(std:("should complete full project generation flow with status updates"), [=]() mutable
        {
            auto hasApiKey = !!(OR((process->env->OPENAI_API_KEY), (process->env->ANTHROPIC_API_KEY)));
            if (!hasApiKey) {
                console->log(std:("Skipping test - no API key available"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime->getService(std:("code-generation"));
            auto statusManager = as<any>(runtime->getService(std:("project-status-manager")));
            expect(codeGenService)->toBeDefined();
            expect(statusManager)->toBeDefined();
            shared statusUpdates = array<any>();
            statusManager["on"](std:("update"), [=](auto update) mutable
            {
                statusUpdates->push(update);
                console->log(std:("Status Update: ") + update["status"] + std:(" - ") + update["message"] + string_empty);
            }
            );
            auto request = object{
                object::pair{std:("projectName"), std:("test-full-flow-plugin")}, 
                object::pair{std:("description"), std:("A plugin that demonstrates the full autocoder flow")}, 
                object::pair{std:("targetType"), as<std::shared_ptr<const>>(std:("plugin"))}, 
                object::pair{std:("requirements"), array<string>{ std:("Create an action that greets users by name"), std:("Include a provider that shows current time"), std:("Add proper TypeScript types"), std:("Include comprehensive tests") }}, 
                object::pair{std:("apis"), array<any>()}, 
                object::pair{std:("testScenarios"), array<string>{ std:("Should greet user with provided name"), std:("Should show current time in provider") }}
            };
            auto result = std::async([=]() { (as<any>(codeGenService))["generateCode"](request); });
            expect(result["success"])->toBe(true);
            expect(result["projectPath"])->toBeDefined();
            expect(result["files"])->toBeDefined();
            expect(result["files"]["length"])->toBeGreaterThan(0);
            expect(statusUpdates->get_length())->toBeGreaterThan(0);
            auto statuses = statusUpdates->map([=](auto u) mutable
            {
                return u["status"];
            }
            );
            expect(statuses)->toContain(std:("planning"));
            expect(statuses)->toContain(std:("generating"));
            expect(statuses)->toContain(std:("testing"));
            expect(statuses)->toContain(std:("completed"));
            auto projectPath = result["projectPath"];
            auto essentialFiles = array<string>{ std:("package.json"), std:("src/index.ts"), std:("tsconfig.json"), std:("README.md"), std:("PRD.json") };
            for (auto& file : essentialFiles)
            {
                auto filePath = path->join(projectPath, file);
                auto exists = std::async([=]() { fs::access(filePath)->then([=]() mutable
                {
                    return true;
                }
                )->_catch([=]() mutable
                {
                    return false;
                }
                ); });
                expect(exists)->toBe(true);
            }
            auto prdPath = path->join(projectPath, std:("PRD.json"));
            auto prdContent = std::async([=]() { fs::readFile(prdPath, std:("utf-8")); });
            auto prd = JSON->parse(prdContent);
            expect(prd["title"])->toBeDefined();
            expect(prd["architecture"])->toBeDefined();
            expect(prd["architecture"]["approach"])->toMatch((new RegExp(std:("^(clone-existing|extend-existing|new-plugin)"))));
            if (result["executionResults"]) {
                console->log(std:("Validation Results:"));
                console->log(std:("  Lint: ") + (result["executionResults"]["lintPass"]) ? std:("✅") : std:("❌") + string_empty);
                console->log(std:("  Types: ") + (result["executionResults"]["typesPass"]) ? std:("✅") : std:("❌") + string_empty);
                console->log(std:("  Tests: ") + (result["executionResults"]["testsPass"]) ? std:("✅") : std:("❌") + string_empty);
                console->log(std:("  Build: ") + (result["executionResults"]["buildPass"]) ? std:("✅") : std:("❌") + string_empty);
            }
        }
        );
        it(std:("should use Claude proxy when configured with OpenAI"), [=]() mutable
        {
            if (!process->env->OPENAI_API_KEY) {
                console->log(std:("Skipping test - no OpenAI API key"));
                return std::shared_ptr<Promise<void>>();
            }
            auto inferenceService = as<any>(runtime->getService(std:("inference")));
            expect(inferenceService)->toBeDefined();
            auto response = std::async([=]() { runtime->useModel(std:("text_large"), object{
                object::pair{std:("prompt"), std:("Generate a simple TypeScript function that adds two numbers")}, 
                object::pair{std:("temperature"), 0.7}, 
                object::pair{std:("max_tokens"), 200}
            }); });
            expect(response)->toBeDefined();
            expect(type_of(response))->toBe(std:("string"));
            expect(response->length)->toBeGreaterThan(0);
        }
        );
        it(std:("should handle research model type correctly"), [=]() mutable
        {
            auto hasApiKey = !!(OR((process->env->OPENAI_API_KEY), (process->env->ANTHROPIC_API_KEY)));
            if (!hasApiKey) {
                console->log(std:("Skipping test - no API key available"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime->getService(std:("code-generation"));
            auto request = object{
                object::pair{std:("projectName"), std:("test-research-integration")}, 
                object::pair{std:("description"), std:("A plugin that integrates with multiple APIs")}, 
                object::pair{std:("targetType"), as<std::shared_ptr<const>>(std:("plugin"))}, 
                object::pair{std:("requirements"), array<string>{ std:("Integrate with GitHub API for issue tracking"), std:("Use OpenAI for text generation"), std:("Implement caching with Redis") }}, 
                object::pair{std:("apis"), array<string>{ std:("GitHub"), std:("OpenAI"), std:("Redis") }}, 
                object::pair{std:("testScenarios"), array<string>{ std:("Test API integrations") }}
            };
            auto result = std::async([=]() { (as<any>(codeGenService))["generateCode"](request); });
            expect(result["success"])->toBe(true);
            auto prdPath = path->join(result["projectPath"], std:("PRD.json"));
            auto prdContent = std::async([=]() { fs::readFile(prdPath, std:("utf-8")); });
            auto prd = JSON->parse(prdContent);
            expect(prd["apiKeys"])->toBeDefined();
            expect(prd["apiKeys"]["length"])->toBeGreaterThan(0);
        }
        );
        it(std:("should track project history correctly"), [=]() mutable
        {
            auto statusManager = as<any>(runtime->getService(std:("project-status-manager")));
            auto summary = statusManager["getStatusSummary"]();
            expect(summary)->toBeDefined();
            expect(summary["active"])->toBeGreaterThanOrEqual(0);
            expect(summary["completed"])->toBeGreaterThanOrEqual(0);
            auto history = statusManager["getHistory"]();
            expect(Array->isArray(history))->toBe(true);
            if (history["length"] > 0) {
                auto project = const_(history)[0];
                expect(project["id"])->toBeDefined();
                expect(project["name"])->toBeDefined();
                expect(project["status"])->toMatch((new RegExp(std:("^(completed|failed)"))));
                expect(project["startedAt"])->toBeDefined();
            }
        }
        );
        it(std:("should handle API key validation correctly"), [=]() mutable
        {
            auto codeGenService = runtime->getService(std:("code-generation"));
            auto request = object{
                object::pair{std:("projectName"), std:("test-api-key-validation")}, 
                object::pair{std:("description"), std:("A plugin that requires specific API keys")}, 
                object::pair{std:("targetType"), as<std::shared_ptr<const>>(std:("plugin"))}, 
                object::pair{std:("requirements"), array<string>{ std:("Use Stripe API for payments") }}, 
                object::pair{std:("apis"), array<string>{ std:("Stripe") }}, 
                object::pair{std:("testScenarios"), array<string>{ std:("Test payment processing") }}
            };
            auto result = std::async([=]() { (as<any>(codeGenService))["generateCode"](request); });
            if (!runtime->getSetting(std:("STRIPE_API_KEY"))) {
                expect(result["success"])->toBe(false);
                expect(result["errors"])->toBeDefined();
                expect(const_(result["errors"])[0])->toContain(std:("STRIPE_API_KEY"));
            } else {
                expect(result["success"])->toBe(true);
            }
        }
        );
        it(std:("should generate plugin with different approaches based on existing plugins"), [=]() mutable
        {
            auto hasApiKey = !!(OR((process->env->OPENAI_API_KEY), (process->env->ANTHROPIC_API_KEY)));
            if (!hasApiKey) {
                console->log(std:("Skipping test - no API key available"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime->getService(std:("code-generation"));
            auto newPluginRequest = object{
                object::pair{std:("projectName"), std:("test-unique-blockchain-plugin")}, 
                object::pair{std:("description"), std:("A plugin for blockchain transaction monitoring")}, 
                object::pair{std:("targetType"), as<std::shared_ptr<const>>(std:("plugin"))}, 
                object::pair{std:("requirements"), array<string>{ std:("Monitor Ethereum transactions"), std:("Alert on large transfers") }}, 
                object::pair{std:("apis"), array<string>{ std:("Ethereum") }}, 
                object::pair{std:("testScenarios"), array<string>{ std:("Test transaction monitoring") }}
            };
            auto newResult = std::async([=]() { (as<any>(codeGenService))["generateCode"](newPluginRequest); });
            expect(newResult["success"])->toBe(true);
            auto newPrd = JSON->parse(std::async([=]() { fs::readFile(path->join(newResult["projectPath"], std:("PRD.json")), std:("utf-8")); }));
            expect(newPrd["architecture"]["approach"])->toBe(std:("new-plugin"));
            auto extensionRequest = object{
                object::pair{std:("projectName"), std:("test-enhanced-shell-plugin")}, 
                object::pair{std:("description"), std:("A plugin that extends shell capabilities with advanced features")}, 
                object::pair{std:("targetType"), as<std::shared_ptr<const>>(std:("plugin"))}, 
                object::pair{std:("requirements"), array<string>{ std:("Execute shell commands"), std:("Add command history"), std:("Add command aliases") }}, 
                object::pair{std:("apis"), array<any>()}, 
                object::pair{std:("testScenarios"), array<string>{ std:("Test shell execution with history") }}
            };
            auto extResult = std::async([=]() { (as<any>(codeGenService))["generateCode"](extensionRequest); });
            expect(extResult["success"])->toBe(true);
            auto extPrd = JSON->parse(std::async([=]() { fs::readFile(path->join(extResult["projectPath"], std:("PRD.json")), std:("utf-8")); }));
            expect(array<string>{ std:("extend-existing"), std:("new-plugin") })->toContain(extPrd["architecture"]["approach"]);
        }
        );
        afterAll([=]() mutable
        {
            try
            {
                std::async([=]() { fs::rm(testProjectsDir, object{
                    object::pair{std:("recursive"), true}, 
                    object::pair{std:("force"), true}
                }); });
            }
            catch (const any& error)
            {
                console->error(std:("Failed to clean up test directory:"), error);
            }
        }
        );
    }
    );
}

MAIN
