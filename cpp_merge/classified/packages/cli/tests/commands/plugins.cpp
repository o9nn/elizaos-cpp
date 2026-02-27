#include "plugins.test.h"

any PLUGIN_INSTALLATION_BUFFER = (process->platform == std:("win32")) ? 30000 : 0;

void Main(void)
{
    describe(std:("ElizaOS Plugin Commands"), [=]() mutable
    {
        shared<string> testTmpDir;
        shared<string> projectDir;
        shared<string> originalCwd;
        beforeAll([=]() mutable
        {
            originalCwd = process->cwd();
            testTmpDir = std::async([=]() { mkdtemp(join(tmpdir(), std:("eliza-test-plugins-"))); });
            projectDir = join(testTmpDir, std:("shared-test-project"));
            process->chdir(testTmpDir);
            console->log(std:("Creating shared test project..."));
            bunExecSync(std:("elizaos create shared-test-project --yes"), getPlatformOptions(object{
                object::pair{std:("stdio"), std:("pipe")}, 
                object::pair{std:("timeout"), TEST_TIMEOUTS["PROJECT_CREATION"]}
            }));
            process->chdir(projectDir);
            console->log(std:("Shared test project created at:"), projectDir);
            console->log(std:("Installing project dependencies..."));
            std::async([=]() { bunExecSimple(std:("bun"), array<string>{ std:("install") }, object{
                object::pair{std:("timeout"), TEST_TIMEOUTS["NETWORK_OPERATION"]}, 
                object::pair{std:("env"), process->env}
            }); });
            console->log(std:("Dependencies installed successfully"));
        }
        );
        beforeEach([=]() mutable
        {
            process->chdir(projectDir);
        }
        );
        afterAll([=]() mutable
        {
            safeChangeDirectory(originalCwd);
            if (AND((testTmpDir), (testTmpDir->includes(std:("eliza-test-plugins-"))))) {
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
        it(std:("plugins command shows help with no subcommand"), [=]() mutable
        {
            auto result = bunExecSync(std:("elizaos plugins"), getPlatformOptions(object{
                object::pair{std:("encoding"), std:("utf8")}
            }));
            expect(result)->toContain(std:("Manage ElizaOS plugins"));
            expect(result)->toContain(std:("Commands:"));
            expect(result)->toContain(std:("list"));
            expect(result)->toContain(std:("add"));
            expect(result)->toContain(std:("installed-plugins"));
            expect(result)->toContain(std:("remove"));
        }
        );
        it(std:("plugins --help shows usage information"), [=]() mutable
        {
            auto result = bunExecSync(std:("elizaos plugins --help"), getPlatformOptions(object{
                object::pair{std:("encoding"), std:("utf8")}
            }));
            expect(result)->toContain(std:("Manage ElizaOS plugins"));
        }
        );
        it(std:("plugins list shows available plugins"), [=]() mutable
        {
            auto result = bunExecSync(std:("elizaos plugins list"), getPlatformOptions(object{
                object::pair{std:("encoding"), std:("utf8")}
            }));
            expect(result)->toContain(std:("Available v1.x plugins"));
            expect(result)->toMatch((new RegExp(std:("plugin-opena"))));
            expect(result)->toMatch((new RegExp(std:("plugin-ollam"))));
        }
        );
        it(std:("plugins list aliases (l, ls) work correctly"), [=]() mutable
        {
            auto aliases = array<string>{ std:("l"), std:("ls") };
            for (auto& alias : aliases)
            {
                auto result = bunExecSync(std:("elizaos plugins ") + alias + string_empty, getPlatformOptions(object{
                    object::pair{std:("encoding"), std:("utf8")}
                }));
                expect(result)->toContain(std:("Available v1.x plugins"));
                expect(result)->toContain(std:("plugins"));
            }
        }
        );
        it(std:("plugins add installs a plugin"), [=]() mutable
        {
            try
            {
                bunExecSync(std:("elizaos plugins add @elizaos/plugin-openai --skip-env-prompt"), object{
                    object::pair{std:("stdio"), std:("pipe")}, 
                    object::pair{std:("timeout"), TEST_TIMEOUTS["PLUGIN_INSTALLATION"]}, 
                    object::pair{std:("cwd"), projectDir}
                });
                auto packageJson = std::async([=]() { readFile(join(projectDir, std:("package.json")), std:("utf8")); });
                expect(packageJson)->toContain(std:("@elizaos/plugin-openai"));
            }
            catch (const any& error)
            {
                console->error(std:("[ERROR] Plugin installation failed:"), error["message"]);
                console->error(std:("[ERROR] stdout:"), OR((error["stdout"]["toString"]()), (std:("none"))));
                console->error(std:("[ERROR] stderr:"), OR((error["stderr"]["toString"]()), (std:("none"))));
                throw any(error);
            }
        }
        , TEST_TIMEOUTS["PLUGIN_INSTALLATION"] + PLUGIN_INSTALLATION_BUFFER);
        it(std:("plugins install alias works"), [=]() mutable
        {
            try
            {
                bunExecSync(std:("elizaos plugins install @elizaos/plugin-mcp --skip-env-prompt"), object{
                    object::pair{std:("stdio"), std:("pipe")}, 
                    object::pair{std:("timeout"), TEST_TIMEOUTS["PLUGIN_INSTALLATION"]}, 
                    object::pair{std:("cwd"), projectDir}
                });
                auto packageJson = std::async([=]() { readFile(join(projectDir, std:("package.json")), std:("utf8")); });
                expect(packageJson)->toContain(std:("@elizaos/plugin-mcp"));
            }
            catch (const any& error)
            {
                console->error(std:("[ERROR] Plugin installation failed:"), error["message"]);
                console->error(std:("[ERROR] stdout:"), OR((error["stdout"]["toString"]()), (std:("none"))));
                console->error(std:("[ERROR] stderr:"), OR((error["stderr"]["toString"]()), (std:("none"))));
                throw any(error);
            }
        }
        , TEST_TIMEOUTS["PLUGIN_INSTALLATION"] + PLUGIN_INSTALLATION_BUFFER);
        it(std:("plugins add supports third-party plugins"), [=]() mutable
        {
            try
            {
                bunExecSync(std:("elizaos plugins add @fleek-platform/eliza-plugin-mcp --skip-env-prompt"), object{
                    object::pair{std:("stdio"), std:("pipe")}, 
                    object::pair{std:("timeout"), TEST_TIMEOUTS["PLUGIN_INSTALLATION"]}, 
                    object::pair{std:("cwd"), projectDir}
                });
                auto packageJson = std::async([=]() { readFile(join(projectDir, std:("package.json")), std:("utf8")); });
                expect(packageJson)->toContain(std:("@fleek-platform/eliza-plugin-mcp"));
            }
            catch (const any& error)
            {
                console->error(std:("[ERROR] Plugin installation failed:"), error["message"]);
                console->error(std:("[ERROR] stdout:"), OR((error["stdout"]["toString"]()), (std:("none"))));
                console->error(std:("[ERROR] stderr:"), OR((error["stderr"]["toString"]()), (std:("none"))));
                throw any(error);
            }
        }
        , TEST_TIMEOUTS["PLUGIN_INSTALLATION"] + PLUGIN_INSTALLATION_BUFFER);
        it(std:("plugins add supports GitHub URL installation"), [=]() mutable
        {
            try
            {
                bunExecSync(std:("elizaos plugins add https://github.com/elizaos-plugins/plugin-video-understanding --skip-env-prompt"), object{
                    object::pair{std:("stdio"), std:("pipe")}, 
                    object::pair{std:("timeout"), TEST_TIMEOUTS["PLUGIN_INSTALLATION"]}, 
                    object::pair{std:("cwd"), projectDir}
                });
                auto packageJson1 = std::async([=]() { readFile(join(projectDir, std:("package.json")), std:("utf8")); });
                expect(packageJson1)->toContain(std:("plugin-video-understanding"));
                bunExecSync(std:("elizaos plugins add github:elizaos-plugins/plugin-openrouter#1.x --skip-env-prompt"), object{
                    object::pair{std:("stdio"), std:("pipe")}, 
                    object::pair{std:("timeout"), TEST_TIMEOUTS["PLUGIN_INSTALLATION"]}, 
                    object::pair{std:("cwd"), projectDir}
                });
                auto packageJson2 = std::async([=]() { readFile(join(projectDir, std:("package.json")), std:("utf8")); });
                expect(packageJson2)->toContain(std:("plugin-openrouter"));
            }
            catch (const any& error)
            {
                console->error(std:("[ERROR] GitHub plugin installation failed:"), error["message"]);
                console->error(std:("[ERROR] stdout:"), OR((error["stdout"]["toString"]()), (std:("none"))));
                console->error(std:("[ERROR] stderr:"), OR((error["stderr"]["toString"]()), (std:("none"))));
                throw any(error);
            }
        }
        , TEST_TIMEOUTS["PLUGIN_INSTALLATION"] + PLUGIN_INSTALLATION_BUFFER);
        it(std:("plugins installed-plugins shows installed plugins"), [=]() mutable
        {
            auto result = bunExecSync(std:("elizaos plugins installed-plugins"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(result)->toMatch((new RegExp(std:("@elizaos\/plugin-|github"))));
        }
        , TEST_TIMEOUTS["PLUGIN_INSTALLATION"] + PLUGIN_INSTALLATION_BUFFER);
        it(std:("plugins remove uninstalls a plugin"), [=]() mutable
        {
            try
            {
                bunExecSync(std:("elizaos plugins add @elizaos/plugin-elevenlabs --skip-env-prompt"), object{
                    object::pair{std:("stdio"), std:("pipe")}, 
                    object::pair{std:("timeout"), TEST_TIMEOUTS["PLUGIN_INSTALLATION"]}, 
                    object::pair{std:("cwd"), projectDir}
                });
                auto packageJson = std::async([=]() { readFile(join(projectDir, std:("package.json")), std:("utf8")); });
                expect(packageJson)->toContain(std:("@elizaos/plugin-elevenlabs"));
                bunExecSync(std:("elizaos plugins remove @elizaos/plugin-elevenlabs"), object{
                    object::pair{std:("stdio"), std:("pipe")}, 
                    object::pair{std:("timeout"), TEST_TIMEOUTS["STANDARD_COMMAND"]}, 
                    object::pair{std:("cwd"), projectDir}
                });
                packageJson = std::async([=]() { readFile(join(projectDir, std:("package.json")), std:("utf8")); });
                expect(packageJson)->not->toContain(std:("@elizaos/plugin-elevenlabs"));
            }
            catch (const any& error)
            {
                console->error(std:("[ERROR] Plugin remove failed:"), error["message"]);
                console->error(std:("[ERROR] stdout:"), OR((error["stdout"]["toString"]()), (std:("none"))));
                console->error(std:("[ERROR] stderr:"), OR((error["stderr"]["toString"]()), (std:("none"))));
                throw any(error);
            }
        }
        , TEST_TIMEOUTS["PLUGIN_INSTALLATION"] + PLUGIN_INSTALLATION_BUFFER);
        it(std:("plugins remove aliases (delete, del, rm) work"), [=]() mutable
        {
            try
            {
                auto plugins = array<string>{ std:("@elizaos/plugin-bedrock"), std:("@elizaos/plugin-knowledge"), std:("@elizaos/plugin-farcaster") };
                for (auto& plugin : plugins)
                {
                    bunExecSync(std:("elizaos plugins add ") + plugin + std:(" --skip-env-prompt"), object{
                        object::pair{std:("stdio"), std:("pipe")}, 
                        object::pair{std:("timeout"), TEST_TIMEOUTS["PLUGIN_INSTALLATION"]}, 
                        object::pair{std:("cwd"), projectDir}
                    });
                }
                auto removeCommands = array<array<string>>{ array<string>{ std:("delete"), std:("@elizaos/plugin-bedrock") }, array<string>{ std:("del"), std:("@elizaos/plugin-knowledge") }, array<string>{ std:("rm"), std:("@elizaos/plugin-farcaster") } };
                for (auto& [command, plugin] : removeCommands)
                {
                    bunExecSync(std:("elizaos plugins ") + command + std:(" ") + plugin + string_empty, object{
                        object::pair{std:("stdio"), std:("pipe")}, 
                        object::pair{std:("timeout"), TEST_TIMEOUTS["STANDARD_COMMAND"]}, 
                        object::pair{std:("cwd"), projectDir}
                    });
                }
            }
            catch (const any& error)
            {
                console->error(std:("[ERROR] Plugin remove aliases failed:"), error["message"]);
                console->error(std:("[ERROR] stdout:"), OR((error["stdout"]["toString"]()), (std:("none"))));
                console->error(std:("[ERROR] stderr:"), OR((error["stderr"]["toString"]()), (std:("none"))));
                throw any(error);
            }
        }
        , TEST_TIMEOUTS["PLUGIN_INSTALLATION"] + PLUGIN_INSTALLATION_BUFFER);
        it(std:("plugins add fails for missing plugin"), [=]() mutable
        {
            try
            {
                bunExecSync(std:("elizaos plugins add missing --skip-env-prompt"), object{
                    object::pair{std:("stdio"), std:("pipe")}, 
                    object::pair{std:("timeout"), TEST_TIMEOUTS["STANDARD_COMMAND"]}, 
                    object::pair{std:("cwd"), projectDir}
                });
                expect(false)->toBe(true);
            }
            catch (const any& e)
            {
                expect(e["status"])->not->toBe(0);
                auto output = OR((OR((e["stdout"]["toString"]()), (e["stderr"]["toString"]()))), (string_empty));
                expect(output)->toMatch((new RegExp(std:("not found in registr"))));
            }
        }
        , TEST_TIMEOUTS["PLUGIN_INSTALLATION"] + PLUGIN_INSTALLATION_BUFFER);
        it(std:("plugins add via GitHub shorthand URL"), [=]() mutable
        {
            try
            {
                bunExecSync(std:("elizaos plugins add github:elizaos-plugins/plugin-farcaster#1.x --skip-env-prompt"), object{
                    object::pair{std:("stdio"), std:("pipe")}, 
                    object::pair{std:("timeout"), TEST_TIMEOUTS["PLUGIN_INSTALLATION"]}, 
                    object::pair{std:("cwd"), projectDir}
                });
                auto packageJson = std::async([=]() { readFile(join(projectDir, std:("package.json")), std:("utf8")); });
                expect(packageJson)->toContain(std:("github:elizaos-plugins/plugin-farcaster#1.x"));
            }
            catch (const any& error)
            {
                console->error(std:("[ERROR] GitHub shorthand plugin installation failed:"), error["message"]);
                console->error(std:("[ERROR] stdout:"), OR((error["stdout"]["toString"]()), (std:("none"))));
                console->error(std:("[ERROR] stderr:"), OR((error["stderr"]["toString"]()), (std:("none"))));
                throw any(error);
            }
        }
        , TEST_TIMEOUTS["PLUGIN_INSTALLATION"] + PLUGIN_INSTALLATION_BUFFER);
    }
    );
}

MAIN
