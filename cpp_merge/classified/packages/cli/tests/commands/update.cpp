#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/tests/commands/update.test.h"

void Main(void)
{
    describe(std:("ElizaOS Update Commands"), [=]() mutable
    {
        shared<string> testTmpDir;
        shared<string> originalCwd;
        beforeEach([=]() mutable
        {
            originalCwd = process->cwd();
            testTmpDir = std::async([=]() { mkdtemp(join(tmpdir(), std:("eliza-test-update-"))); });
            process->chdir(testTmpDir);
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
            bunExecSync(std:("elizaos create ") + name + std:(" --yes"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            process->chdir(join(testTmpDir, name));
        };
        it(std:("update --help shows usage and options"), [=]() mutable
        {
            auto result = bunExecSync(std:("elizaos update --help"), object{
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
            auto result = bunExecSync(std:("elizaos update"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(result)->toMatch((new RegExp(std:("(Project successfully updated|Update completed|already up to date|No updates available"))));
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std:("update --check works"), [=]() mutable
        {
            std::async([=]() { makeProj(std:("update-check-app")); });
            auto result = bunExecSync(std:("elizaos update --check"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(result)->toMatch((new RegExp(std:("Version: 1\.2\.\d"))));
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std:("update --skip-build works"), [=]() mutable
        {
            std::async([=]() { makeProj(std:("update-skip-build-app")); });
            auto result = bunExecSync(std:("elizaos update --skip-build"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(result)->not->toContain(std:("Building project"));
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std:("update --packages works"), [=]() mutable
        {
            std::async([=]() { makeProj(std:("update-packages-app")); });
            auto result = bunExecSync(std:("elizaos update --packages"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(result)->toMatch((new RegExp(std:("(Project successfully updated|Update completed|already up to date|No updates available"))));
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std:("update --cli works outside a project"), [=]() mutable
        {
            auto result = bunExecSync(std:("elizaos update --cli"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(result)->toMatch((new RegExp(std:("(Project successfully updated|Update completed|already up to date|No updates available|install the CLI globally|CLI update is not available|CLI is already at the latest version"))));
        }
        , TEST_TIMEOUTS["STANDARD_COMMAND"]);
        it(std:("update --cli --packages works"), [=]() mutable
        {
            std::async([=]() { makeProj(std:("update-combined-app")); });
            auto result = bunExecSync(std:("elizaos update --cli --packages"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(result)->toMatch((new RegExp(std:("(Project successfully updated|Update completed|already up to date|No updates available"))));
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it->skipIf(OR((process->env->CI == std:("true")), (process->env->GITHUB_ACTIONS == std:("true"))))(std:("update succeeds outside a project (global check)"), [=]() mutable
        {
            auto result = bunExecSync(std:("elizaos update"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(result)->toMatch((new RegExp(std:("(Project successfully updated|Update completed|already up to date|No updates available|create a new ElizaOS project|This appears to be an empty directory|Version: monorepo|Version: 1\.2\.\d+"))));
        }
        , TEST_TIMEOUTS["STANDARD_COMMAND"]);
        it(std:("update --packages shows helpful message in empty directory"), [=]() mutable
        {
            auto result = bunExecSync(std:("elizaos update --packages"), object{
                object::pair{std:("encoding"), std:("utf8")}
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
            auto result = bunExecSync(std:("elizaos update --packages"), object{
                object::pair{std:("encoding"), std:("utf8")}
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
            auto result = bunExecSync(std:("elizaos update --packages --check"), object{
                object::pair{std:("encoding"), std:("utf8")}
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
            auto result = bunExecSync(std:("elizaos update --packages"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(result)->toContain(std:("No ElizaOS packages found"));
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it->skipIf(OR((process->env->CI == std:("true")), (process->env->GITHUB_ACTIONS == std:("true"))))(std:("update command should not create files in non-project directory"), [=]() mutable
        {
            auto tmpDir = mkdtempSync(join(tmpdir(), std:("eliza-test-")));
            auto currentDir = process->cwd();
            {
                utils::finally __finally8506_8674([&]() mutable
                {
                    process->chdir(currentDir);
                    rmSync(tmpDir, object{
                        object::pair{std:("recursive"), true}, 
                        object::pair{std:("force"), true}
                    });
                });
                try
                {
                    process->chdir(tmpDir);
                    auto result = bunExecSync(std:("elizaos update"), object{
                        object::pair{std:("encoding"), std:("utf8")}
                    });
                    expect(result)->toBeTruthy();
                    expect(existsSync(join(tmpDir, std:("package.json"))))->toBe(false);
                    expect(existsSync(join(tmpDir, std:("bun.lock"))))->toBe(false);
                    expect(existsSync(join(tmpDir, std:("node_modules"))))->toBe(false);
                    expect(existsSync(join(tmpDir, std:("package-lock.json"))))->toBe(false);
                    expect(existsSync(join(tmpDir, std:("yarn.lock"))))->toBe(false);
                    expect(result)->toMatch((new RegExp(std:("CLI.*update|updat.*CLI|Version: monorepo|Version: 1\.2\.\d+"))));
                    expect(result)->not->toMatch((new RegExp(std:("packages.*installed"))));
                }
                catch (...)
                {
                    throw;
                }
            }
        }
        , TEST_TIMEOUTS["STANDARD_COMMAND"]);
        describe(std:("bunx/npx detection"), [=]() mutable
        {
            it->skip(std:("update --cli shows warning when running via bunx"), [=]() mutable
            {
                auto result = bunExecSync(std:("elizaos update --cli"), object{
                    object::pair{std:("encoding"), std:("utf8")}, 
                    object::pair{std:("env"), utils::assign(object{
                        , 
                        object::pair{std:("BUN_INSTALL_CACHE_DIR"), std:("/Users/user/.bun/install/cache")}
                    }, process->env)}
                });
                expect(result)->toContain(std:("CLI update is not available when running via npx or bunx"));
                expect(result)->toContain(std:("bun install -g @elizaos/cli"));
            }
            );
            it->skip(std:("update --cli shows warning when BUN_INSTALL_CACHE_DIR is set"), [=]() mutable
            {
                auto result = bunExecSync(std:("elizaos update --cli"), object{
                    object::pair{std:("encoding"), std:("utf8")}, 
                    object::pair{std:("env"), utils::assign(object{
                        , 
                        object::pair{std:("BUN_INSTALL_CACHE_DIR"), std:("/Users/user/.bun/install/cache")}
                    }, process->env)}
                });
                expect(result)->toContain(std:("CLI update is not available when running via npx or bunx"));
            }
            );
            it->skip(std:("update --cli shows warning when running via npx"), [=]() mutable
            {
                auto result = bunExecSync(std:("elizaos update --cli"), object{
                    object::pair{std:("encoding"), std:("utf8")}, 
                    object::pair{std:("env"), utils::assign(object{
                        , 
                        object::pair{std:("npm_execpath"), std:("/usr/local/lib/node_modules/npm/bin/npx-cli.js")}
                    }, process->env)}
                });
                expect(result)->toContain(std:("CLI update is not available when running via npx or bunx"));
            }
            );
            it->skip(std:("update --cli works with global bun installation"), [=]() mutable
            {
                process->argv = array<string>{ std:("/Users/user/.bun/bin/bun"), std:("/Users/user/.bun/install/global/@elizaos/cli/dist/index.js"), std:("update"), std:("--cli") };
                process->env = object{};
                auto result = bunExecSync(std:("elizaos update --cli"), object{
                    object::pair{std:("encoding"), std:("utf8")}
                });
                expect(result)->not->toContain(std:("CLI update is not available when running via npx or bunx"));
                expect(result)->toMatch((new RegExp(std:("(Project successfully updated|Update completed|already up to date|No updates available|Checking for updates"))));
            }
            );
            it->skip(std:("update --cli works with global npm installation"), [=]() mutable
            {
                process->argv = array<string>{ std:("node"), std:("/usr/local/lib/node_modules/@elizaos/cli/dist/index.js"), std:("update"), std:("--cli") };
                process->env = object{};
                auto result = bunExecSync(std:("elizaos update --cli"), object{
                    object::pair{std:("encoding"), std:("utf8")}
                });
                expect(result)->not->toContain(std:("CLI update is not available when running via npx or bunx"));
                expect(result)->toMatch((new RegExp(std:("(Project successfully updated|Update completed|already up to date|No updates available|Checking for updates"))));
            }
            );
            it->skip(std:("update --cli works when NODE_ENV=global"), [=]() mutable
            {
                process->argv = array<string>{ std:("/Users/user/.bun/bin/bun"), std:("/some/local/path/index.js"), std:("update"), std:("--cli") };
                process->env = object{
                    object::pair{std:("NODE_ENV"), std:("global")}
                };
                auto result = bunExecSync(std:("elizaos update --cli"), object{
                    object::pair{std:("encoding"), std:("utf8")}
                });
                expect(result)->not->toContain(std:("CLI update is not available when running via npx or bunx"));
            }
            );
            it(std:("update --packages still works when running via bunx"), [=]() mutable
            {
                std::async([=]() { makeProj(std:("update-bunx-packages")); });
                auto result = bunExecSync(std:("elizaos update --packages"), object{
                    object::pair{std:("encoding"), std:("utf8")}, 
                    object::pair{std:("env"), utils::assign(object{
                        , 
                        object::pair{std:("BUN_INSTALL_CACHE_DIR"), std:("/Users/user/.bun/install/cache")}
                    }, process->env)}
                });
                expect(result)->toMatch((new RegExp(std:("(Project successfully updated|Update completed|already up to date|No updates available"))));
            }
            , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
            it->skip(std:("update (both cli and packages) shows warning but continues with packages via bunx"), [=]() mutable
            {
                std::async([=]() { makeProj(std:("update-bunx-both")); });
                auto result = bunExecSync(std:("elizaos update"), object{
                    object::pair{std:("encoding"), std:("utf8")}, 
                    object::pair{std:("env"), utils::assign(object{
                        , 
                        object::pair{std:("BUN_INSTALL_CACHE_DIR"), std:("/Users/user/.bun/install/cache")}
                    }, process->env)}
                });
                expect(result)->toContain(std:("CLI update is not available when running via npx or bunx"));
                expect(result)->toMatch((new RegExp(std:("(Project successfully updated|Update completed|already up to date|No updates available|Found.*ElizaOS package"))));
            }
            , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        }
        );
    }
    );
}

MAIN
