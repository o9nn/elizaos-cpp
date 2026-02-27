#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/tests/commands/create.test.h"

void Main(void)
{
    describe(std:("ElizaOS Create Commands"), [=]() mutable
    {
        shared<string> testTmpDir;
        shared<string> originalCwd;
        beforeEach([=]() mutable
        {
            originalCwd = process->cwd();
            testTmpDir = std::async([=]() { mkdtemp(join(tmpdir(), std:("eliza-test-"))); });
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
            auto result = as<string>(bunExecSync(std:("elizaos create --help"), getPlatformOptions(object{
                object::pair{std:("encoding"), std:("utf8")}
            })));
            expect(result)->toContain(std:("Usage: elizaos create"));
            expect(result)->toMatch((new RegExp(std:("(project|plugin|agent"))));
            expect(result)->not->toContain(std:("frobnicate"));
        }
        );
        it(std:("create default project succeeds"), [=]() mutable
        {
            std::async([=]() { crossPlatform["removeDir"](std:("my-default-app")); });
            shared result = as<string>(bunExecSync(std:("elizaos create my-default-app --yes"), getPlatformOptions(object{
                object::pair{std:("encoding"), std:("utf8")}, 
                object::pair{std:("timeout"), TEST_TIMEOUTS["PROJECT_CREATION"]}
            })));
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
            expect(existsSync(std:("my-default-app/CLAUDE.md")))->toBe(true);
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std:("create plugin project succeeds"), [=]() mutable
        {
            std::async([=]() { crossPlatform["removeDir"](std:("plugin-my-plugin-app")); });
            shared result = as<string>(bunExecSync(std:("elizaos create my-plugin-app --yes --type plugin"), object{
                object::pair{std:("encoding"), std:("utf8")}, 
                object::pair{std:("timeout"), TEST_TIMEOUTS["PROJECT_CREATION"]}
            }));
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
            expect(existsSync(join(pluginDir, std:("CLAUDE.md"))))->toBe(true);
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std:("create agent succeeds"), [=]() mutable
        {
            std::async([=]() { crossPlatform["removeFile"](std:("my-test-agent.json")); });
            auto result = as<string>(bunExecSync(std:("elizaos create my-test-agent --yes --type agent"), object{
                object::pair{std:("encoding"), std:("utf8")}
            }));
            expect(result)->toContain(std:("Agent character created successfully"));
            expect(existsSync(std:("my-test-agent.json")))->toBe(true);
            std::async([=]() { validateAgentJson(std:("my-test-agent.json"), std:("my-test-agent")); });
        }
        );
        it(std:("rejects creating project in existing directory"), [=]() mutable
        {
            try
            {
                std::async([=]() { crossPlatform["removeDir"](std:("existing-app")); });
                bunExecSync(std:("mkdir existing-app"), object{
                    object::pair{std:("stdio"), std:("ignore")}
                });
                if (process->platform == std:("win32")) {
                    bunExecSync(std:("echo test > existing-app\file.txt"), object{
                        object::pair{std:("stdio"), std:("ignore")}
                    });
                } else {
                    bunExecSync(std:("echo "test" > existing-app/file.txt"), object{
                        object::pair{std:("stdio"), std:("ignore")}
                    });
                }
            }
            catch (const any& e)
            {
            }
            object result;
            try
            {
                auto output = as<string>(bunExecSync(std:("elizaos create existing-app --yes"), object{
                    object::pair{std:("encoding"), std:("utf8")}
                }));
                throw any(std::make_shared<Error>(std:("Command should have failed but succeeded with output: ") + output + string_empty));
            }
            catch (const any& e)
            {
                if (e["message"]["includes"](std:("Command should have failed"))) {
                    throw any(e);
                }
                result = object{
                    object::pair{std:("status"), OR((OR((e["status"]), (e["exitCode"]))), (-1))}, 
                    object::pair{std:("output"), (OR((e["stdout"]), (string_empty))) + (OR((e["stderr"]), (string_empty)))}
                };
            }
            expect(result["status"])->not->toBe(0);
            expect(result["output"])->toContain(std:("already exists"));
        }
        );
        it(std:("create project in current directory"), [=]() mutable
        {
            try
            {
                std::async([=]() { crossPlatform["removeDir"](std:("create-in-place")); });
                bunExecSync(std:("mkdir create-in-place"), object{
                    object::pair{std:("stdio"), std:("ignore")}
                });
            }
            catch (const any& e)
            {
            }
            process->chdir(std:("create-in-place"));
            auto result = as<string>(bunExecSync(std:("elizaos create . --yes"), object{
                object::pair{std:("encoding"), std:("utf8")}, 
                object::pair{std:("timeout"), TEST_TIMEOUTS["PROJECT_CREATION"]}
            }));
            expect(result)->toContain(std:("Project initialized successfully!"));
            expect(existsSync(std:("package.json")))->toBe(true);
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std:("rejects invalid project name"), [=]() mutable
        {
            object result;
            try
            {
                auto output = as<string>(bunExecSync(std:("elizaos create Invalid-Name! --yes"), object{
                    object::pair{std:("encoding"), std:("utf8")}
                }));
                throw any(std::make_shared<Error>(std:("Command should have failed but succeeded with output: ") + output + string_empty));
            }
            catch (const any& e)
            {
                if (e["message"]["includes"](std:("Command should have failed"))) {
                    throw any(e);
                }
                result = object{
                    object::pair{std:("status"), OR((OR((e["status"]), (e["exitCode"]))), (-1))}, 
                    object::pair{std:("output"), (OR((e["stdout"]), (string_empty))) + (OR((e["stderr"]), (string_empty)))}
                };
            }
            expect(result["status"])->not->toBe(0);
            expect(result["output"])->toMatch((new RegExp(std:("Invalid project name"))));
        }
        );
        it(std:("rejects invalid project type"), [=]() mutable
        {
            object result;
            try
            {
                auto output = as<string>(bunExecSync(std:("elizaos create bad-type-proj --yes --type bad-type"), object{
                    object::pair{std:("encoding"), std:("utf8")}
                }));
                throw any(std::make_shared<Error>(std:("Command should have failed but succeeded with output: ") + output + string_empty));
            }
            catch (const any& e)
            {
                if (e["message"]["includes"](std:("Command should have failed"))) {
                    throw any(e);
                }
                result = object{
                    object::pair{std:("status"), OR((OR((e["status"]), (e["exitCode"]))), (-1))}, 
                    object::pair{std:("output"), (OR((e["stdout"]), (string_empty))) + (OR((e["stderr"]), (string_empty)))}
                };
            }
            expect(result["status"])->not->toBe(0);
            expect(result["output"])->toMatch((new RegExp(std:("Invalid type"))));
        }
        );
        it(std:("create-eliza default project succeeds"), [=]() mutable
        {
            std::async([=]() { crossPlatform["removeDir"](std:("my-create-app")); });
            console->warn(std:("Skipping create-eliza test - command not available"));
        }
        , 60000);
        it(std:("create-eliza plugin project succeeds"), [=]() mutable
        {
            std::async([=]() { crossPlatform["removeDir"](std:("plugin-my-create-plugin")); });
            console->warn(std:("Skipping create-eliza plugin test - command not available"));
        }
        , 60000);
        it(std:("create-eliza agent succeeds"), [=]() mutable
        {
            std::async([=]() { crossPlatform["removeFile"](std:("my-create-agent.json")); });
            console->warn(std:("Skipping create-eliza agent test - command not available"));
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
        describe(std:("CLAUDE.md File Creation"), [=]() mutable
        {
            it(std:("creates project with proper CLAUDE.md file"), [=]() mutable
            {
                std::async([=]() { crossPlatform["removeDir"](std:("claude-md-test-project")); });
                auto result = as<string>(bunExecSync(std:("elizaos create claude-md-test-project --yes"), getPlatformOptions(object{
                    object::pair{std:("encoding"), std:("utf8")}, 
                    object::pair{std:("timeout"), TEST_TIMEOUTS["PROJECT_CREATION"]}
                })));
                expect(existsSync(std:("claude-md-test-project")))->toBe(true);
                expect(existsSync(std:("claude-md-test-project/CLAUDE.md")))->toBe(true);
                auto claudeMdContent = std::async([=]() { readFile(std:("claude-md-test-project/CLAUDE.md"), std:("utf8")); });
                expect(claudeMdContent)->toContain(std:("ElizaOS Agent Project Development Guide for Claude"));
                expect(claudeMdContent)->toContain(std:("Project Type** | ElizaOS Agent Project"));
                expect(claudeMdContent)->toContain(std:("Character Configuration"));
                expect(claudeMdContent)->toContain(std:("Custom Plugin Development"));
                expect(claudeMdContent)->toContain(std:("Custom service for your specific needs"));
                expect(claudeMdContent)->toContain(std:("Custom action for specific commands"));
                expect(claudeMdContent)->toContain(std:("elizaos dev"));
                expect(claudeMdContent)->toContain(std:("elizaos start"));
            }
            , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
            it(std:("creates plugin with proper CLAUDE.md file"), [=]() mutable
            {
                std::async([=]() { crossPlatform["removeDir"](std:("plugin-claude-md-test")); });
                auto result = as<string>(bunExecSync(std:("elizaos create claude-md-test --yes --type plugin"), getPlatformOptions(object{
                    object::pair{std:("encoding"), std:("utf8")}, 
                    object::pair{std:("timeout"), TEST_TIMEOUTS["PROJECT_CREATION"]}
                })));
                auto pluginDir = std:("plugin-claude-md-test");
                expect(existsSync(pluginDir))->toBe(true);
                expect(existsSync(join(pluginDir, std:("CLAUDE.md"))))->toBe(true);
                auto claudeMdContent = std::async([=]() { readFile(join(pluginDir, std:("CLAUDE.md")), std:("utf8")); });
                expect(claudeMdContent)->toContain(std:("ElizaOS Plugin Development Guide for Claude"));
                expect(claudeMdContent)->toContain(std:("Project Type** | ElizaOS Plugin"));
                expect(claudeMdContent)->toContain(std:("Plugin Architecture"));
                expect(claudeMdContent)->toContain(std:("Services** (Required for External APIs)"));
                expect(claudeMdContent)->toContain(std:("Actions** (Required for User Interactions)"));
                expect(claudeMdContent)->toContain(std:("Providers** (Optional - Context Supply)"));
                expect(claudeMdContent)->toContain(std:("Evaluators** (Optional - Post-Processing)"));
                expect(claudeMdContent)->toContain(std:("Plugin Export Pattern"));
                expect(claudeMdContent)->toContain(std:("elizaos dev"));
            }
            , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        }
        );
        describe(std:("Cleanup on Interruption"), [=]() mutable
        {
            it(std:("cleans up partial plugin creation on process termination"), [=]() mutable
            {
                auto pluginName = std:("test-cleanup-plugin");
                auto pluginDir = std:("plugin-") + pluginName + string_empty;
                std::async([=]() { crossPlatform["removeDir"](pluginDir); });
                expect(existsSync(pluginDir))->toBe(false);
                auto createProcess = Bun->spawn(array<string>{ std:("elizaos"), std:("create"), pluginName, std:("--type"), std:("plugin"), std:("--yes") }, object{
                    object::pair{std:("stdout"), std:("ignore")}, 
                    object::pair{std:("stderr"), std:("ignore")}, 
                    object::pair{std:("stdin"), std:("ignore")}
                });
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    return setTimeout(resolve, 1500);
                }
                ); });
                try
                {
                    createProcess->kill(std:("SIGINT"));
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
        describe(std:("--dir Flag Removal (Breaking Change)"), [=]() mutable
        {
            it(std:("rejects --dir flag with helpful error message"), [=]() mutable
            {
                shared<object> result;
                try
                {
                    auto output = as<string>(bunExecSync(std:("elizaos create my-project --dir /some/path"), object{
                        object::pair{std:("encoding"), std:("utf8")}
                    }));
                    throw any(std::make_shared<Error>(std:("Command should have failed but succeeded with output: ") + output + string_empty));
                }
                catch (const any& e)
                {
                    auto error = as<any>(e);
                    if (error->message->includes(std:("Command should have failed"))) {
                        throw any(error);
                    }
                    result = object{
                        object::pair{std:("status"), OR((OR((error->status), (error->exitCode))), (-1))}, 
                        object::pair{std:("output"), (OR((error->stdout), (string_empty))) + (OR((error->stderr), (string_empty)))}
                    };
                }
                expect(result["status"])->not->toBe(0);
                auto errorPatterns = array<string>{ std:("--dir flag is no longer supported"), std:("Unknown option"), std:("unknown option"), std:("Invalid option"), std:("dir") };
                auto hasError = errorPatterns->some([=](auto pattern) mutable
                {
                    return result["output"]->toLowerCase()->includes(pattern->toLowerCase());
                }
                );
                expect(hasError)->toBe(true);
            }
            );
            it(std:("rejects -d shorthand flag"), [=]() mutable
            {
                shared<object> result;
                try
                {
                    auto output = as<string>(bunExecSync(std:("elizaos create my-project -d /some/path"), object{
                        object::pair{std:("encoding"), std:("utf8")}
                    }));
                    throw any(std::make_shared<Error>(std:("Command should have failed but succeeded with output: ") + output + string_empty));
                }
                catch (const any& e)
                {
                    auto error = as<any>(e);
                    if (error->message->includes(std:("Command should have failed"))) {
                        throw any(error);
                    }
                    result = object{
                        object::pair{std:("status"), OR((OR((error->status), (error->exitCode))), (-1))}, 
                        object::pair{std:("output"), (OR((error->stdout), (string_empty))) + (OR((error->stderr), (string_empty)))}
                    };
                }
                expect(result["status"])->not->toBe(0);
                auto errorPatterns = array<string>{ std:("-d flag is no longer supported"), std:("Unknown option"), std:("unknown option"), std:("Invalid option") };
                auto hasError = errorPatterns->some([=](auto pattern) mutable
                {
                    return result["output"]->toLowerCase()->includes(pattern->toLowerCase());
                }
                );
                expect(hasError)->toBe(true);
            }
            );
            it(std:("creates project in current directory without --dir flag"), [=]() mutable
            {
                auto testSubDir = std:("test-subdir");
                std::async([=]() { crossPlatform["removeDir"](testSubDir); });
                bunExecSync(std:("mkdir ") + testSubDir + string_empty, object{
                    object::pair{std:("stdio"), std:("ignore")}
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
                        shared result = as<string>(bunExecSync(std:("elizaos create my-current-dir-project --yes"), object{
                            object::pair{std:("encoding"), std:("utf8")}, 
                            object::pair{std:("timeout"), TEST_TIMEOUTS["PROJECT_CREATION"]}
                        }));
                        auto successPatterns = array<string>{ std:("Project initialized successfully!"), std:("successfully initialized"), std:("Project created"), std:("created successfully") };
                        auto hasSuccess = successPatterns->some([=](auto pattern) mutable
                        {
                            return result->includes(pattern);
                        }
                        );
                        expect(OR((hasSuccess), (existsSync(std:("my-current-dir-project")))))->toBe(true);
                        expect(existsSync(std:("my-current-dir-project")))->toBe(true);
                        expect(existsSync(std:("my-current-dir-project/package.json")))->toBe(true);
                    }
                    catch (...)
                    {
                        throw;
                    }
                }
            }
            , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
            it(std:("migration guide: shows how to create in specific directory"), [=]() mutable
            {
                auto testDir = std:("migration-test-dir");
                std::async([=]() { crossPlatform["removeDir"](testDir); });
                bunExecSync(std:("mkdir ") + testDir + string_empty, object{
                    object::pair{std:("stdio"), std:("ignore")}
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
                        auto result = as<string>(bunExecSync(std:("elizaos create migrated-project --yes"), object{
                            object::pair{std:("encoding"), std:("utf8")}, 
                            object::pair{std:("timeout"), TEST_TIMEOUTS["PROJECT_CREATION"]}
                        }));
                        expect(existsSync(std:("migrated-project")))->toBe(true);
                        expect(existsSync(std:("migrated-project/package.json")))->toBe(true);
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
        it(std:("does not hoist PGLITE database to parent .eliza directory"), [=]() mutable
        {
            auto parentDir = std::async([=]() { mkdtemp(join(tmpdir(), std:("eliza-parent-"))); });
            auto parentElizaDir = join(parentDir, std:(".eliza"));
            std::async([=]() { mkdir(parentElizaDir, object{
                object::pair{std:("recursive"), true}
            }); });
            std::async([=]() { writeFile(join(parentElizaDir, std:("parent-marker.txt")), std:("parent")); });
            auto originalDir = process->cwd();
            {
                utils::finally __finally24193_24308([&]() mutable
                {
                    process->chdir(originalDir);
                    std::async([=]() { rm(parentDir, object{
                        object::pair{std:("recursive"), true}
                    }); });
                });
                try
                {
                    process->chdir(parentDir);
                    auto result = as<string>(bunExecSync(std:("elizaos create test-no-hoist --yes"), object{
                        object::pair{std:("encoding"), std:("utf8")}, 
                        object::pair{std:("timeout"), TEST_TIMEOUTS["PROJECT_CREATION"]}
                    }));
                    expect(existsSync(std:("test-no-hoist")))->toBe(true);
                    expect(existsSync(std:("test-no-hoist/package.json")))->toBe(true);
                    auto projectElizaDir = join(std:("test-no-hoist"), std:(".eliza"));
                    expect(existsSync(projectElizaDir))->toBe(true);
                    auto projectDbDir = join(projectElizaDir, std:(".elizadb"));
                    expect(existsSync(projectDbDir))->toBe(true);
                    auto projectMarkerPath = join(projectElizaDir, std:("parent-marker.txt"));
                    expect(existsSync(projectMarkerPath))->toBe(false);
                    auto projectEnvPath = join(std:("test-no-hoist"), std:(".env"));
                    expect(existsSync(projectEnvPath))->toBe(true);
                    auto envContent = std::async([=]() { readFile(projectEnvPath, std:("utf8")); });
                    expect(envContent)->toContain(std:("PGLITE_DATA_DIR="));
                    auto pgliteMatch = envContent->match((new RegExp(std:("PGLITE_DATA_DIR=(.+"))));
                    expect(pgliteMatch)->toBeTruthy();
                    auto pgliteDataDir = const_(pgliteMatch)[1];
                    expect(pgliteDataDir)->toContain(join(std:("test-no-hoist"), std:(".eliza"), std:(".elizadb")));
                    auto sep = path->sep->replace((new RegExp(std:("\\"))), std:("\\"));
                    expect(pgliteDataDir)->not->toMatch(std::make_shared<RegExp>(std:("eliza-parent-[^") + sep + std:("]+") + sep + std:("\.eliza") + sep + std:("\.elizadb$")));
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
