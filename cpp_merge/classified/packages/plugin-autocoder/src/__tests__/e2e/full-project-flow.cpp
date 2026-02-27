#include "full-project-flow.test.h"

void Main(void)
{
    describe(std:("Full Project Flow with Real Keys"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> runtime;
        shared<string> testProjectsDir;
        beforeAll([=]() mutable
        {
            auto hasApiKey = !!(OR((process->env->OPENAI_API_KEY), (process->env->ANTHROPIC_API_KEY)));
            if (!hasApiKey) {
                console->log(std:("⚠️  No API keys found - skipping full flow tests"));
                return std::shared_ptr<Promise<void>>();
            }
            testProjectsDir = path->join(process->cwd(), std:(".test-full-flow-projects"));
            std::async([=]() { fs::mkdir(testProjectsDir, object{
                object::pair{std:("recursive"), true}
            }); });
            process->env->FORCE_BUNSQLITE = std:("true");
            process->env->DATABASE_PATH = std:("./.eliza/.test-full-flow");
            process->env->ELIZA_TEST_MODE = std:("true");
            process->env->SECRET_SALT = OR((process->env->SECRET_SALT), (std:("test-salt-for-testing-only-not-secure")));
            runtime = std::make_shared<AgentRuntime>(object{
                object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("character"), object{
                    object::pair{std:("name"), std:("Test Agent")}, 
                    object::pair{std:("bio"), array<string>{ std:("Test agent for full flow validation") }}, 
                    object::pair{std:("system"), std:("You are a helpful coding assistant")}, 
                    object::pair{std:("settings"), object{
                        object::pair{std:("OPENAI_API_KEY"), process->env->OPENAI_API_KEY}, 
                        object::pair{std:("ANTHROPIC_API_KEY"), process->env->ANTHROPIC_API_KEY}, 
                        object::pair{std:("USE_CLAUDE_PROXY"), (process->env->ANTHROPIC_API_KEY) ? std:("false") : std:("true")}
                    }}, 
                    object::pair{std:("modelProvider"), (process->env->ANTHROPIC_API_KEY) ? std:("anthropic") : std:("openai")}
                }}
            });
            std::async([=]() { runtime->registerPlugin(sqlPlugin); });
            std::async([=]() { runtime->registerPlugin(inferencePlugin); });
            if (process->env->ANTHROPIC_API_KEY) {
                std::async([=]() { runtime->registerPlugin(anthropicPlugin); });
            } else if (process->env->OPENAI_API_KEY) {
                std::async([=]() { runtime->registerPlugin(openaiPlugin); });
            }
            std::async([=]() { runtime->registerPlugin(formsPlugin); });
            std::async([=]() { runtime->registerPlugin(autocoderPlugin); });
            std::async([=]() { runtime->initialize(); });
        }
        );
        it(std:("should complete full project generation flow: setup -> PRD -> code -> validation"), [=]() mutable
        {
            auto hasApiKey = !!(OR((process->env->OPENAI_API_KEY), (process->env->ANTHROPIC_API_KEY)));
            if (!hasApiKey) {
                console->log(std:("Skipping test - no API key available"));
                return std::shared_ptr<Promise<void>>();
            }
            console->log(std:("🚀 Starting full project flow test"));
            console->log(std:("   Using model provider: ") + runtime->character->modelProvider + string_empty);
            auto codeGenService = runtime->getService(std:("code-generation"));
            auto statusManager = as<any>(runtime->getService(std:("project-status-manager")));
            auto projectPlanningService = as<any>(runtime->getService(std:("project-planning")));
            expect(codeGenService)->toBeDefined();
            expect(statusManager)->toBeDefined();
            expect(projectPlanningService)->toBeDefined();
            shared statusHistory = array<any>();
            statusManager["on"](std:("update"), [=](auto update) mutable
            {
                statusHistory->push(object{
                    object::pair{std:("timestamp"), Date->now()}, 
                    object::pair{std:("status"), update["status"]}, 
                    object::pair{std:("progress"), update["progress"]}, 
                    object::pair{std:("step"), update["currentStep"]}, 
                    object::pair{std:("message"), update["message"]}
                });
                console->log(std:("📊 [") + (OR((update["progress"]), (0))) + std:("%] ") + update["status"] + std:(": ") + (OR((OR((update["message"]), (update["currentStep"]))), (string_empty))) + string_empty);
            }
            );
            console->log(std:("\
📝 Step 1: Creating project request..."));
            auto request = object{
                object::pair{std:("projectName"), std:("hello-world-plugin")}, 
                object::pair{std:("description"), std:("A simple plugin that greets users with Hello World")}, 
                object::pair{std:("targetType"), as<std::shared_ptr<const>>(std:("plugin"))}, 
                object::pair{std:("requirements"), array<string>{ std:("Create a GREET action that responds with "Hello World""), std:("Add a greeting provider that shows current greeting"), std:("Include proper TypeScript types"), std:("Add unit tests for the action") }}, 
                object::pair{std:("apis"), array<any>()}, 
                object::pair{std:("testScenarios"), array<string>{ std:("Should respond with Hello World when greeted"), std:("Should validate message correctly"), std:("Should provide greeting context") }}
            };
            console->log(std:("\
🔨 Step 2: Generating project..."));
            auto startTime = Date->now();
            auto result = std::async([=]() { (as<any>(codeGenService))["generateCode"](request); });
            auto duration = Date->now() - startTime;
            console->log(std:("\
✅ Step 3: Verifying results (completed in ") + (duration / 1000)->toFixed(1) + std:("s)"));
            expect(result["success"])->toBe(true);
            expect(result["projectPath"])->toBeDefined();
            expect(result["files"])->toBeDefined();
            expect(result["files"]["length"])->toBeGreaterThan(0);
            console->log(std:("   Generated ") + result["files"]["length"] + std:(" files"));
            console->log(std:("   Project path: ") + result["projectPath"] + string_empty);
            console->log(std:("\
🔍 Step 4: Verifying project phases..."));
            shared phases = array<string>{ std:("planning"), std:("generating"), std:("testing") };
            auto completedPhases = statusHistory->map([=](auto s) mutable
            {
                return s["status"];
            }
            )->filter([=](auto s) mutable
            {
                return phases->includes(s);
            }
            );
            expect(completedPhases->get_length())->toBeGreaterThan(0);
            console->log(std:("   Completed phases: ") + (array<any>{ std::make_shared<Set>(completedPhases) })->join(std:(", ")) + string_empty);
            console->log(std:("\
📋 Step 5: Verifying PRD..."));
            auto prdPath = path->join(result["projectPath"], std:("PRD.json"));
            auto prdExists = std::async([=]() { fs::access(prdPath)->then([=]() mutable
            {
                return true;
            }
            )->_catch([=]() mutable
            {
                return false;
            }
            ); });
            expect(prdExists)->toBe(true);
            auto prd = JSON->parse(std::async([=]() { fs::readFile(prdPath, std:("utf-8")); }));
            expect(prd["title"])->toBeDefined();
            expect(prd["architecture"])->toBeDefined();
            expect(prd["architecture"]["approach"])->toMatch((new RegExp(std:("^(clone-existing|extend-existing|new-plugin)"))));
            console->log(std:("   PRD approach: ") + prd["architecture"]["approach"] + string_empty);
            console->log(std:("   Components: ") + prd["architecture"]["components"]["join"](std:(", ")) + string_empty);
            console->log(std:("\
📁 Step 6: Verifying essential files..."));
            auto essentialFiles = array<string>{ std:("package.json"), std:("src/index.ts"), std:("tsconfig.json"), std:("README.md"), std:(".gitignore") };
            for (auto& file : essentialFiles)
            {
                auto filePath = path->join(result["projectPath"], file);
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
                console->log(std:("   ✓ ") + file + string_empty);
            }
            console->log(std:("\
🎯 Step 7: Verifying GREET action..."));
            auto actionsDir = path->join(result["projectPath"], std:("src/actions"));
            auto actionsDirExists = std::async([=]() { fs::access(actionsDir)->then([=]() mutable
            {
                return true;
            }
            )->_catch([=]() mutable
            {
                return false;
            }
            ); });
            expect(actionsDirExists)->toBe(true);
            if (actionsDirExists) {
                auto actionFiles = std::async([=]() { fs::readdir(actionsDir); });
                auto greetAction = actionFiles->find([=](auto f) mutable
                {
                    return OR((f->toLowerCase()->includes(std:("greet"))), (f->toLowerCase()->includes(std:("hello"))));
                }
                );
                expect(greetAction)->toBeDefined();
                console->log(std:("   ✓ Found action file: ") + greetAction + string_empty);
            }
            console->log(std:("\
🧪 Step 8: Verifying test files..."));
            auto testsDir = path->join(result["projectPath"], std:("src/__tests__"));
            auto testsDirExists = std::async([=]() { fs::access(testsDir)->then([=]() mutable
            {
                return true;
            }
            )->_catch([=]() mutable
            {
                return false;
            }
            ); });
            expect(testsDirExists)->toBe(true);
            console->log(std:("   ✓ Tests directory exists"));
            console->log(std:("\
📊 Step 9: Checking validation results..."));
            if (result["executionResults"]) {
                console->log(std:("   Lint: ") + (result["executionResults"]["lintPass"]) ? std:("✅") : std:("❌") + string_empty);
                console->log(std:("   Types: ") + (result["executionResults"]["typesPass"]) ? std:("✅") : std:("❌") + string_empty);
                console->log(std:("   Tests: ") + (result["executionResults"]["testsPass"]) ? std:("✅") : std:("❌") + string_empty);
                console->log(std:("   Build: ") + (result["executionResults"]["buildPass"]) ? std:("✅") : std:("❌") + string_empty);
            }
            console->log(std:("\
📈 Step 10: Verifying status tracking..."));
            auto projectHistory = statusManager["getHistory"]();
            expect(projectHistory["length"])->toBeGreaterThan(0);
            auto latestProject = const_(projectHistory)[0];
            expect(latestProject["status"])->toBe(std:("completed"));
            expect(latestProject["filesGenerated"])->toBeGreaterThan(0);
            console->log(std:("   Project completed in ") + ((OR((latestProject["duration"]), (0))) / 1000)->toFixed(1) + std:("s"));
            console->log(std:("   Files generated: ") + latestProject["filesGenerated"] + string_empty);
            console->log(std:("\
🎉 Full project flow test completed successfully!"));
        }
        );
        it(std:("should handle complex project with multiple APIs"), [=]() mutable
        {
            auto hasApiKey = !!(OR((process->env->OPENAI_API_KEY), (process->env->ANTHROPIC_API_KEY)));
            if (!hasApiKey) {
                console->log(std:("Skipping test - no API key available"));
                return std::shared_ptr<Promise<void>>();
            }
            console->log(std:("🚀 Starting complex project test"));
            auto codeGenService = runtime->getService(std:("code-generation"));
            auto request = object{
                object::pair{std:("projectName"), std:("weather-alert-plugin")}, 
                object::pair{std:("description"), std:("A plugin that monitors weather and sends alerts")}, 
                object::pair{std:("targetType"), as<std::shared_ptr<const>>(std:("plugin"))}, 
                object::pair{std:("requirements"), array<string>{ std:("Monitor weather for multiple cities"), std:("Send alerts when severe weather is detected"), std:("Cache weather data for efficiency"), std:("Support multiple weather providers") }}, 
                object::pair{std:("apis"), array<string>{ std:("OpenWeatherMap"), std:("Redis") }}, 
                object::pair{std:("testScenarios"), array<string>{ std:("Should fetch weather data"), std:("Should cache results"), std:("Should send alerts for severe weather") }}
            };
            console->log(std:("🔨 Generating complex project..."));
            auto result = std::async([=]() { (as<any>(codeGenService))["generateCode"](request); });
            if (!result["success"]) {
                if (const_(result["errors"])[0]["includes"](std:("API_KEY"))) {
                    console->log(std:("✅ Correctly validated API key requirements"));
                    expect(const_(result["errors"])[0])->toContain(std:("API_KEY"));
                    return std::shared_ptr<Promise<void>>();
                }
                throw any(std::make_shared<Error>(std:("Unexpected failure: ") + result["errors"]["join"](std:(", ")) + string_empty));
            }
            auto prdPath = path->join(result["projectPath"], std:("PRD.json"));
            auto prd = JSON->parse(std::async([=]() { fs::readFile(prdPath, std:("utf-8")); }));
            expect(prd["apiKeys"])->toBeDefined();
            expect(prd["apiKeys"]["length"])->toBeGreaterThan(0);
            console->log(std:("✅ PRD includes ") + prd["apiKeys"]["length"] + std:(" required API keys"));
        }
        );
        it(std:("should use claude proxy when only OpenAI key is available"), [=]() mutable
        {
            if (OR((!process->env->OPENAI_API_KEY), (process->env->ANTHROPIC_API_KEY))) {
                console->log(std:("Skipping test - requires OpenAI key without Anthropic key"));
                return std::shared_ptr<Promise<void>>();
            }
            console->log(std:("🔄 Testing Claude proxy integration..."));
            expect(runtime->getSetting(std:("USE_CLAUDE_PROXY")))->toBe(std:("true"));
            auto codeGenService = runtime->getService(std:("code-generation"));
            auto request = object{
                object::pair{std:("projectName"), std:("proxy-test-plugin")}, 
                object::pair{std:("description"), std:("Test plugin to verify proxy works")}, 
                object::pair{std:("targetType"), as<std::shared_ptr<const>>(std:("plugin"))}, 
                object::pair{std:("requirements"), array<string>{ std:("Simple test action") }}, 
                object::pair{std:("apis"), array<any>()}
            };
            auto result = std::async([=]() { (as<any>(codeGenService))["generateCode"](request); });
            expect(result["success"])->toBe(true);
            console->log(std:("✅ Successfully generated code using Claude proxy with OpenAI"));
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
