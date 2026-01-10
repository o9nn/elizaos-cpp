#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-autocoder/src/__tests__/e2e/full-project-flow.test.h"

void Main(void)
{
    describe(std::string("Full Project Flow with Real Keys"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> runtime;
        shared<string> testProjectsDir;
        beforeAll([=]() mutable
        {
            auto hasApiKey = !!(OR((process->env->OPENAI_API_KEY), (process->env->ANTHROPIC_API_KEY)));
            if (!hasApiKey) {
                console->log(std::string("⚠️  No API keys found - skipping full flow tests"));
                return std::shared_ptr<Promise<void>>();
            }
            testProjectsDir = path->join(process->cwd(), std::string(".test-full-flow-projects"));
            std::async([=]() { fs::mkdir(testProjectsDir, object{
                object::pair{std::string("recursive"), true}
            }); });
            process->env->FORCE_BUNSQLITE = std::string("true");
            process->env->DATABASE_PATH = std::string("./.eliza/.test-full-flow");
            process->env->ELIZA_TEST_MODE = std::string("true");
            process->env->SECRET_SALT = OR((process->env->SECRET_SALT), (std::string("test-salt-for-testing-only-not-secure")));
            runtime = std::make_shared<AgentRuntime>(object{
                object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("character"), object{
                    object::pair{std::string("name"), std::string("Test Agent")}, 
                    object::pair{std::string("bio"), array<string>{ std::string("Test agent for full flow validation") }}, 
                    object::pair{std::string("system"), std::string("You are a helpful coding assistant")}, 
                    object::pair{std::string("settings"), object{
                        object::pair{std::string("OPENAI_API_KEY"), process->env->OPENAI_API_KEY}, 
                        object::pair{std::string("ANTHROPIC_API_KEY"), process->env->ANTHROPIC_API_KEY}, 
                        object::pair{std::string("USE_CLAUDE_PROXY"), (process->env->ANTHROPIC_API_KEY) ? std::string("false") : std::string("true")}
                    }}, 
                    object::pair{std::string("modelProvider"), (process->env->ANTHROPIC_API_KEY) ? std::string("anthropic") : std::string("openai")}
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
        it(std::string("should complete full project generation flow: setup -> PRD -> code -> validation"), [=]() mutable
        {
            auto hasApiKey = !!(OR((process->env->OPENAI_API_KEY), (process->env->ANTHROPIC_API_KEY)));
            if (!hasApiKey) {
                console->log(std::string("Skipping test - no API key available"));
                return std::shared_ptr<Promise<void>>();
            }
            console->log(std::string("🚀 Starting full project flow test"));
            console->log(std::string("   Using model provider: ") + runtime->character->modelProvider + string_empty);
            auto codeGenService = runtime->getService(std::string("code-generation"));
            auto statusManager = as<any>(runtime->getService(std::string("project-status-manager")));
            auto projectPlanningService = as<any>(runtime->getService(std::string("project-planning")));
            expect(codeGenService)->toBeDefined();
            expect(statusManager)->toBeDefined();
            expect(projectPlanningService)->toBeDefined();
            shared statusHistory = array<any>();
            statusManager["on"](std::string("update"), [=](auto update) mutable
            {
                statusHistory->push(object{
                    object::pair{std::string("timestamp"), Date->now()}, 
                    object::pair{std::string("status"), update["status"]}, 
                    object::pair{std::string("progress"), update["progress"]}, 
                    object::pair{std::string("step"), update["currentStep"]}, 
                    object::pair{std::string("message"), update["message"]}
                });
                console->log(std::string("📊 [") + (OR((update["progress"]), (0))) + std::string("%] ") + update["status"] + std::string(": ") + (OR((OR((update["message"]), (update["currentStep"]))), (string_empty))) + string_empty);
            }
            );
            console->log(std::string("\
📝 Step 1: Creating project request..."));
            auto request = object{
                object::pair{std::string("projectName"), std::string("hello-world-plugin")}, 
                object::pair{std::string("description"), std::string("A simple plugin that greets users with Hello World")}, 
                object::pair{std::string("targetType"), as<std::shared_ptr<const>>(std::string("plugin"))}, 
                object::pair{std::string("requirements"), array<string>{ std::string("Create a GREET action that responds with "Hello World""), std::string("Add a greeting provider that shows current greeting"), std::string("Include proper TypeScript types"), std::string("Add unit tests for the action") }}, 
                object::pair{std::string("apis"), array<any>()}, 
                object::pair{std::string("testScenarios"), array<string>{ std::string("Should respond with Hello World when greeted"), std::string("Should validate message correctly"), std::string("Should provide greeting context") }}
            };
            console->log(std::string("\
🔨 Step 2: Generating project..."));
            auto startTime = Date->now();
            auto result = std::async([=]() { (as<any>(codeGenService))["generateCode"](request); });
            auto duration = Date->now() - startTime;
            console->log(std::string("\
✅ Step 3: Verifying results (completed in ") + (duration / 1000)->toFixed(1) + std::string("s)"));
            expect(result["success"])->toBe(true);
            expect(result["projectPath"])->toBeDefined();
            expect(result["files"])->toBeDefined();
            expect(result["files"]["length"])->toBeGreaterThan(0);
            console->log(std::string("   Generated ") + result["files"]["length"] + std::string(" files"));
            console->log(std::string("   Project path: ") + result["projectPath"] + string_empty);
            console->log(std::string("\
🔍 Step 4: Verifying project phases..."));
            shared phases = array<string>{ std::string("planning"), std::string("generating"), std::string("testing") };
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
            console->log(std::string("   Completed phases: ") + (array<any>{ std::make_shared<Set>(completedPhases) })->join(std::string(", ")) + string_empty);
            console->log(std::string("\
📋 Step 5: Verifying PRD..."));
            auto prdPath = path->join(result["projectPath"], std::string("PRD.json"));
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
            auto prd = JSON->parse(std::async([=]() { fs::readFile(prdPath, std::string("utf-8")); }));
            expect(prd["title"])->toBeDefined();
            expect(prd["architecture"])->toBeDefined();
            expect(prd["architecture"]["approach"])->toMatch((new RegExp(std::string("^(clone-existing|extend-existing|new-plugin)"))));
            console->log(std::string("   PRD approach: ") + prd["architecture"]["approach"] + string_empty);
            console->log(std::string("   Components: ") + prd["architecture"]["components"]["join"](std::string(", ")) + string_empty);
            console->log(std::string("\
📁 Step 6: Verifying essential files..."));
            auto essentialFiles = array<string>{ std::string("package.json"), std::string("src/index.ts"), std::string("tsconfig.json"), std::string("README.md"), std::string(".gitignore") };
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
                console->log(std::string("   ✓ ") + file + string_empty);
            }
            console->log(std::string("\
🎯 Step 7: Verifying GREET action..."));
            auto actionsDir = path->join(result["projectPath"], std::string("src/actions"));
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
                    return OR((f->toLowerCase()->includes(std::string("greet"))), (f->toLowerCase()->includes(std::string("hello"))));
                }
                );
                expect(greetAction)->toBeDefined();
                console->log(std::string("   ✓ Found action file: ") + greetAction + string_empty);
            }
            console->log(std::string("\
🧪 Step 8: Verifying test files..."));
            auto testsDir = path->join(result["projectPath"], std::string("src/__tests__"));
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
            console->log(std::string("   ✓ Tests directory exists"));
            console->log(std::string("\
📊 Step 9: Checking validation results..."));
            if (result["executionResults"]) {
                console->log(std::string("   Lint: ") + (result["executionResults"]["lintPass"]) ? std::string("✅") : std::string("❌") + string_empty);
                console->log(std::string("   Types: ") + (result["executionResults"]["typesPass"]) ? std::string("✅") : std::string("❌") + string_empty);
                console->log(std::string("   Tests: ") + (result["executionResults"]["testsPass"]) ? std::string("✅") : std::string("❌") + string_empty);
                console->log(std::string("   Build: ") + (result["executionResults"]["buildPass"]) ? std::string("✅") : std::string("❌") + string_empty);
            }
            console->log(std::string("\
📈 Step 10: Verifying status tracking..."));
            auto projectHistory = statusManager["getHistory"]();
            expect(projectHistory["length"])->toBeGreaterThan(0);
            auto latestProject = const_(projectHistory)[0];
            expect(latestProject["status"])->toBe(std::string("completed"));
            expect(latestProject["filesGenerated"])->toBeGreaterThan(0);
            console->log(std::string("   Project completed in ") + ((OR((latestProject["duration"]), (0))) / 1000)->toFixed(1) + std::string("s"));
            console->log(std::string("   Files generated: ") + latestProject["filesGenerated"] + string_empty);
            console->log(std::string("\
🎉 Full project flow test completed successfully!"));
        }
        );
        it(std::string("should handle complex project with multiple APIs"), [=]() mutable
        {
            auto hasApiKey = !!(OR((process->env->OPENAI_API_KEY), (process->env->ANTHROPIC_API_KEY)));
            if (!hasApiKey) {
                console->log(std::string("Skipping test - no API key available"));
                return std::shared_ptr<Promise<void>>();
            }
            console->log(std::string("🚀 Starting complex project test"));
            auto codeGenService = runtime->getService(std::string("code-generation"));
            auto request = object{
                object::pair{std::string("projectName"), std::string("weather-alert-plugin")}, 
                object::pair{std::string("description"), std::string("A plugin that monitors weather and sends alerts")}, 
                object::pair{std::string("targetType"), as<std::shared_ptr<const>>(std::string("plugin"))}, 
                object::pair{std::string("requirements"), array<string>{ std::string("Monitor weather for multiple cities"), std::string("Send alerts when severe weather is detected"), std::string("Cache weather data for efficiency"), std::string("Support multiple weather providers") }}, 
                object::pair{std::string("apis"), array<string>{ std::string("OpenWeatherMap"), std::string("Redis") }}, 
                object::pair{std::string("testScenarios"), array<string>{ std::string("Should fetch weather data"), std::string("Should cache results"), std::string("Should send alerts for severe weather") }}
            };
            console->log(std::string("🔨 Generating complex project..."));
            auto result = std::async([=]() { (as<any>(codeGenService))["generateCode"](request); });
            if (!result["success"]) {
                if (const_(result["errors"])[0]["includes"](std::string("API_KEY"))) {
                    console->log(std::string("✅ Correctly validated API key requirements"));
                    expect(const_(result["errors"])[0])->toContain(std::string("API_KEY"));
                    return std::shared_ptr<Promise<void>>();
                }
                throw any(std::make_shared<Error>(std::string("Unexpected failure: ") + result["errors"]["join"](std::string(", ")) + string_empty));
            }
            auto prdPath = path->join(result["projectPath"], std::string("PRD.json"));
            auto prd = JSON->parse(std::async([=]() { fs::readFile(prdPath, std::string("utf-8")); }));
            expect(prd["apiKeys"])->toBeDefined();
            expect(prd["apiKeys"]["length"])->toBeGreaterThan(0);
            console->log(std::string("✅ PRD includes ") + prd["apiKeys"]["length"] + std::string(" required API keys"));
        }
        );
        it(std::string("should use claude proxy when only OpenAI key is available"), [=]() mutable
        {
            if (OR((!process->env->OPENAI_API_KEY), (process->env->ANTHROPIC_API_KEY))) {
                console->log(std::string("Skipping test - requires OpenAI key without Anthropic key"));
                return std::shared_ptr<Promise<void>>();
            }
            console->log(std::string("🔄 Testing Claude proxy integration..."));
            expect(runtime->getSetting(std::string("USE_CLAUDE_PROXY")))->toBe(std::string("true"));
            auto codeGenService = runtime->getService(std::string("code-generation"));
            auto request = object{
                object::pair{std::string("projectName"), std::string("proxy-test-plugin")}, 
                object::pair{std::string("description"), std::string("Test plugin to verify proxy works")}, 
                object::pair{std::string("targetType"), as<std::shared_ptr<const>>(std::string("plugin"))}, 
                object::pair{std::string("requirements"), array<string>{ std::string("Simple test action") }}, 
                object::pair{std::string("apis"), array<any>()}
            };
            auto result = std::async([=]() { (as<any>(codeGenService))["generateCode"](request); });
            expect(result["success"])->toBe(true);
            console->log(std::string("✅ Successfully generated code using Claude proxy with OpenAI"));
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
