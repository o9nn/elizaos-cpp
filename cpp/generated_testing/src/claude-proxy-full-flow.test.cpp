#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-autocoder/src/__tests__/e2e/claude-proxy-full-flow.test.h"

void Main(void)
{
    describe(std::string("Claude Proxy Full Flow Tests"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> runtime;
        shared<string> testProjectsDir;
        beforeAll([=]() mutable
        {
            testProjectsDir = path->join(process->cwd(), std::string(".test-generated-plugins-claude-proxy"));
            std::async([=]() { fs::mkdir(testProjectsDir, object{
                object::pair{std::string("recursive"), true}
            }); });
            process->env->FORCE_BUNSQLITE = std::string("true");
            process->env->DATABASE_PATH = std::string("./.eliza/.test-claude-proxy");
            process->env->ELIZA_TEST_MODE = std::string("true");
            process->env->SECRET_SALT = OR((process->env->SECRET_SALT), (std::string("test-salt-for-testing-only-not-secure")));
            runtime = std::make_shared<AgentRuntime>(object{
                object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("character"), object{
                    object::pair{std::string("name"), std::string("Test Agent")}, 
                    object::pair{std::string("bio"), array<string>{ std::string("Test agent for claude proxy integration") }}, 
                    object::pair{std::string("system"), std::string("You are a helpful assistant")}, 
                    object::pair{std::string("settings"), object{
                        object::pair{std::string("USE_CLAUDE_PROXY"), std::string("true")}, 
                        object::pair{std::string("OPENAI_API_KEY"), OR((process->env->OPENAI_API_KEY), (std::string("test-key")))}, 
                        object::pair{std::string("ANTHROPIC_API_KEY"), OR((process->env->ANTHROPIC_API_KEY), (std::string("test-key")))}
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
        it(std::string("should complete full project generation flow with status updates"), [=]() mutable
        {
            auto hasApiKey = !!(OR((process->env->OPENAI_API_KEY), (process->env->ANTHROPIC_API_KEY)));
            if (!hasApiKey) {
                console->log(std::string("Skipping test - no API key available"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime->getService(std::string("code-generation"));
            auto statusManager = as<any>(runtime->getService(std::string("project-status-manager")));
            expect(codeGenService)->toBeDefined();
            expect(statusManager)->toBeDefined();
            shared statusUpdates = array<any>();
            statusManager["on"](std::string("update"), [=](auto update) mutable
            {
                statusUpdates->push(update);
                console->log(std::string("Status Update: ") + update["status"] + std::string(" - ") + update["message"] + string_empty);
            }
            );
            auto request = object{
                object::pair{std::string("projectName"), std::string("test-full-flow-plugin")}, 
                object::pair{std::string("description"), std::string("A plugin that demonstrates the full autocoder flow")}, 
                object::pair{std::string("targetType"), as<std::shared_ptr<const>>(std::string("plugin"))}, 
                object::pair{std::string("requirements"), array<string>{ std::string("Create an action that greets users by name"), std::string("Include a provider that shows current time"), std::string("Add proper TypeScript types"), std::string("Include comprehensive tests") }}, 
                object::pair{std::string("apis"), array<any>()}, 
                object::pair{std::string("testScenarios"), array<string>{ std::string("Should greet user with provided name"), std::string("Should show current time in provider") }}
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
            expect(statuses)->toContain(std::string("planning"));
            expect(statuses)->toContain(std::string("generating"));
            expect(statuses)->toContain(std::string("testing"));
            expect(statuses)->toContain(std::string("completed"));
            auto projectPath = result["projectPath"];
            auto essentialFiles = array<string>{ std::string("package.json"), std::string("src/index.ts"), std::string("tsconfig.json"), std::string("README.md"), std::string("PRD.json") };
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
            auto prdPath = path->join(projectPath, std::string("PRD.json"));
            auto prdContent = std::async([=]() { fs::readFile(prdPath, std::string("utf-8")); });
            auto prd = JSON->parse(prdContent);
            expect(prd["title"])->toBeDefined();
            expect(prd["architecture"])->toBeDefined();
            expect(prd["architecture"]["approach"])->toMatch((new RegExp(std::string("^(clone-existing|extend-existing|new-plugin)"))));
            if (result["executionResults"]) {
                console->log(std::string("Validation Results:"));
                console->log(std::string("  Lint: ") + (result["executionResults"]["lintPass"]) ? std::string("✅") : std::string("❌") + string_empty);
                console->log(std::string("  Types: ") + (result["executionResults"]["typesPass"]) ? std::string("✅") : std::string("❌") + string_empty);
                console->log(std::string("  Tests: ") + (result["executionResults"]["testsPass"]) ? std::string("✅") : std::string("❌") + string_empty);
                console->log(std::string("  Build: ") + (result["executionResults"]["buildPass"]) ? std::string("✅") : std::string("❌") + string_empty);
            }
        }
        );
        it(std::string("should use Claude proxy when configured with OpenAI"), [=]() mutable
        {
            if (!process->env->OPENAI_API_KEY) {
                console->log(std::string("Skipping test - no OpenAI API key"));
                return std::shared_ptr<Promise<void>>();
            }
            auto inferenceService = as<any>(runtime->getService(std::string("inference")));
            expect(inferenceService)->toBeDefined();
            auto response = std::async([=]() { runtime->useModel(std::string("text_large"), object{
                object::pair{std::string("prompt"), std::string("Generate a simple TypeScript function that adds two numbers")}, 
                object::pair{std::string("temperature"), 0.7}, 
                object::pair{std::string("max_tokens"), 200}
            }); });
            expect(response)->toBeDefined();
            expect(type_of(response))->toBe(std::string("string"));
            expect(response->length)->toBeGreaterThan(0);
        }
        );
        it(std::string("should handle research model type correctly"), [=]() mutable
        {
            auto hasApiKey = !!(OR((process->env->OPENAI_API_KEY), (process->env->ANTHROPIC_API_KEY)));
            if (!hasApiKey) {
                console->log(std::string("Skipping test - no API key available"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime->getService(std::string("code-generation"));
            auto request = object{
                object::pair{std::string("projectName"), std::string("test-research-integration")}, 
                object::pair{std::string("description"), std::string("A plugin that integrates with multiple APIs")}, 
                object::pair{std::string("targetType"), as<std::shared_ptr<const>>(std::string("plugin"))}, 
                object::pair{std::string("requirements"), array<string>{ std::string("Integrate with GitHub API for issue tracking"), std::string("Use OpenAI for text generation"), std::string("Implement caching with Redis") }}, 
                object::pair{std::string("apis"), array<string>{ std::string("GitHub"), std::string("OpenAI"), std::string("Redis") }}, 
                object::pair{std::string("testScenarios"), array<string>{ std::string("Test API integrations") }}
            };
            auto result = std::async([=]() { (as<any>(codeGenService))["generateCode"](request); });
            expect(result["success"])->toBe(true);
            auto prdPath = path->join(result["projectPath"], std::string("PRD.json"));
            auto prdContent = std::async([=]() { fs::readFile(prdPath, std::string("utf-8")); });
            auto prd = JSON->parse(prdContent);
            expect(prd["apiKeys"])->toBeDefined();
            expect(prd["apiKeys"]["length"])->toBeGreaterThan(0);
        }
        );
        it(std::string("should track project history correctly"), [=]() mutable
        {
            auto statusManager = as<any>(runtime->getService(std::string("project-status-manager")));
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
                expect(project["status"])->toMatch((new RegExp(std::string("^(completed|failed)"))));
                expect(project["startedAt"])->toBeDefined();
            }
        }
        );
        it(std::string("should handle API key validation correctly"), [=]() mutable
        {
            auto codeGenService = runtime->getService(std::string("code-generation"));
            auto request = object{
                object::pair{std::string("projectName"), std::string("test-api-key-validation")}, 
                object::pair{std::string("description"), std::string("A plugin that requires specific API keys")}, 
                object::pair{std::string("targetType"), as<std::shared_ptr<const>>(std::string("plugin"))}, 
                object::pair{std::string("requirements"), array<string>{ std::string("Use Stripe API for payments") }}, 
                object::pair{std::string("apis"), array<string>{ std::string("Stripe") }}, 
                object::pair{std::string("testScenarios"), array<string>{ std::string("Test payment processing") }}
            };
            auto result = std::async([=]() { (as<any>(codeGenService))["generateCode"](request); });
            if (!runtime->getSetting(std::string("STRIPE_API_KEY"))) {
                expect(result["success"])->toBe(false);
                expect(result["errors"])->toBeDefined();
                expect(const_(result["errors"])[0])->toContain(std::string("STRIPE_API_KEY"));
            } else {
                expect(result["success"])->toBe(true);
            }
        }
        );
        it(std::string("should generate plugin with different approaches based on existing plugins"), [=]() mutable
        {
            auto hasApiKey = !!(OR((process->env->OPENAI_API_KEY), (process->env->ANTHROPIC_API_KEY)));
            if (!hasApiKey) {
                console->log(std::string("Skipping test - no API key available"));
                return std::shared_ptr<Promise<void>>();
            }
            auto codeGenService = runtime->getService(std::string("code-generation"));
            auto newPluginRequest = object{
                object::pair{std::string("projectName"), std::string("test-unique-blockchain-plugin")}, 
                object::pair{std::string("description"), std::string("A plugin for blockchain transaction monitoring")}, 
                object::pair{std::string("targetType"), as<std::shared_ptr<const>>(std::string("plugin"))}, 
                object::pair{std::string("requirements"), array<string>{ std::string("Monitor Ethereum transactions"), std::string("Alert on large transfers") }}, 
                object::pair{std::string("apis"), array<string>{ std::string("Ethereum") }}, 
                object::pair{std::string("testScenarios"), array<string>{ std::string("Test transaction monitoring") }}
            };
            auto newResult = std::async([=]() { (as<any>(codeGenService))["generateCode"](newPluginRequest); });
            expect(newResult["success"])->toBe(true);
            auto newPrd = JSON->parse(std::async([=]() { fs::readFile(path->join(newResult["projectPath"], std::string("PRD.json")), std::string("utf-8")); }));
            expect(newPrd["architecture"]["approach"])->toBe(std::string("new-plugin"));
            auto extensionRequest = object{
                object::pair{std::string("projectName"), std::string("test-enhanced-shell-plugin")}, 
                object::pair{std::string("description"), std::string("A plugin that extends shell capabilities with advanced features")}, 
                object::pair{std::string("targetType"), as<std::shared_ptr<const>>(std::string("plugin"))}, 
                object::pair{std::string("requirements"), array<string>{ std::string("Execute shell commands"), std::string("Add command history"), std::string("Add command aliases") }}, 
                object::pair{std::string("apis"), array<any>()}, 
                object::pair{std::string("testScenarios"), array<string>{ std::string("Test shell execution with history") }}
            };
            auto extResult = std::async([=]() { (as<any>(codeGenService))["generateCode"](extensionRequest); });
            expect(extResult["success"])->toBe(true);
            auto extPrd = JSON->parse(std::async([=]() { fs::readFile(path->join(extResult["projectPath"], std::string("PRD.json")), std::string("utf-8")); }));
            expect(array<string>{ std::string("extend-existing"), std::string("new-plugin") })->toContain(extPrd["architecture"]["approach"]);
        }
        );
        afterAll([=]() mutable
        {
            try
            {
                std::async([=]() { fs::rm(testProjectsDir, object{
                    object::pair{std::string("recursive"), true}, 
                    object::pair{std::string("force"), true}
                }); });
            }
            catch (const any& error)
            {
                console->error(std::string("Failed to clean up test directory:"), error);
            }
        }
        );
    }
    );
}

MAIN
