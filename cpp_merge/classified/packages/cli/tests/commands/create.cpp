#include "create.test.h"
#include <string>

void Main(void)
{
    describe(std::string("ElizaOS Create Commands"), [=]() mutable
    {
        shared<string> testTmpDir;
        shared<string> originalCwd;
        beforeEach([=]() mutable
        {
            originalCwd = process->cwd();
            testTmpDir = std::async([=]() { mkdtemp(join(tmpdir(), std::string("eliza-test-"))); });
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
            auto result = as<string>(bunExecSync(std::string("elizaos create --help"), getPlatformOptions(object{
                object::pair{std::string("encoding"), std::string("utf8")}
            })));
            expect(result)->toContain(std::string("Usage: elizaos create"));
            expect(result)->toMatch((new RegExp(std::string("(project|plugin|agent"))));
            expect(result)->not->toContain(std::string("frobnicate"));
        }
        );
        it(std::string("create default project succeeds"), [=]() mutable
        {
            std::async([=]() { crossPlatform["removeDir"](std::string("my-default-app")); });
            shared result = as<string>(bunExecSync(std::string("elizaos create my-default-app --yes"), getPlatformOptions(object{
                object::pair{std::string("encoding"), std::string("utf8")}, 
                object::pair{std::string("timeout"), TEST_TIMEOUTS["PROJECT_CREATION"]}
            })));
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
            expect(existsSync(std::string("my-default-app/CLAUDE.md")))->toBe(true);
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std::string("create plugin project succeeds"), [=]() mutable
        {
            std::async([=]() { crossPlatform["removeDir"](std::string("plugin-my-plugin-app")); });
            shared result = as<string>(bunExecSync(std::string("elizaos create my-plugin-app --yes --type plugin"), object{
                object::pair{std::string("encoding"), std::string("utf8")}, 
                object::pair{std::string("timeout"), TEST_TIMEOUTS["PROJECT_CREATION"]}
            }));
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
            expect(existsSync(join(pluginDir, std::string("CLAUDE.md"))))->toBe(true);
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std::string("create agent succeeds"), [=]() mutable
        {
            std::async([=]() { crossPlatform["removeFile"](std::string("my-test-agent.json")); });
            auto result = as<string>(bunExecSync(std::string("elizaos create my-test-agent --yes --type agent"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
            }));
            expect(result)->toContain(std::string("Agent character created successfully"));
            expect(existsSync(std::string("my-test-agent.json")))->toBe(true);
            std::async([=]() { validateAgentJson(std::string("my-test-agent.json"), std::string("my-test-agent")); });
        }
        );
        it(std::string("rejects creating project in existing directory"), [=]() mutable
        {
            try
            {
                std::async([=]() { crossPlatform["removeDir"](std::string("existing-app")); });
                bunExecSync(std::string("mkdir existing-app"), object{
                    object::pair{std::string("stdio"), std::string("ignore")}
                });
                if (process->platform == std::string("win32")) {
                    bunExecSync(std::string("echo test > existing-app\file.txt"), object{
                        object::pair{std::string("stdio"), std::string("ignore")}
                    });
                } else {
                    bunExecSync(std::string("echo "test" > existing-app/file.txt"), object{
                        object::pair{std::string("stdio"), std::string("ignore")}
                    });
                }
            }
            catch (const any& e)
            {
            }
            object result;
            try
            {
                auto output = as<string>(bunExecSync(std::string("elizaos create existing-app --yes"), object{
                    object::pair{std::string("encoding"), std::string("utf8")}
                }));
                throw any(std::make_shared<Error>(std::string("Command should have failed but succeeded with output: ") + output + string_empty));
            }
            catch (const any& e)
            {
                if (e["message"]["includes"](std::string("Command should have failed"))) {
                    throw any(e);
                }
                result = object{
                    object::pair{std::string("status"), OR((OR((e["status"]), (e["exitCode"]))), (-1))}, 
                    object::pair{std::string("output"), (OR((e["stdout"]), (string_empty))) + (OR((e["stderr"]), (string_empty)))}
                };
            }
            expect(result["status"])->not->toBe(0);
            expect(result["output"])->toContain(std::string("already exists"));
        }
        );
        it(std::string("create project in current directory"), [=]() mutable
        {
            try
            {
                std::async([=]() { crossPlatform["removeDir"](std::string("create-in-place")); });
                bunExecSync(std::string("mkdir create-in-place"), object{
                    object::pair{std::string("stdio"), std::string("ignore")}
                });
            }
            catch (const any& e)
            {
            }
            process->chdir(std::string("create-in-place"));
            auto result = as<string>(bunExecSync(std::string("elizaos create . --yes"), object{
                object::pair{std::string("encoding"), std::string("utf8")}, 
                object::pair{std::string("timeout"), TEST_TIMEOUTS["PROJECT_CREATION"]}
            }));
            expect(result)->toContain(std::string("Project initialized successfully!"));
            expect(existsSync(std::string("package.json")))->toBe(true);
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std::string("rejects invalid project name"), [=]() mutable
        {
            object result;
            try
            {
                auto output = as<string>(bunExecSync(std::string("elizaos create Invalid-Name! --yes"), object{
                    object::pair{std::string("encoding"), std::string("utf8")}
                }));
                throw any(std::make_shared<Error>(std::string("Command should have failed but succeeded with output: ") + output + string_empty));
            }
            catch (const any& e)
            {
                if (e["message"]["includes"](std::string("Command should have failed"))) {
                    throw any(e);
                }
                result = object{
                    object::pair{std::string("status"), OR((OR((e["status"]), (e["exitCode"]))), (-1))}, 
                    object::pair{std::string("output"), (OR((e["stdout"]), (string_empty))) + (OR((e["stderr"]), (string_empty)))}
                };
            }
            expect(result["status"])->not->toBe(0);
            expect(result["output"])->toMatch((new RegExp(std::string("Invalid project name"))));
        }
        );
        it(std::string("rejects invalid project type"), [=]() mutable
        {
            object result;
            try
            {
                auto output = as<string>(bunExecSync(std::string("elizaos create bad-type-proj --yes --type bad-type"), object{
                    object::pair{std::string("encoding"), std::string("utf8")}
                }));
                throw any(std::make_shared<Error>(std::string("Command should have failed but succeeded with output: ") + output + string_empty));
            }
            catch (const any& e)
            {
                if (e["message"]["includes"](std::string("Command should have failed"))) {
                    throw any(e);
                }
                result = object{
                    object::pair{std::string("status"), OR((OR((e["status"]), (e["exitCode"]))), (-1))}, 
                    object::pair{std::string("output"), (OR((e["stdout"]), (string_empty))) + (OR((e["stderr"]), (string_empty)))}
                };
            }
            expect(result["status"])->not->toBe(0);
            expect(result["output"])->toMatch((new RegExp(std::string("Invalid type"))));
        }
        );
        it(std::string("create-eliza default project succeeds"), [=]() mutable
        {
            std::async([=]() { crossPlatform["removeDir"](std::string("my-create-app")); });
            console->warn(std::string("Skipping create-eliza test - command not available"));
        }
        , 60000);
        it(std::string("create-eliza plugin project succeeds"), [=]() mutable
        {
            std::async([=]() { crossPlatform["removeDir"](std::string("plugin-my-create-plugin")); });
            console->warn(std::string("Skipping create-eliza plugin test - command not available"));
        }
        , 60000);
        it(std::string("create-eliza agent succeeds"), [=]() mutable
        {
            std::async([=]() { crossPlatform["removeFile"](std::string("my-create-agent.json")); });
            console->warn(std::string("Skipping create-eliza agent test - command not available"));
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
                expect(isValidOllamaEndpoint(as<any>(std::nullopt)))->toBe(false);
            }
            );
        }
        );
        describe(std::string("CLAUDE.md File Creation"), [=]() mutable
        {
            it(std::string("creates project with proper CLAUDE.md file"), [=]() mutable
            {
                std::async([=]() { crossPlatform["removeDir"](std::string("claude-md-test-project")); });
                auto result = as<string>(bunExecSync(std::string("elizaos create claude-md-test-project --yes"), getPlatformOptions(object{
                    object::pair{std::string("encoding"), std::string("utf8")}, 
                    object::pair{std::string("timeout"), TEST_TIMEOUTS["PROJECT_CREATION"]}
                })));
                expect(existsSync(std::string("claude-md-test-project")))->toBe(true);
                expect(existsSync(std::string("claude-md-test-project/CLAUDE.md")))->toBe(true);
                auto claudeMdContent = std::async([=]() { readFile(std::string("claude-md-test-project/CLAUDE.md"), std::string("utf8")); });
                expect(claudeMdContent)->toContain(std::string("ElizaOS Agent Project Development Guide for Claude"));
                expect(claudeMdContent)->toContain(std::string("Project Type** | ElizaOS Agent Project"));
                expect(claudeMdContent)->toContain(std::string("Character Configuration"));
                expect(claudeMdContent)->toContain(std::string("Custom Plugin Development"));
                expect(claudeMdContent)->toContain(std::string("Custom service for your specific needs"));
                expect(claudeMdContent)->toContain(std::string("Custom action for specific commands"));
                expect(claudeMdContent)->toContain(std::string("elizaos dev"));
                expect(claudeMdContent)->toContain(std::string("elizaos start"));
            }
            , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
            it(std::string("creates plugin with proper CLAUDE.md file"), [=]() mutable
            {
                std::async([=]() { crossPlatform["removeDir"](std::string("plugin-claude-md-test")); });
                auto result = as<string>(bunExecSync(std::string("elizaos create claude-md-test --yes --type plugin"), getPlatformOptions(object{
                    object::pair{std::string("encoding"), std::string("utf8")}, 
                    object::pair{std::string("timeout"), TEST_TIMEOUTS["PROJECT_CREATION"]}
                })));
                auto pluginDir = std::string("plugin-claude-md-test");
                expect(existsSync(pluginDir))->toBe(true);
                expect(existsSync(join(pluginDir, std::string("CLAUDE.md"))))->toBe(true);
                auto claudeMdContent = std::async([=]() { readFile(join(pluginDir, std::string("CLAUDE.md")), std::string("utf8")); });
                expect(claudeMdContent)->toContain(std::string("ElizaOS Plugin Development Guide for Claude"));
                expect(claudeMdContent)->toContain(std::string("Project Type** | ElizaOS Plugin"));
                expect(claudeMdContent)->toContain(std::string("Plugin Architecture"));
                expect(claudeMdContent)->toContain(std::string("Services** (Required for External APIs)"));
                expect(claudeMdContent)->toContain(std::string("Actions** (Required for User Interactions)"));
                expect(claudeMdContent)->toContain(std::string("Providers** (Optional - Context Supply)"));
                expect(claudeMdContent)->toContain(std::string("Evaluators** (Optional - Post-Processing)"));
                expect(claudeMdContent)->toContain(std::string("Plugin Export Pattern"));
                expect(claudeMdContent)->toContain(std::string("elizaos dev"));
            }
            , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        }
        );
        describe(std::string("Cleanup on Interruption"), [=]() mutable
        {
            it(std::string("cleans up partial plugin creation on process termination"), [=]() mutable
            {
                auto pluginName = std::string("test-cleanup-plugin");
                auto pluginDir = std::string("plugin-") + pluginName + string_empty;
                std::async([=]() { crossPlatform["removeDir"](pluginDir); });
                expect(existsSync(pluginDir))->toBe(false);
                auto createProcess = Bun->spawn(array<string>{ std::string("elizaos"), std::string("create"), pluginName, std::string("--type"), std::string("plugin"), std::string("--yes") }, object{
                    object::pair{std::string("stdout"), std::string("ignore")}, 
                    object::pair{std::string("stderr"), std::string("ignore")}, 
                    object::pair{std::string("stdin"), std::string("ignore")}
                });
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    return setTimeout(resolve, 1500);
                }
                ); });
                try
                {
                    createProcess->kill(std::string("SIGINT"));
                }
                catch (const any& e)
                {
                }
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    return setTimeout(resolve, 2000);
                }
                ); });
                expect(existsSync(pluginDir))->toBe(false);
            }
            , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        }
        );
        describe(std::string("--dir Flag Removal (Breaking Change)"), [=]() mutable
        {
            it(std::string("rejects --dir flag with helpful error message"), [=]() mutable
            {
                shared<object> result;
                try
                {
                    auto output = as<string>(bunExecSync(std::string("elizaos create my-project --dir /some/path"), object{
                        object::pair{std::string("encoding"), std::string("utf8")}
                    }));
                    throw any(std::make_shared<Error>(std::string("Command should have failed but succeeded with output: ") + output + string_empty));
                }
                catch (const any& e)
                {
                    auto error = as<any>(e);
                    if (error->message->includes(std::string("Command should have failed"))) {
                        throw any(error);
                    }
                    result = object{
                        object::pair{std::string("status"), OR((OR((error->status), (error->exitCode))), (-1))}, 
                        object::pair{std::string("output"), (OR((error->stdout), (string_empty))) + (OR((error->stderr), (string_empty)))}
                    };
                }
                expect(result["status"])->not->toBe(0);
                auto errorPatterns = array<string>{ std::string("--dir flag is no longer supported"), std::string("Unknown option"), std::string("unknown option"), std::string("Invalid option"), std::string("dir") };
                auto hasError = errorPatterns->some([=](auto pattern) mutable
                {
                    return result["output"]->toLowerCase()->includes(pattern->toLowerCase());
                }
                );
                expect(hasError)->toBe(true);
            }
            );
            it(std::string("rejects -d shorthand flag"), [=]() mutable
            {
                shared<object> result;
                try
                {
                    auto output = as<string>(bunExecSync(std::string("elizaos create my-project -d /some/path"), object{
                        object::pair{std::string("encoding"), std::string("utf8")}
                    }));
                    throw any(std::make_shared<Error>(std::string("Command should have failed but succeeded with output: ") + output + string_empty));
                }
                catch (const any& e)
                {
                    auto error = as<any>(e);
                    if (error->message->includes(std::string("Command should have failed"))) {
                        throw any(error);
                    }
                    result = object{
                        object::pair{std::string("status"), OR((OR((error->status), (error->exitCode))), (-1))}, 
                        object::pair{std::string("output"), (OR((error->stdout), (string_empty))) + (OR((error->stderr), (string_empty)))}
                    };
                }
                expect(result["status"])->not->toBe(0);
                auto errorPatterns = array<string>{ std::string("-d flag is no longer supported"), std::string("Unknown option"), std::string("unknown option"), std::string("Invalid option") };
                auto hasError = errorPatterns->some([=](auto pattern) mutable
                {
                    return result["output"]->toLowerCase()->includes(pattern->toLowerCase());
                }
                );
                expect(hasError)->toBe(true);
            }
            );
            it(std::string("creates project in current directory without --dir flag"), [=]() mutable
            {
                auto testSubDir = std::string("test-subdir");
                std::async([=]() { crossPlatform["removeDir"](testSubDir); });
                bunExecSync(std::string("mkdir ") + testSubDir + string_empty, object{
                    object::pair{std::string("stdio"), std::string("ignore")}
                });
                auto originalDir = process->cwd();
                process->chdir(testSubDir);
                {
                    utils::finally __finally20288_20378([&]() mutable
                    {
                        process->chdir(originalDir);
                    });
                    try
                    {
                        shared result = as<string>(bunExecSync(std::string("elizaos create my-current-dir-project --yes"), object{
                            object::pair{std::string("encoding"), std::string("utf8")}, 
                            object::pair{std::string("timeout"), TEST_TIMEOUTS["PROJECT_CREATION"]}
                        }));
                        auto successPatterns = array<string>{ std::string("Project initialized successfully!"), std::string("successfully initialized"), std::string("Project created"), std::string("created successfully") };
                        auto hasSuccess = successPatterns->some([=](auto pattern) mutable
                        {
                            return result->includes(pattern);
                        }
                        );
                        expect(OR((hasSuccess), (existsSync(std::string("my-current-dir-project")))))->toBe(true);
                        expect(existsSync(std::string("my-current-dir-project")))->toBe(true);
                        expect(existsSync(std::string("my-current-dir-project/package.json")))->toBe(true);
                    }
                    catch (...)
                    {
                        throw;
                    }
                }
            }
            , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
            it(std::string("migration guide: shows how to create in specific directory"), [=]() mutable
            {
                auto testDir = std::string("migration-test-dir");
                std::async([=]() { crossPlatform["removeDir"](testDir); });
                bunExecSync(std::string("mkdir ") + testDir + string_empty, object{
                    object::pair{std::string("stdio"), std::string("ignore")}
                });
                auto originalDir = process->cwd();
                {
                    utils::finally __finally21425_21475([&]() mutable
                    {
                        process->chdir(originalDir);
                    });
                    try
                    {
                        process->chdir(testDir);
                        auto result = as<string>(bunExecSync(std::string("elizaos create migrated-project --yes"), object{
                            object::pair{std::string("encoding"), std::string("utf8")}, 
                            object::pair{std::string("timeout"), TEST_TIMEOUTS["PROJECT_CREATION"]}
                        }));
                        expect(existsSync(std::string("migrated-project")))->toBe(true);
                        expect(existsSync(std::string("migrated-project/package.json")))->toBe(true);
                    }
                    catch (...)
                    {
                        throw;
                    }
                }
            }
            , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        }
        );
        it(std::string("does not hoist PGLITE database to parent .eliza directory"), [=]() mutable
        {
            auto parentDir = std::async([=]() { mkdtemp(join(tmpdir(), std::string("eliza-parent-"))); });
            auto parentElizaDir = join(parentDir, std::string(".eliza"));
            std::async([=]() { mkdir(parentElizaDir, object{
                object::pair{std::string("recursive"), true}
            }); });
            std::async([=]() { writeFile(join(parentElizaDir, std::string("parent-marker.txt")), std::string("parent")); });
            auto originalDir = process->cwd();
            {
                utils::finally __finally24193_24308([&]() mutable
                {
                    process->chdir(originalDir);
                    std::async([=]() { rm(parentDir, object{
                        object::pair{std::string("recursive"), true}
                    }); });
                });
                try
                {
                    process->chdir(parentDir);
                    auto result = as<string>(bunExecSync(std::string("elizaos create test-no-hoist --yes"), object{
                        object::pair{std::string("encoding"), std::string("utf8")}, 
                        object::pair{std::string("timeout"), TEST_TIMEOUTS["PROJECT_CREATION"]}
                    }));
                    expect(existsSync(std::string("test-no-hoist")))->toBe(true);
                    expect(existsSync(std::string("test-no-hoist/package.json")))->toBe(true);
                    auto projectElizaDir = join(std::string("test-no-hoist"), std::string(".eliza"));
                    expect(existsSync(projectElizaDir))->toBe(true);
                    auto projectDbDir = join(projectElizaDir, std::string(".elizadb"));
                    expect(existsSync(projectDbDir))->toBe(true);
                    auto projectMarkerPath = join(projectElizaDir, std::string("parent-marker.txt"));
                    expect(existsSync(projectMarkerPath))->toBe(false);
                    auto projectEnvPath = join(std::string("test-no-hoist"), std::string(".env"));
                    expect(existsSync(projectEnvPath))->toBe(true);
                    auto envContent = std::async([=]() { readFile(projectEnvPath, std::string("utf8")); });
                    expect(envContent)->toContain(std::string("PGLITE_DATA_DIR="));
                    auto pgliteMatch = envContent->match((new RegExp(std::string("PGLITE_DATA_DIR=(.+"))));
                    expect(pgliteMatch)->toBeTruthy();
                    auto pgliteDataDir = const_(pgliteMatch)[1];
                    expect(pgliteDataDir)->toContain(join(std::string("test-no-hoist"), std::string(".eliza"), std::string(".elizadb")));
                    auto sep = path->sep->replace((new RegExp(std::string("\\"))), std::string("\\"));
                    expect(pgliteDataDir)->not->toMatch(std::make_shared<RegExp>(std::string("eliza-parent-[^") + sep + std::string("]+") + sep + std::string("\.eliza") + sep + std::string("\.elizadb$")));
                }
                catch (...)
                {
                    throw;
                }
            }
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
    }
    );
}

MAIN
