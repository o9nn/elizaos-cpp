#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/tests/commands/update.test.h"

void Main(void)
{
    describe(std::string("ElizaOS Update Commands"), [=]() mutable
    {
        shared<string> testTmpDir;
        shared<string> elizaosCmd;
        shared<string> originalCwd;
        beforeEach([=]() mutable
        {
            originalCwd = process->cwd();
            testTmpDir = std::async([=]() { mkdtemp(join(tmpdir(), std::string("eliza-test-update-"))); });
            process->chdir(testTmpDir);
            auto scriptDir = join(__dirname, std::string(".."));
            elizaosCmd = std::string("bun ") + join(scriptDir, std::string("../dist/index.js")) + string_empty;
        }
        );
        afterEach([=]() mutable
        {
            safeChangeDirectory(originalCwd);
            if (AND((testTmpDir), (testTmpDir->includes(std::string("eliza-test-update-"))))) {
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
        shared makeProj = [=](auto name) mutable
        {
            runCliCommandSilently(elizaosCmd, std::string("create ") + name + std::string(" --yes"), object{
                object::pair{std::string("timeout"), TEST_TIMEOUTS["PROJECT_CREATION"]}
            });
            process->chdir(join(testTmpDir, name));
        };
        it(std::string("update --help shows usage and options"), [=]() mutable
        {
            auto result = execSync(string_empty + elizaosCmd + std::string(" update --help"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
            });
            expect(result)->toContain(std::string("Usage: elizaos update"));
            expect(result)->toContain(std::string("--cli"));
            expect(result)->toContain(std::string("--packages"));
            expect(result)->toContain(std::string("--check"));
            expect(result)->toContain(std::string("--skip-build"));
        }
        );
        it(std::string("update runs in a valid project"), [=]() mutable
        {
            std::async([=]() { makeProj(std::string("update-app")); });
            auto result = runCliCommandSilently(elizaosCmd, std::string("update"), object{
                object::pair{std::string("timeout"), TEST_TIMEOUTS["STANDARD_COMMAND"]}
            });
            expect(result)->toMatch((new RegExp(std::string("(Project successfully updated|Update completed|already up to date|No updates available"))));
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std::string("update --check works"), [=]() mutable
        {
            std::async([=]() { makeProj(std::string("update-check-app")); });
            auto result = runCliCommandSilently(elizaosCmd, std::string("update --check"), object{
                object::pair{std::string("timeout"), TEST_TIMEOUTS["STANDARD_COMMAND"]}
            });
            expect(result)->toMatch((new RegExp(std::string("Version: 1\."))));
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std::string("update --skip-build works"), [=]() mutable
        {
            std::async([=]() { makeProj(std::string("update-skip-build-app")); });
            auto result = runCliCommandSilently(elizaosCmd, std::string("update --skip-build"), object{
                object::pair{std::string("timeout"), TEST_TIMEOUTS["STANDARD_COMMAND"]}
            });
            expect(result)->not->toContain(std::string("Building project"));
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std::string("update --packages works"), [=]() mutable
        {
            std::async([=]() { makeProj(std::string("update-packages-app")); });
            auto result = runCliCommandSilently(elizaosCmd, std::string("update --packages"), object{
                object::pair{std::string("timeout"), TEST_TIMEOUTS["STANDARD_COMMAND"]}
            });
            expect(result)->toMatch((new RegExp(std::string("(Project successfully updated|Update completed|already up to date|No updates available"))));
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std::string("update --cli works outside a project"), [=]() mutable
        {
            auto result = runCliCommandSilently(elizaosCmd, std::string("update --cli"), object{
                object::pair{std::string("timeout"), TEST_TIMEOUTS["STANDARD_COMMAND"]}
            });
            expect(result)->toMatch((new RegExp(std::string("(Project successfully updated|Update completed|already up to date|No updates available|install the CLI globally|CLI update is not available"))));
        }
        , TEST_TIMEOUTS["STANDARD_COMMAND"]);
        it(std::string("update --cli --packages works"), [=]() mutable
        {
            std::async([=]() { makeProj(std::string("update-combined-app")); });
            auto result = runCliCommandSilently(elizaosCmd, std::string("update --cli --packages"), object{
                object::pair{std::string("timeout"), TEST_TIMEOUTS["STANDARD_COMMAND"]}
            });
            expect(result)->toMatch((new RegExp(std::string("(Project successfully updated|Update completed|already up to date|No updates available"))));
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std::string("update succeeds outside a project (global check)"), [=]() mutable
        {
            auto result = runCliCommandSilently(elizaosCmd, std::string("update"), object{
                object::pair{std::string("timeout"), TEST_TIMEOUTS["STANDARD_COMMAND"]}
            });
            expect(result)->toMatch((new RegExp(std::string("(Project successfully updated|Update completed|already up to date|No updates available|create a new ElizaOS project|This appears to be an empty directory"))));
        }
        , TEST_TIMEOUTS["STANDARD_COMMAND"]);
        it(std::string("update --packages shows helpful message in empty directory"), [=]() mutable
        {
            auto result = runCliCommandSilently(elizaosCmd, std::string("update --packages"), object{
                object::pair{std::string("timeout"), TEST_TIMEOUTS["STANDARD_COMMAND"]}
            });
            expect(result)->toContain(std::string("This directory doesn't appear to be an ElizaOS project"));
        }
        , TEST_TIMEOUTS["STANDARD_COMMAND"]);
        it(std::string("update --packages shows helpful message in non-elizaos project"), [=]() mutable
        {
            std::async([=]() { writeFile(std::string("package.json"), JSON->stringify(object{
                object::pair{std::string("name"), std::string("some-other-project")}, 
                object::pair{std::string("version"), std::string("1.0.0")}, 
                object::pair{std::string("dependencies"), object{
                    object::pair{std::string("express"), std::string("^4.18.0")}
                }}
            }, nullptr, 2)); });
            auto result = runCliCommandSilently(elizaosCmd, std::string("update --packages"), object{
                object::pair{std::string("timeout"), TEST_TIMEOUTS["STANDARD_COMMAND"]}
            });
            expect(result)->toContain(std::string("some-other-project"));
            expect(result)->toContain(std::string("elizaos create"));
        }
        , TEST_TIMEOUTS["STANDARD_COMMAND"]);
        it(std::string("update --packages works in elizaos project with dependencies"), [=]() mutable
        {
            std::async([=]() { makeProj(std::string("update-elizaos-project")); });
            std::async([=]() { writeFile(std::string("package.json"), JSON->stringify(object{
                object::pair{std::string("name"), std::string("test-elizaos-project")}, 
                object::pair{std::string("version"), std::string("1.0.0")}, 
                object::pair{std::string("dependencies"), object{
                    object::pair{std::string("@elizaos/core"), std::string("^1.0.0")}
                }}
            }, nullptr, 2)); });
            auto result = runCliCommandSilently(elizaosCmd, std::string("update --packages --check"), object{
                object::pair{std::string("timeout"), TEST_TIMEOUTS["STANDARD_COMMAND"]}
            });
            expect(result)->toContain(std::string("ElizaOS"));
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std::string("update --packages shows message for project without elizaos dependencies"), [=]() mutable
        {
            std::async([=]() { makeProj(std::string("update-no-deps-project")); });
            std::async([=]() { writeFile(std::string("package.json"), JSON->stringify(object{
                object::pair{std::string("name"), std::string("test-project")}, 
                object::pair{std::string("version"), std::string("1.0.0")}, 
                object::pair{std::string("eliza"), object{
                    object::pair{std::string("type"), std::string("project")}
                }}, 
                object::pair{std::string("dependencies"), object{
                    object::pair{std::string("express"), std::string("^4.18.0")}
                }}
            }, nullptr, 2)); });
            auto result = runCliCommandSilently(elizaosCmd, std::string("update --packages"), object{
                object::pair{std::string("timeout"), TEST_TIMEOUTS["STANDARD_COMMAND"]}
            });
            expect(result)->toContain(std::string("No ElizaOS packages found"));
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
    }
    );
}

MAIN
