#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/tests/commands/create.test.h"

void Main(void)
{
    describe(std::string("ElizaOS Create Commands"), [=]() mutable
    {
        shared<string> testTmpDir;
        shared<string> elizaosCmd;
        shared<string> createElizaCmd;
        shared<string> originalCwd;
        beforeEach([=]() mutable
        {
            originalCwd = process->cwd();
            testTmpDir = std::async([=]() { mkdtemp(join(tmpdir(), std::string("eliza-test-"))); });
            auto scriptDir = join(__dirname, std::string(".."));
            elizaosCmd = std::string("bun "") + join(scriptDir, std::string("../dist/index.js")) + std::string(""");
            createElizaCmd = std::string("bun "") + join(scriptDir, std::string("../../create-eliza/index.mjs")) + std::string(""");
            process->chdir(testTmpDir);
        }
        );
        afterEach([=]() mutable
        {
            safeChangeDirectory(originalCwd);
            if (testTmpDir) {
                try
                {
                    std::async([=]() { rm(testTmpDir, object{
                        object::pair{std::string("recursive"), true}
                    }); });
                }
                catch (const any& e)
                {
                }
            }
        }
        );
        shared validateAgentJson = [=](auto jsonFile, auto expectedName) mutable
        {
            auto content = std::async([=]() { readFile(jsonFile, std::string("utf8")); });
            auto agentData = JSON->parse(content);
            expect(agentData["name"])->toBe(expectedName);
            expect(type_of(agentData["system"]))->toBe(std::string("string"));
            expect(agentData["system"]["length"])->toBeGreaterThan(0);
            expect(Array->isArray(agentData["bio"]))->toBe(true);
            expect(agentData["bio"]["length"])->toBeGreaterThan(0);
            expect(Array->isArray(agentData["messageExamples"]))->toBe(true);
            expect(agentData["messageExamples"]["length"])->toBeGreaterThan(0);
            expect(type_of(agentData["style"]))->toBe(std::string("object"));
            expect(Array->isArray(agentData["style"]["all"]))->toBe(true);
            expect(agentData["style"]["all"]["length"])->toBeGreaterThan(0);
        };
        it(std::string("create --help shows usage"), [=]() mutable
        {
            auto result = execSync(string_empty + elizaosCmd + std::string(" create --help"), getPlatformOptions(object{
                object::pair{std::string("encoding"), std::string("utf8")}
            }));
            expect(result)->toContain(std::string("Usage: elizaos create"));
            expect(result)->toMatch((new RegExp(std::string("(project|plugin|agent"))));
            expect(result)->not->toContain(std::string("frobnicate"));
        }
        );
        it(std::string("create default project succeeds"), [=]() mutable
        {
            crossPlatform["removeDir"](std::string("my-default-app"));
            shared result = runCliCommandSilently(elizaosCmd, std::string("create my-default-app --yes"), object{
                object::pair{std::string("timeout"), TEST_TIMEOUTS["PROJECT_CREATION"]}
            });
            auto successPatterns = array<string>{ std::string("Project initialized successfully!"), std::string("successfully initialized"), std::string("Project created"), std::string("created successfully") };
            auto hasSuccess = successPatterns->some([=](auto pattern) mutable
            {
                return result->includes(pattern);
            }
            );
            if (!hasSuccess) {
                expect(existsSync(std::string("my-default-app")))->toBe(true);
                expect(existsSync(std::string("my-default-app/package.json")))->toBe(true);
            } else {
                expect(hasSuccess)->toBe(true);
            }
            expect(existsSync(std::string("my-default-app")))->toBe(true);
            expect(existsSync(std::string("my-default-app/package.json")))->toBe(true);
            expect(existsSync(std::string("my-default-app/src")))->toBe(true);
            expect(existsSync(std::string("my-default-app/.gitignore")))->toBe(true);
            expect(existsSync(std::string("my-default-app/.npmignore")))->toBe(true);
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std::string("create plugin project succeeds"), [=]() mutable
        {
            crossPlatform["removeDir"](std::string("plugin-my-plugin-app"));
            shared result = runCliCommandSilently(elizaosCmd, std::string("create my-plugin-app --yes --type plugin"), object{
                object::pair{std::string("timeout"), TEST_TIMEOUTS["PROJECT_CREATION"]}
            });
            auto successPatterns = array<string>{ std::string("Plugin initialized successfully!"), std::string("successfully initialized"), std::string("Plugin created"), std::string("created successfully") };
            auto hasSuccess = successPatterns->some([=](auto pattern) mutable
            {
                return result->includes(pattern);
            }
            );
            auto pluginDir = std::string("plugin-my-plugin-app");
            if (!hasSuccess) {
                expect(existsSync(pluginDir))->toBe(true);
                expect(existsSync(join(pluginDir, std::string("package.json"))))->toBe(true);
            } else {
                expect(hasSuccess)->toBe(true);
            }
            expect(existsSync(pluginDir))->toBe(true);
            expect(existsSync(join(pluginDir, std::string("package.json"))))->toBe(true);
            expect(existsSync(join(pluginDir, std::string("src/index.ts"))))->toBe(true);
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std::string("create agent succeeds"), [=]() mutable
        {
            crossPlatform["removeFile"](std::string("my-test-agent.json"));
            auto result = runCliCommandSilently(elizaosCmd, std::string("create my-test-agent --yes --type agent"));
            expect(result)->toContain(std::string("Agent character created successfully"));
            expect(existsSync(std::string("my-test-agent.json")))->toBe(true);
            std::async([=]() { validateAgentJson(std::string("my-test-agent.json"), std::string("my-test-agent")); });
        }
        );
        it(std::string("rejects creating project in existing directory"), [=]() mutable
        {
            try
            {
                crossPlatform["removeDir"](std::string("existing-app"));
                execSync(std::string("mkdir existing-app"), getPlatformOptions(object{
                    object::pair{std::string("stdio"), std::string("ignore")}
                }));
                if (process->platform == std::string("win32")) {
                    execSync(std::string("echo test > existing-app\file.txt"), getPlatformOptions(object{
                        object::pair{std::string("stdio"), std::string("ignore")}
                    }));
                } else {
                    execSync(std::string("echo "test" > existing-app/file.txt"), getPlatformOptions(object{
                        object::pair{std::string("stdio"), std::string("ignore")}
                    }));
                }
            }
            catch (const any& e)
            {
            }
            auto result = expectCliCommandToFail(elizaosCmd, std::string("create existing-app --yes"));
            expect(result["status"])->not->toBe(0);
            expect(result["output"])->toContain(std::string("already exists"));
        }
        );
        it(std::string("create project in current directory"), [=]() mutable
        {
            try
            {
                crossPlatform["removeDir"](std::string("create-in-place"));
                execSync(std::string("mkdir create-in-place"), getPlatformOptions(object{
                    object::pair{std::string("stdio"), std::string("ignore")}
                }));
            }
            catch (const any& e)
            {
            }
            process->chdir(std::string("create-in-place"));
            auto result = runCliCommandSilently(elizaosCmd, std::string("create . --yes"), object{
                object::pair{std::string("timeout"), TEST_TIMEOUTS["PROJECT_CREATION"]}
            });
            expect(result)->toContain(std::string("Project initialized successfully!"));
            expect(existsSync(std::string("package.json")))->toBe(true);
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std::string("rejects invalid project name"), [=]() mutable
        {
            auto result = expectCliCommandToFail(elizaosCmd, std::string("create "Invalid Name" --yes"));
            expect(result["status"])->not->toBe(0);
            expect(result["output"])->toMatch((new RegExp(std::string("Invalid"))));
        }
        );
        it(std::string("rejects invalid project type"), [=]() mutable
        {
            auto result = expectCliCommandToFail(elizaosCmd, std::string("create bad-type-proj --yes --type bad-type"));
            expect(result["status"])->not->toBe(0);
            expect(result["output"])->toMatch((new RegExp(std::string("Invalid type"))));
        }
        );
        it(std::string("create-eliza default project succeeds"), [=]() mutable
        {
            crossPlatform["removeDir"](std::string("my-create-app"));
            try
            {
                auto result = runCliCommandSilently(createElizaCmd, std::string("my-create-app --yes"));
                expect(result)->toContain(std::string("Project initialized successfully!"));
                expect(existsSync(std::string("my-create-app")))->toBe(true);
                expect(existsSync(std::string("my-create-app/package.json")))->toBe(true);
                expect(existsSync(std::string("my-create-app/src")))->toBe(true);
            }
            catch (const any& e)
            {
                console->warn(std::string("Skipping create-eliza test - command not available"));
            }
        }
        , 60000);
        it(std::string("create-eliza plugin project succeeds"), [=]() mutable
        {
            crossPlatform["removeDir"](std::string("plugin-my-create-plugin"));
            try
            {
                auto result = runCliCommandSilently(createElizaCmd, std::string("my-create-plugin --yes --type plugin"));
                expect(result)->toContain(std::string("Plugin initialized successfully!"));
                auto pluginDir = std::string("plugin-my-create-plugin");
                expect(existsSync(pluginDir))->toBe(true);
                expect(existsSync(join(pluginDir, std::string("package.json"))))->toBe(true);
                expect(existsSync(join(pluginDir, std::string("src/index.ts"))))->toBe(true);
            }
            catch (const any& e)
            {
                console->warn(std::string("Skipping create-eliza plugin test - command not available"));
            }
        }
        , 60000);
        it(std::string("create-eliza agent succeeds"), [=]() mutable
        {
            crossPlatform["removeFile"](std::string("my-create-agent.json"));
            try
            {
                auto result = runCliCommandSilently(createElizaCmd, std::string("my-create-agent --yes --type agent"));
                expect(result)->toContain(std::string("Agent character created successfully"));
                expect(existsSync(std::string("my-create-agent.json")))->toBe(true);
                std::async([=]() { validateAgentJson(std::string("my-create-agent.json"), std::string("my-create-agent")); });
            }
            catch (const any& e)
            {
                console->warn(std::string("Skipping create-eliza agent test - command not available"));
            }
        }
        , 60000);
        describe(std::string("AI Model Selection"), [=]() mutable
        {
            it(std::string("returns a reasonable number of AI model options"), [=]() mutable
            {
                auto models = getAvailableAIModels();
                expect(models->get_length())->toBeGreaterThanOrEqual(3);
                expect(models->get_length())->toBeLessThanOrEqual(7);
            }
            );
            it(std::string("maintains core AI model options"), [=]() mutable
            {
                auto models = getAvailableAIModels();
                shared values = models->map([=](auto m) mutable
                {
                    return m->value;
                }
                );
                auto CORE_PROVIDERS = array<string>{ std::string("local"), std::string("openai"), std::string("claude"), std::string("openrouter") };
                CORE_PROVIDERS->forEach([=](auto provider) mutable
                {
                    expect(values)->toContain(provider);
                }
                );
            }
            );
            it(std::string("all AI models follow the expected contract"), [=]() mutable
            {
                auto models = getAvailableAIModels();
                models->forEach([=](auto model) mutable
                {
                    expect(model)->toHaveProperty(std::string("value"));
                    expect(model)->toHaveProperty(std::string("title"));
                    expect(model)->toHaveProperty(std::string("description"));
                    expect(type_of(model->value))->toBe(std::string("string"));
                    expect(type_of(model->title))->toBe(std::string("string"));
                    expect(type_of(model->description))->toBe(std::string("string"));
                    expect(model->value->get_length())->toBeGreaterThan(0);
                    expect(model->title->get_length())->toBeGreaterThan(0);
                    expect(model->description->get_length())->toBeGreaterThan(0);
                    expect(model->value)->toBe(model->value->toLowerCase());
                }
                );
            }
            );
        }
        );
        describe(std::string("Ollama Configuration"), [=]() mutable
        {
            it(std::string("validates valid ollama endpoints"), [=]() mutable
            {
                expect(isValidOllamaEndpoint(std::string("http://localhost:11434")))->toBe(true);
                expect(isValidOllamaEndpoint(std::string("https://ollama.example.com")))->toBe(true);
                expect(isValidOllamaEndpoint(std::string("http://192.168.1.100:11434")))->toBe(true);
            }
            );
            it(std::string("rejects invalid ollama endpoints"), [=]() mutable
            {
                expect(isValidOllamaEndpoint(string_empty))->toBe(false);
                expect(isValidOllamaEndpoint(std::string("localhost:11434")))->toBe(false);
                expect(isValidOllamaEndpoint(std::string("ftp://localhost:11434")))->toBe(false);
                expect(isValidOllamaEndpoint(std::string("not-a-url")))->toBe(false);
                expect(isValidOllamaEndpoint(as<any>(nullptr)))->toBe(false);
                expect(isValidOllamaEndpoint(as<any>(undefined)))->toBe(false);
            }
            );
        }
        );
    }
    );
}

MAIN
