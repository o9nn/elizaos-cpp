#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/tests/commands/update.test.h"

void Main(void)
{
    describe(std:("ElizaOS Update Commands"), [=]() mutable
    {
        shared<string> testTmpDir;
        shared<string> elizaosCmd;
        shared<string> originalCwd;
        beforeEach([=]() mutable
        {
            originalCwd = process->cwd();
            testTmpDir = std::async([=]() { mkdtemp(join(tmpdir(), std:("eliza-test-update-"))); });
            process->chdir(testTmpDir);
            auto scriptDir = join(__dirname, std:(".."));
            elizaosCmd = std:("bun ") + join(scriptDir, std:("../dist/index.js")) + string_empty;
        }
        );
        afterEach([=]() mutable
        {
            safeChangeDirectory(originalCwd);
            if (AND((testTmpDir), (testTmpDir->includes(std:("eliza-test-update-"))))) {
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
        shared makeProj = [=](auto name) mutable
        {
            runCliCommandSilently(elizaosCmd, std:("create ") + name + std:(" --yes"), object{
                object::pair{std:("timeout"), TEST_TIMEOUTS["PROJECT_CREATION"]}
            });
            process->chdir(join(testTmpDir, name));
        };
        it(std:("update --help shows usage and options"), [=]() mutable
        {
            auto result = execSync(string_empty + elizaosCmd + std:(" update --help"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(result)->toContain(std:("Usage: elizaos update"));
            expect(result)->toContain(std:("--cli"));
            expect(result)->toContain(std:("--packages"));
            expect(result)->toContain(std:("--check"));
            expect(result)->toContain(std:("--skip-build"));
        }
        );
        it(std:("update runs in a valid project"), [=]() mutable
        {
            std::async([=]() { makeProj(std:("update-app")); });
            auto result = runCliCommandSilently(elizaosCmd, std:("update"), object{
                object::pair{std:("timeout"), TEST_TIMEOUTS["STANDARD_COMMAND"]}
            });
            expect(result)->toMatch((new RegExp(std:("(Project successfully updated|Update completed|already up to date|No updates available"))));
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std:("update --check works"), [=]() mutable
        {
            std::async([=]() { makeProj(std:("update-check-app")); });
            auto result = runCliCommandSilently(elizaosCmd, std:("update --check"), object{
                object::pair{std:("timeout"), TEST_TIMEOUTS["STANDARD_COMMAND"]}
            });
            expect(result)->toMatch((new RegExp(std:("Version: 1\."))));
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std:("update --skip-build works"), [=]() mutable
        {
            std::async([=]() { makeProj(std:("update-skip-build-app")); });
            auto result = runCliCommandSilently(elizaosCmd, std:("update --skip-build"), object{
                object::pair{std:("timeout"), TEST_TIMEOUTS["STANDARD_COMMAND"]}
            });
            expect(result)->not->toContain(std:("Building project"));
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std:("update --packages works"), [=]() mutable
        {
            std::async([=]() { makeProj(std:("update-packages-app")); });
            auto result = runCliCommandSilently(elizaosCmd, std:("update --packages"), object{
                object::pair{std:("timeout"), TEST_TIMEOUTS["STANDARD_COMMAND"]}
            });
            expect(result)->toMatch((new RegExp(std:("(Project successfully updated|Update completed|already up to date|No updates available"))));
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std:("update --cli works outside a project"), [=]() mutable
        {
            auto result = runCliCommandSilently(elizaosCmd, std:("update --cli"), object{
                object::pair{std:("timeout"), TEST_TIMEOUTS["STANDARD_COMMAND"]}
            });
            expect(result)->toMatch((new RegExp(std:("(Project successfully updated|Update completed|already up to date|No updates available|install the CLI globally|CLI update is not available"))));
        }
        , TEST_TIMEOUTS["STANDARD_COMMAND"]);
        it(std:("update --cli --packages works"), [=]() mutable
        {
            std::async([=]() { makeProj(std:("update-combined-app")); });
            auto result = runCliCommandSilently(elizaosCmd, std:("update --cli --packages"), object{
                object::pair{std:("timeout"), TEST_TIMEOUTS["STANDARD_COMMAND"]}
            });
            expect(result)->toMatch((new RegExp(std:("(Project successfully updated|Update completed|already up to date|No updates available"))));
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std:("update succeeds outside a project (global check)"), [=]() mutable
        {
            auto result = runCliCommandSilently(elizaosCmd, std:("update"), object{
                object::pair{std:("timeout"), TEST_TIMEOUTS["STANDARD_COMMAND"]}
            });
            expect(result)->toMatch((new RegExp(std:("(Project successfully updated|Update completed|already up to date|No updates available|create a new ElizaOS project|This appears to be an empty directory"))));
        }
        , TEST_TIMEOUTS["STANDARD_COMMAND"]);
        it(std:("update --packages shows helpful message in empty directory"), [=]() mutable
        {
            auto result = runCliCommandSilently(elizaosCmd, std:("update --packages"), object{
                object::pair{std:("timeout"), TEST_TIMEOUTS["STANDARD_COMMAND"]}
            });
            expect(result)->toContain(std:("This directory doesn't appear to be an ElizaOS project"));
        }
        , TEST_TIMEOUTS["STANDARD_COMMAND"]);
        it(std:("update --packages shows helpful message in non-elizaos project"), [=]() mutable
        {
            std::async([=]() { writeFile(std:("package.json"), JSON->stringify(object{
                object::pair{std:("name"), std:("some-other-project")}, 
                object::pair{std:("version"), std:("1.0.0")}, 
                object::pair{std:("dependencies"), object{
                    object::pair{std:("express"), std:("^4.18.0")}
                }}
            }, nullptr, 2)); });
            auto result = runCliCommandSilently(elizaosCmd, std:("update --packages"), object{
                object::pair{std:("timeout"), TEST_TIMEOUTS["STANDARD_COMMAND"]}
            });
            expect(result)->toContain(std:("some-other-project"));
            expect(result)->toContain(std:("elizaos create"));
        }
        , TEST_TIMEOUTS["STANDARD_COMMAND"]);
        it(std:("update --packages works in elizaos project with dependencies"), [=]() mutable
        {
            std::async([=]() { makeProj(std:("update-elizaos-project")); });
            std::async([=]() { writeFile(std:("package.json"), JSON->stringify(object{
                object::pair{std:("name"), std:("test-elizaos-project")}, 
                object::pair{std:("version"), std:("1.0.0")}, 
                object::pair{std:("dependencies"), object{
                    object::pair{std:("@elizaos/core"), std:("^1.0.0")}
                }}
            }, nullptr, 2)); });
            auto result = runCliCommandSilently(elizaosCmd, std:("update --packages --check"), object{
                object::pair{std:("timeout"), TEST_TIMEOUTS["STANDARD_COMMAND"]}
            });
            expect(result)->toContain(std:("ElizaOS"));
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std:("update --packages shows message for project without elizaos dependencies"), [=]() mutable
        {
            std::async([=]() { makeProj(std:("update-no-deps-project")); });
            std::async([=]() { writeFile(std:("package.json"), JSON->stringify(object{
                object::pair{std:("name"), std:("test-project")}, 
                object::pair{std:("version"), std:("1.0.0")}, 
                object::pair{std:("eliza"), object{
                    object::pair{std:("type"), std:("project")}
                }}, 
                object::pair{std:("dependencies"), object{
                    object::pair{std:("express"), std:("^4.18.0")}
                }}
            }, nullptr, 2)); });
            auto result = runCliCommandSilently(elizaosCmd, std:("update --packages"), object{
                object::pair{std:("timeout"), TEST_TIMEOUTS["STANDARD_COMMAND"]}
            });
            expect(result)->toContain(std:("No ElizaOS packages found"));
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
    }
    );
}

MAIN
