#include "update.test.h"
#include <string>

void Main(void)
{
    describe(std::string("ElizaOS Update Commands"), [=]() mutable
    {
        shared<string> testTmpDir;
        shared<string> originalCwd;
        beforeEach([=]() mutable
        {
            originalCwd = process->cwd();
            testTmpDir = std::async([=]() { mkdtemp(join(tmpdir(), std::string("eliza-test-update-"))); });
            process->chdir(testTmpDir);
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
            bunExecSync(std::string("elizaos create ") + name + std::string(" --yes"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
            });
            process->chdir(join(testTmpDir, name));
        };
        it(std::string("update --help shows usage and options"), [=]() mutable
        {
            auto result = bunExecSync(std::string("elizaos update --help"), object{
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
            auto result = bunExecSync(std::string("elizaos update"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
            });
            expect(result)->toMatch((new RegExp(std::string("(Project successfully updated|Update completed|already up to date|No updates available"))));
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std::string("update --check works"), [=]() mutable
        {
            std::async([=]() { makeProj(std::string("update-check-app")); });
            auto result = bunExecSync(std::string("elizaos update --check"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
            });
            expect(result)->toMatch((new RegExp(std::string("Version: 1\.2\.\d"))));
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std::string("update --skip-build works"), [=]() mutable
        {
            std::async([=]() { makeProj(std::string("update-skip-build-app")); });
            auto result = bunExecSync(std::string("elizaos update --skip-build"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
            });
            expect(result)->not->toContain(std::string("Building project"));
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std::string("update --packages works"), [=]() mutable
        {
            std::async([=]() { makeProj(std::string("update-packages-app")); });
            auto result = bunExecSync(std::string("elizaos update --packages"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
            });
            expect(result)->toMatch((new RegExp(std::string("(Project successfully updated|Update completed|already up to date|No updates available"))));
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std::string("update --cli works outside a project"), [=]() mutable
        {
            auto result = bunExecSync(std::string("elizaos update --cli"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
            });
            expect(result)->toMatch((new RegExp(std::string("(Project successfully updated|Update completed|already up to date|No updates available|install the CLI globally|CLI update is not available|CLI is already at the latest version"))));
        }
        , TEST_TIMEOUTS["STANDARD_COMMAND"]);
        it(std::string("update --cli --packages works"), [=]() mutable
        {
            std::async([=]() { makeProj(std::string("update-combined-app")); });
            auto result = bunExecSync(std::string("elizaos update --cli --packages"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
            });
            expect(result)->toMatch((new RegExp(std::string("(Project successfully updated|Update completed|already up to date|No updates available"))));
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it->skipIf(OR((process->env->CI == std::string("true")), (process->env->GITHUB_ACTIONS == std::string("true"))))(std::string("update succeeds outside a project (global check)"), [=]() mutable
        {
            auto result = bunExecSync(std::string("elizaos update"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
            });
            expect(result)->toMatch((new RegExp(std::string("(Project successfully updated|Update completed|already up to date|No updates available|create a new ElizaOS project|This appears to be an empty directory|Version: monorepo|Version: 1\.2\.\d+"))));
        }
        , TEST_TIMEOUTS["STANDARD_COMMAND"]);
        it(std::string("update --packages shows helpful message in empty directory"), [=]() mutable
        {
            auto result = bunExecSync(std::string("elizaos update --packages"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
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
            auto result = bunExecSync(std::string("elizaos update --packages"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
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
            auto result = bunExecSync(std::string("elizaos update --packages --check"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
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
            auto result = bunExecSync(std::string("elizaos update --packages"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
            });
            expect(result)->toContain(std::string("No ElizaOS packages found"));
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it->skipIf(OR((process->env->CI == std::string("true")), (process->env->GITHUB_ACTIONS == std::string("true"))))(std::string("update command should not create files in non-project directory"), [=]() mutable
        {
            auto tmpDir = mkdtempSync(join(tmpdir(), std::string("eliza-test-")));
            auto currentDir = process->cwd();
            {
                utils::finally __finally8506_8674([&]() mutable
                {
                    process->chdir(currentDir);
                    rmSync(tmpDir, object{
                        object::pair{std::string("recursive"), true}, 
                        object::pair{std::string("force"), true}
                    });
                });
                try
                {
                    process->chdir(tmpDir);
                    auto result = bunExecSync(std::string("elizaos update"), object{
                        object::pair{std::string("encoding"), std::string("utf8")}
                    });
                    expect(result)->toBeTruthy();
                    expect(existsSync(join(tmpDir, std::string("package.json"))))->toBe(false);
                    expect(existsSync(join(tmpDir, std::string("bun.lock"))))->toBe(false);
                    expect(existsSync(join(tmpDir, std::string("node_modules"))))->toBe(false);
                    expect(existsSync(join(tmpDir, std::string("package-lock.json"))))->toBe(false);
                    expect(existsSync(join(tmpDir, std::string("yarn.lock"))))->toBe(false);
                    expect(result)->toMatch((new RegExp(std::string("CLI.*update|updat.*CLI|Version: monorepo|Version: 1\.2\.\d+"))));
                    expect(result)->not->toMatch((new RegExp(std::string("packages.*installed"))));
                }
                catch (...)
                {
                    throw;
                }
            }
        }
        , TEST_TIMEOUTS["STANDARD_COMMAND"]);
        describe(std::string("bunx/npx detection"), [=]() mutable
        {
            it->skip(std::string("update --cli shows warning when running via bunx"), [=]() mutable
            {
                auto result = bunExecSync(std::string("elizaos update --cli"), object{
                    object::pair{std::string("encoding"), std::string("utf8")}, 
                    object::pair{std::string("env"), utils::assign(object{
                        , 
                        object::pair{std::string("BUN_INSTALL_CACHE_DIR"), std::string("/Users/user/.bun/install/cache")}
                    }, process->env)}
                });
                expect(result)->toContain(std::string("CLI update is not available when running via npx or bunx"));
                expect(result)->toContain(std::string("bun install -g @elizaos/cli"));
            }
            );
            it->skip(std::string("update --cli shows warning when BUN_INSTALL_CACHE_DIR is set"), [=]() mutable
            {
                auto result = bunExecSync(std::string("elizaos update --cli"), object{
                    object::pair{std::string("encoding"), std::string("utf8")}, 
                    object::pair{std::string("env"), utils::assign(object{
                        , 
                        object::pair{std::string("BUN_INSTALL_CACHE_DIR"), std::string("/Users/user/.bun/install/cache")}
                    }, process->env)}
                });
                expect(result)->toContain(std::string("CLI update is not available when running via npx or bunx"));
            }
            );
            it->skip(std::string("update --cli shows warning when running via npx"), [=]() mutable
            {
                auto result = bunExecSync(std::string("elizaos update --cli"), object{
                    object::pair{std::string("encoding"), std::string("utf8")}, 
                    object::pair{std::string("env"), utils::assign(object{
                        , 
                        object::pair{std::string("npm_execpath"), std::string("/usr/local/lib/node_modules/npm/bin/npx-cli.js")}
                    }, process->env)}
                });
                expect(result)->toContain(std::string("CLI update is not available when running via npx or bunx"));
            }
            );
            it->skip(std::string("update --cli works with global bun installation"), [=]() mutable
            {
                process->argv = array<string>{ std::string("/Users/user/.bun/bin/bun"), std::string("/Users/user/.bun/install/global/@elizaos/cli/dist/index.js"), std::string("update"), std::string("--cli") };
                process->env = object{};
                auto result = bunExecSync(std::string("elizaos update --cli"), object{
                    object::pair{std::string("encoding"), std::string("utf8")}
                });
                expect(result)->not->toContain(std::string("CLI update is not available when running via npx or bunx"));
                expect(result)->toMatch((new RegExp(std::string("(Project successfully updated|Update completed|already up to date|No updates available|Checking for updates"))));
            }
            );
            it->skip(std::string("update --cli works with global npm installation"), [=]() mutable
            {
                process->argv = array<string>{ std::string("node"), std::string("/usr/local/lib/node_modules/@elizaos/cli/dist/index.js"), std::string("update"), std::string("--cli") };
                process->env = object{};
                auto result = bunExecSync(std::string("elizaos update --cli"), object{
                    object::pair{std::string("encoding"), std::string("utf8")}
                });
                expect(result)->not->toContain(std::string("CLI update is not available when running via npx or bunx"));
                expect(result)->toMatch((new RegExp(std::string("(Project successfully updated|Update completed|already up to date|No updates available|Checking for updates"))));
            }
            );
            it->skip(std::string("update --cli works when NODE_ENV=global"), [=]() mutable
            {
                process->argv = array<string>{ std::string("/Users/user/.bun/bin/bun"), std::string("/some/local/path/index.js"), std::string("update"), std::string("--cli") };
                process->env = object{
                    object::pair{std::string("NODE_ENV"), std::string("global")}
                };
                auto result = bunExecSync(std::string("elizaos update --cli"), object{
                    object::pair{std::string("encoding"), std::string("utf8")}
                });
                expect(result)->not->toContain(std::string("CLI update is not available when running via npx or bunx"));
            }
            );
            it(std::string("update --packages still works when running via bunx"), [=]() mutable
            {
                std::async([=]() { makeProj(std::string("update-bunx-packages")); });
                auto result = bunExecSync(std::string("elizaos update --packages"), object{
                    object::pair{std::string("encoding"), std::string("utf8")}, 
                    object::pair{std::string("env"), utils::assign(object{
                        , 
                        object::pair{std::string("BUN_INSTALL_CACHE_DIR"), std::string("/Users/user/.bun/install/cache")}
                    }, process->env)}
                });
                expect(result)->toMatch((new RegExp(std::string("(Project successfully updated|Update completed|already up to date|No updates available"))));
            }
            , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
            it->skip(std::string("update (both cli and packages) shows warning but continues with packages via bunx"), [=]() mutable
            {
                std::async([=]() { makeProj(std::string("update-bunx-both")); });
                auto result = bunExecSync(std::string("elizaos update"), object{
                    object::pair{std::string("encoding"), std::string("utf8")}, 
                    object::pair{std::string("env"), utils::assign(object{
                        , 
                        object::pair{std::string("BUN_INSTALL_CACHE_DIR"), std::string("/Users/user/.bun/install/cache")}
                    }, process->env)}
                });
                expect(result)->toContain(std::string("CLI update is not available when running via npx or bunx"));
                expect(result)->toMatch((new RegExp(std::string("(Project successfully updated|Update completed|already up to date|No updates available|Found.*ElizaOS package"))));
            }
            , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        }
        );
    }
    );
}

MAIN
