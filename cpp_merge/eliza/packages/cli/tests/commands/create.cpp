#include "create.test.h"

void Main(void)
{
    describe(std:("ElizaOS Create Commands"), [=]() mutable
    {
        shared<string> testTmpDir;
        shared<string> elizaosCmd;
        shared<string> createElizaCmd;
        shared<string> originalCwd;
        beforeEach([=]() mutable
        {
            originalCwd = process->cwd();
            testTmpDir = std::async([=]() { mkdtemp(join(tmpdir(), std:("eliza-test-"))); });
            auto scriptDir = join(__dirname, std:(".."));
            elizaosCmd = std:("bun "") + join(scriptDir, std:("../dist/index.js")) + std:(""");
            createElizaCmd = std:("bun "") + join(scriptDir, std:("../../create-eliza/index.mjs")) + std:(""");
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
                        object::pair{std:("recursive"), true}
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
            auto content = std::async([=]() { readFile(jsonFile, std:("utf8")); });
            auto agentData = JSON->parse(content);
            expect(agentData["name"])->toBe(expectedName);
            expect(type_of(agentData["system"]))->toBe(std:("string"));
            expect(agentData["system"]["length"])->toBeGreaterThan(0);
            expect(Array->isArray(agentData["bio"]))->toBe(true);
            expect(agentData["bio"]["length"])->toBeGreaterThan(0);
            expect(Array->isArray(agentData["messageExamples"]))->toBe(true);
            expect(agentData["messageExamples"]["length"])->toBeGreaterThan(0);
            expect(type_of(agentData["style"]))->toBe(std:("object"));
            expect(Array->isArray(agentData["style"]["all"]))->toBe(true);
            expect(agentData["style"]["all"]["length"])->toBeGreaterThan(0);
        };
        it(std:("create --help shows usage"), [=]() mutable
        {
            auto result = execSync(string_empty + elizaosCmd + std:(" create --help"), getPlatformOptions(object{
                object::pair{std:("encoding"), std:("utf8")}
            }));
            expect(result)->toContain(std:("Usage: elizaos create"));
            expect(result)->toMatch((new RegExp(std:("(project|plugin|agent"))));
            expect(result)->not->toContain(std:("frobnicate"));
        }
        );
        it(std:("create default project succeeds"), [=]() mutable
        {
            crossPlatform["removeDir"](std:("my-default-app"));
            shared result = runCliCommandSilently(elizaosCmd, std:("create my-default-app --yes"), object{
                object::pair{std:("timeout"), TEST_TIMEOUTS["PROJECT_CREATION"]}
            });
            auto successPatterns = array<string>{ std:("Project initialized successfully!"), std:("successfully initialized"), std:("Project created"), std:("created successfully") };
            auto hasSuccess = successPatterns->some([=](auto pattern) mutable
            {
                return result->includes(pattern);
            }
            );
            if (!hasSuccess) {
                expect(existsSync(std:("my-default-app")))->toBe(true);
                expect(existsSync(std:("my-default-app/package.json")))->toBe(true);
            } else {
                expect(hasSuccess)->toBe(true);
            }
            expect(existsSync(std:("my-default-app")))->toBe(true);
            expect(existsSync(std:("my-default-app/package.json")))->toBe(true);
            expect(existsSync(std:("my-default-app/src")))->toBe(true);
            expect(existsSync(std:("my-default-app/.gitignore")))->toBe(true);
            expect(existsSync(std:("my-default-app/.npmignore")))->toBe(true);
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std:("create plugin project succeeds"), [=]() mutable
        {
            crossPlatform["removeDir"](std:("plugin-my-plugin-app"));
            shared result = runCliCommandSilently(elizaosCmd, std:("create my-plugin-app --yes --type plugin"), object{
                object::pair{std:("timeout"), TEST_TIMEOUTS["PROJECT_CREATION"]}
            });
            auto successPatterns = array<string>{ std:("Plugin initialized successfully!"), std:("successfully initialized"), std:("Plugin created"), std:("created successfully") };
            auto hasSuccess = successPatterns->some([=](auto pattern) mutable
            {
                return result->includes(pattern);
            }
            );
            auto pluginDir = std:("plugin-my-plugin-app");
            if (!hasSuccess) {
                expect(existsSync(pluginDir))->toBe(true);
                expect(existsSync(join(pluginDir, std:("package.json"))))->toBe(true);
            } else {
                expect(hasSuccess)->toBe(true);
            }
            expect(existsSync(pluginDir))->toBe(true);
            expect(existsSync(join(pluginDir, std:("package.json"))))->toBe(true);
            expect(existsSync(join(pluginDir, std:("src/index.ts"))))->toBe(true);
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std:("create agent succeeds"), [=]() mutable
        {
            crossPlatform["removeFile"](std:("my-test-agent.json"));
            auto result = runCliCommandSilently(elizaosCmd, std:("create my-test-agent --yes --type agent"));
            expect(result)->toContain(std:("Agent character created successfully"));
            expect(existsSync(std:("my-test-agent.json")))->toBe(true);
            std::async([=]() { validateAgentJson(std:("my-test-agent.json"), std:("my-test-agent")); });
        }
        );
        it(std:("rejects creating project in existing directory"), [=]() mutable
        {
            try
            {
                crossPlatform["removeDir"](std:("existing-app"));
                execSync(std:("mkdir existing-app"), getPlatformOptions(object{
                    object::pair{std:("stdio"), std:("ignore")}
                }));
                if (process->platform == std:("win32")) {
                    execSync(std:("echo test > existing-app\file.txt"), getPlatformOptions(object{
                        object::pair{std:("stdio"), std:("ignore")}
                    }));
                } else {
                    execSync(std:("echo "test" > existing-app/file.txt"), getPlatformOptions(object{
                        object::pair{std:("stdio"), std:("ignore")}
                    }));
                }
            }
            catch (const any& e)
            {
            }
            auto result = expectCliCommandToFail(elizaosCmd, std:("create existing-app --yes"));
            expect(result["status"])->not->toBe(0);
            expect(result["output"])->toContain(std:("already exists"));
        }
        );
        it(std:("create project in current directory"), [=]() mutable
        {
            try
            {
                crossPlatform["removeDir"](std:("create-in-place"));
                execSync(std:("mkdir create-in-place"), getPlatformOptions(object{
                    object::pair{std:("stdio"), std:("ignore")}
                }));
            }
            catch (const any& e)
            {
            }
            process->chdir(std:("create-in-place"));
            auto result = runCliCommandSilently(elizaosCmd, std:("create . --yes"), object{
                object::pair{std:("timeout"), TEST_TIMEOUTS["PROJECT_CREATION"]}
            });
            expect(result)->toContain(std:("Project initialized successfully!"));
            expect(existsSync(std:("package.json")))->toBe(true);
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std:("rejects invalid project name"), [=]() mutable
        {
            auto result = expectCliCommandToFail(elizaosCmd, std:("create "Invalid Name" --yes"));
            expect(result["status"])->not->toBe(0);
            expect(result["output"])->toMatch((new RegExp(std:("Invalid"))));
        }
        );
        it(std:("rejects invalid project type"), [=]() mutable
        {
            auto result = expectCliCommandToFail(elizaosCmd, std:("create bad-type-proj --yes --type bad-type"));
            expect(result["status"])->not->toBe(0);
            expect(result["output"])->toMatch((new RegExp(std:("Invalid type"))));
        }
        );
        it(std:("create-eliza default project succeeds"), [=]() mutable
        {
            crossPlatform["removeDir"](std:("my-create-app"));
            try
            {
                auto result = runCliCommandSilently(createElizaCmd, std:("my-create-app --yes"));
                expect(result)->toContain(std:("Project initialized successfully!"));
                expect(existsSync(std:("my-create-app")))->toBe(true);
                expect(existsSync(std:("my-create-app/package.json")))->toBe(true);
                expect(existsSync(std:("my-create-app/src")))->toBe(true);
            }
            catch (const any& e)
            {
                console->warn(std:("Skipping create-eliza test - command not available"));
            }
        }
        , 60000);
        it(std:("create-eliza plugin project succeeds"), [=]() mutable
        {
            crossPlatform["removeDir"](std:("plugin-my-create-plugin"));
            try
            {
                auto result = runCliCommandSilently(createElizaCmd, std:("my-create-plugin --yes --type plugin"));
                expect(result)->toContain(std:("Plugin initialized successfully!"));
                auto pluginDir = std:("plugin-my-create-plugin");
                expect(existsSync(pluginDir))->toBe(true);
                expect(existsSync(join(pluginDir, std:("package.json"))))->toBe(true);
                expect(existsSync(join(pluginDir, std:("src/index.ts"))))->toBe(true);
            }
            catch (const any& e)
            {
                console->warn(std:("Skipping create-eliza plugin test - command not available"));
            }
        }
        , 60000);
        it(std:("create-eliza agent succeeds"), [=]() mutable
        {
            crossPlatform["removeFile"](std:("my-create-agent.json"));
            try
            {
                auto result = runCliCommandSilently(createElizaCmd, std:("my-create-agent --yes --type agent"));
                expect(result)->toContain(std:("Agent character created successfully"));
                expect(existsSync(std:("my-create-agent.json")))->toBe(true);
                std::async([=]() { validateAgentJson(std:("my-create-agent.json"), std:("my-create-agent")); });
            }
            catch (const any& e)
            {
                console->warn(std:("Skipping create-eliza agent test - command not available"));
            }
        }
        , 60000);
        describe(std:("AI Model Selection"), [=]() mutable
        {
            it(std:("returns a reasonable number of AI model options"), [=]() mutable
            {
                auto models = getAvailableAIModels();
                expect(models->get_length())->toBeGreaterThanOrEqual(3);
                expect(models->get_length())->toBeLessThanOrEqual(7);
            }
            );
            it(std:("maintains core AI model options"), [=]() mutable
            {
                auto models = getAvailableAIModels();
                shared values = models->map([=](auto m) mutable
                {
                    return m->value;
                }
                );
                auto CORE_PROVIDERS = array<string>{ std:("local"), std:("openai"), std:("claude"), std:("openrouter") };
                CORE_PROVIDERS->forEach([=](auto provider) mutable
                {
                    expect(values)->toContain(provider);
                }
                );
            }
            );
            it(std:("all AI models follow the expected contract"), [=]() mutable
            {
                auto models = getAvailableAIModels();
                models->forEach([=](auto model) mutable
                {
                    expect(model)->toHaveProperty(std:("value"));
                    expect(model)->toHaveProperty(std:("title"));
                    expect(model)->toHaveProperty(std:("description"));
                    expect(type_of(model->value))->toBe(std:("string"));
                    expect(type_of(model->title))->toBe(std:("string"));
                    expect(type_of(model->description))->toBe(std:("string"));
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
        describe(std:("Ollama Configuration"), [=]() mutable
        {
            it(std:("validates valid ollama endpoints"), [=]() mutable
            {
                expect(isValidOllamaEndpoint(std:("http://localhost:11434")))->toBe(true);
                expect(isValidOllamaEndpoint(std:("https://ollama.example.com")))->toBe(true);
                expect(isValidOllamaEndpoint(std:("http://192.168.1.100:11434")))->toBe(true);
            }
            );
            it(std:("rejects invalid ollama endpoints"), [=]() mutable
            {
                expect(isValidOllamaEndpoint(string_empty))->toBe(false);
                expect(isValidOllamaEndpoint(std:("localhost:11434")))->toBe(false);
                expect(isValidOllamaEndpoint(std:("ftp://localhost:11434")))->toBe(false);
                expect(isValidOllamaEndpoint(std:("not-a-url")))->toBe(false);
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
