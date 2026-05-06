#include "plugins.test.h"
#include <string>

any PLUGIN_INSTALLATION_BUFFER = (process->platform == std::string("win32")) ? 30000 : 0;

void Main(void)
{
    describe(std::string("ElizaOS Plugin Commands"), [=]() mutable
    {
        shared<string> testTmpDir;
        shared<string> projectDir;
        shared<string> originalCwd;
        beforeAll([=]() mutable
        {
            originalCwd = process->cwd();
            testTmpDir = std::async([=]() { mkdtemp(join(tmpdir(), std::string("eliza-test-plugins-"))); });
            projectDir = join(testTmpDir, std::string("shared-test-project"));
            process->chdir(testTmpDir);
            console->log(std::string("Creating shared test project..."));
            bunExecSync(std::string("elizaos create shared-test-project --yes"), getPlatformOptions(object{
                object::pair{std::string("stdio"), std::string("pipe")}, 
                object::pair{std::string("timeout"), TEST_TIMEOUTS["PROJECT_CREATION"]}
            }));
            process->chdir(projectDir);
            console->log(std::string("Shared test project created at:"), projectDir);
            console->log(std::string("Installing project dependencies..."));
            std::async([=]() { bunExecSimple(std::string("bun"), array<string>{ std::string("install") }, object{
                object::pair{std::string("timeout"), TEST_TIMEOUTS["NETWORK_OPERATION"]}, 
                object::pair{std::string("env"), process->env}
            }); });
            console->log(std::string("Dependencies installed successfully"));
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
            if (AND((testTmpDir), (testTmpDir->includes(std::string("eliza-test-plugins-"))))) {
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
        it(std::string("plugins command shows help with no subcommand"), [=]() mutable
        {
            auto result = bunExecSync(std::string("elizaos plugins"), getPlatformOptions(object{
                object::pair{std::string("encoding"), std::string("utf8")}
            }));
            expect(result)->toContain(std::string("Manage ElizaOS plugins"));
            expect(result)->toContain(std::string("Commands:"));
            expect(result)->toContain(std::string("list"));
            expect(result)->toContain(std::string("add"));
            expect(result)->toContain(std::string("installed-plugins"));
            expect(result)->toContain(std::string("remove"));
        }
        );
        it(std::string("plugins --help shows usage information"), [=]() mutable
        {
            auto result = bunExecSync(std::string("elizaos plugins --help"), getPlatformOptions(object{
                object::pair{std::string("encoding"), std::string("utf8")}
            }));
            expect(result)->toContain(std::string("Manage ElizaOS plugins"));
        }
        );
        it(std::string("plugins list shows available plugins"), [=]() mutable
        {
            auto result = bunExecSync(std::string("elizaos plugins list"), getPlatformOptions(object{
                object::pair{std::string("encoding"), std::string("utf8")}
            }));
            expect(result)->toContain(std::string("Available v1.x plugins"));
            expect(result)->toMatch((new RegExp(std::string("plugin-opena"))));
            expect(result)->toMatch((new RegExp(std::string("plugin-ollam"))));
        }
        );
        it(std::string("plugins list aliases (l, ls) work correctly"), [=]() mutable
        {
            auto aliases = array<string>{ std::string("l"), std::string("ls") };
            for (auto& alias : aliases)
            {
                auto result = bunExecSync(std::string("elizaos plugins ") + alias + string_empty, getPlatformOptions(object{
                    object::pair{std::string("encoding"), std::string("utf8")}
                }));
                expect(result)->toContain(std::string("Available v1.x plugins"));
                expect(result)->toContain(std::string("plugins"));
            }
        }
        );
        it(std::string("plugins add installs a plugin"), [=]() mutable
        {
            try
            {
                bunExecSync(std::string("elizaos plugins add @elizaos/plugin-openai --skip-env-prompt"), object{
                    object::pair{std::string("stdio"), std::string("pipe")}, 
                    object::pair{std::string("timeout"), TEST_TIMEOUTS["PLUGIN_INSTALLATION"]}, 
                    object::pair{std::string("cwd"), projectDir}
                });
                auto packageJson = std::async([=]() { readFile(join(projectDir, std::string("package.json")), std::string("utf8")); });
                expect(packageJson)->toContain(std::string("@elizaos/plugin-openai"));
            }
            catch (const any& error)
            {
                console->error(std::string("[ERROR] Plugin installation failed:"), error["message"]);
                console->error(std::string("[ERROR] stdout:"), OR((error["stdout"]["toString"]()), (std::string("none"))));
                console->error(std::string("[ERROR] stderr:"), OR((error["stderr"]["toString"]()), (std::string("none"))));
                throw any(error);
            }
        }
        , TEST_TIMEOUTS["PLUGIN_INSTALLATION"] + PLUGIN_INSTALLATION_BUFFER);
        it(std::string("plugins install alias works"), [=]() mutable
        {
            try
            {
                bunExecSync(std::string("elizaos plugins install @elizaos/plugin-mcp --skip-env-prompt"), object{
                    object::pair{std::string("stdio"), std::string("pipe")}, 
                    object::pair{std::string("timeout"), TEST_TIMEOUTS["PLUGIN_INSTALLATION"]}, 
                    object::pair{std::string("cwd"), projectDir}
                });
                auto packageJson = std::async([=]() { readFile(join(projectDir, std::string("package.json")), std::string("utf8")); });
                expect(packageJson)->toContain(std::string("@elizaos/plugin-mcp"));
            }
            catch (const any& error)
            {
                console->error(std::string("[ERROR] Plugin installation failed:"), error["message"]);
                console->error(std::string("[ERROR] stdout:"), OR((error["stdout"]["toString"]()), (std::string("none"))));
                console->error(std::string("[ERROR] stderr:"), OR((error["stderr"]["toString"]()), (std::string("none"))));
                throw any(error);
            }
        }
        , TEST_TIMEOUTS["PLUGIN_INSTALLATION"] + PLUGIN_INSTALLATION_BUFFER);
        it(std::string("plugins add supports third-party plugins"), [=]() mutable
        {
            try
            {
                bunExecSync(std::string("elizaos plugins add @fleek-platform/eliza-plugin-mcp --skip-env-prompt"), object{
                    object::pair{std::string("stdio"), std::string("pipe")}, 
                    object::pair{std::string("timeout"), TEST_TIMEOUTS["PLUGIN_INSTALLATION"]}, 
                    object::pair{std::string("cwd"), projectDir}
                });
                auto packageJson = std::async([=]() { readFile(join(projectDir, std::string("package.json")), std::string("utf8")); });
                expect(packageJson)->toContain(std::string("@fleek-platform/eliza-plugin-mcp"));
            }
            catch (const any& error)
            {
                console->error(std::string("[ERROR] Plugin installation failed:"), error["message"]);
                console->error(std::string("[ERROR] stdout:"), OR((error["stdout"]["toString"]()), (std::string("none"))));
                console->error(std::string("[ERROR] stderr:"), OR((error["stderr"]["toString"]()), (std::string("none"))));
                throw any(error);
            }
        }
        , TEST_TIMEOUTS["PLUGIN_INSTALLATION"] + PLUGIN_INSTALLATION_BUFFER);
        it(std::string("plugins add supports GitHub URL installation"), [=]() mutable
        {
            try
            {
                bunExecSync(std::string("elizaos plugins add https://github.com/elizaos-plugins/plugin-video-understanding --skip-env-prompt"), object{
                    object::pair{std::string("stdio"), std::string("pipe")}, 
                    object::pair{std::string("timeout"), TEST_TIMEOUTS["PLUGIN_INSTALLATION"]}, 
                    object::pair{std::string("cwd"), projectDir}
                });
                auto packageJson1 = std::async([=]() { readFile(join(projectDir, std::string("package.json")), std::string("utf8")); });
                expect(packageJson1)->toContain(std::string("plugin-video-understanding"));
                bunExecSync(std::string("elizaos plugins add github:elizaos-plugins/plugin-openrouter#1.x --skip-env-prompt"), object{
                    object::pair{std::string("stdio"), std::string("pipe")}, 
                    object::pair{std::string("timeout"), TEST_TIMEOUTS["PLUGIN_INSTALLATION"]}, 
                    object::pair{std::string("cwd"), projectDir}
                });
                auto packageJson2 = std::async([=]() { readFile(join(projectDir, std::string("package.json")), std::string("utf8")); });
                expect(packageJson2)->toContain(std::string("plugin-openrouter"));
            }
            catch (const any& error)
            {
                console->error(std::string("[ERROR] GitHub plugin installation failed:"), error["message"]);
                console->error(std::string("[ERROR] stdout:"), OR((error["stdout"]["toString"]()), (std::string("none"))));
                console->error(std::string("[ERROR] stderr:"), OR((error["stderr"]["toString"]()), (std::string("none"))));
                throw any(error);
            }
        }
        , TEST_TIMEOUTS["PLUGIN_INSTALLATION"] + PLUGIN_INSTALLATION_BUFFER);
        it(std::string("plugins installed-plugins shows installed plugins"), [=]() mutable
        {
            auto result = bunExecSync(std::string("elizaos plugins installed-plugins"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
            });
            expect(result)->toMatch((new RegExp(std::string("@elizaos\/plugin-|github"))));
        }
        , TEST_TIMEOUTS["PLUGIN_INSTALLATION"] + PLUGIN_INSTALLATION_BUFFER);
        it(std::string("plugins remove uninstalls a plugin"), [=]() mutable
        {
            try
            {
                bunExecSync(std::string("elizaos plugins add @elizaos/plugin-elevenlabs --skip-env-prompt"), object{
                    object::pair{std::string("stdio"), std::string("pipe")}, 
                    object::pair{std::string("timeout"), TEST_TIMEOUTS["PLUGIN_INSTALLATION"]}, 
                    object::pair{std::string("cwd"), projectDir}
                });
                auto packageJson = std::async([=]() { readFile(join(projectDir, std::string("package.json")), std::string("utf8")); });
                expect(packageJson)->toContain(std::string("@elizaos/plugin-elevenlabs"));
                bunExecSync(std::string("elizaos plugins remove @elizaos/plugin-elevenlabs"), object{
                    object::pair{std::string("stdio"), std::string("pipe")}, 
                    object::pair{std::string("timeout"), TEST_TIMEOUTS["STANDARD_COMMAND"]}, 
                    object::pair{std::string("cwd"), projectDir}
                });
                packageJson = std::async([=]() { readFile(join(projectDir, std::string("package.json")), std::string("utf8")); });
                expect(packageJson)->not->toContain(std::string("@elizaos/plugin-elevenlabs"));
            }
            catch (const any& error)
            {
                console->error(std::string("[ERROR] Plugin remove failed:"), error["message"]);
                console->error(std::string("[ERROR] stdout:"), OR((error["stdout"]["toString"]()), (std::string("none"))));
                console->error(std::string("[ERROR] stderr:"), OR((error["stderr"]["toString"]()), (std::string("none"))));
                throw any(error);
            }
        }
        , TEST_TIMEOUTS["PLUGIN_INSTALLATION"] + PLUGIN_INSTALLATION_BUFFER);
        it(std::string("plugins remove aliases (delete, del, rm) work"), [=]() mutable
        {
            try
            {
                auto plugins = array<string>{ std::string("@elizaos/plugin-bedrock"), std::string("@elizaos/plugin-knowledge"), std::string("@elizaos/plugin-farcaster") };
                for (auto& plugin : plugins)
                {
                    bunExecSync(std::string("elizaos plugins add ") + plugin + std::string(" --skip-env-prompt"), object{
                        object::pair{std::string("stdio"), std::string("pipe")}, 
                        object::pair{std::string("timeout"), TEST_TIMEOUTS["PLUGIN_INSTALLATION"]}, 
                        object::pair{std::string("cwd"), projectDir}
                    });
                }
                auto removeCommands = array<array<string>>{ array<string>{ std::string("delete"), std::string("@elizaos/plugin-bedrock") }, array<string>{ std::string("del"), std::string("@elizaos/plugin-knowledge") }, array<string>{ std::string("rm"), std::string("@elizaos/plugin-farcaster") } };
                for (auto& [command, plugin] : removeCommands)
                {
                    bunExecSync(std::string("elizaos plugins ") + command + std::string(" ") + plugin + string_empty, object{
                        object::pair{std::string("stdio"), std::string("pipe")}, 
                        object::pair{std::string("timeout"), TEST_TIMEOUTS["STANDARD_COMMAND"]}, 
                        object::pair{std::string("cwd"), projectDir}
                    });
                }
            }
            catch (const any& error)
            {
                console->error(std::string("[ERROR] Plugin remove aliases failed:"), error["message"]);
                console->error(std::string("[ERROR] stdout:"), OR((error["stdout"]["toString"]()), (std::string("none"))));
                console->error(std::string("[ERROR] stderr:"), OR((error["stderr"]["toString"]()), (std::string("none"))));
                throw any(error);
            }
        }
        , TEST_TIMEOUTS["PLUGIN_INSTALLATION"] + PLUGIN_INSTALLATION_BUFFER);
        it(std::string("plugins add fails for missing plugin"), [=]() mutable
        {
            try
            {
                bunExecSync(std::string("elizaos plugins add missing --skip-env-prompt"), object{
                    object::pair{std::string("stdio"), std::string("pipe")}, 
                    object::pair{std::string("timeout"), TEST_TIMEOUTS["STANDARD_COMMAND"]}, 
                    object::pair{std::string("cwd"), projectDir}
                });
                expect(false)->toBe(true);
            }
            catch (const any& e)
            {
                expect(e["status"])->not->toBe(0);
                auto output = OR((OR((e["stdout"]["toString"]()), (e["stderr"]["toString"]()))), (string_empty));
                expect(output)->toMatch((new RegExp(std::string("not found in registr"))));
            }
        }
        , TEST_TIMEOUTS["PLUGIN_INSTALLATION"] + PLUGIN_INSTALLATION_BUFFER);
        it(std::string("plugins add via GitHub shorthand URL"), [=]() mutable
        {
            try
            {
                bunExecSync(std::string("elizaos plugins add github:elizaos-plugins/plugin-farcaster#1.x --skip-env-prompt"), object{
                    object::pair{std::string("stdio"), std::string("pipe")}, 
                    object::pair{std::string("timeout"), TEST_TIMEOUTS["PLUGIN_INSTALLATION"]}, 
                    object::pair{std::string("cwd"), projectDir}
                });
                auto packageJson = std::async([=]() { readFile(join(projectDir, std::string("package.json")), std::string("utf8")); });
                expect(packageJson)->toContain(std::string("github:elizaos-plugins/plugin-farcaster#1.x"));
            }
            catch (const any& error)
            {
                console->error(std::string("[ERROR] GitHub shorthand plugin installation failed:"), error["message"]);
                console->error(std::string("[ERROR] stdout:"), OR((error["stdout"]["toString"]()), (std::string("none"))));
                console->error(std::string("[ERROR] stderr:"), OR((error["stderr"]["toString"]()), (std::string("none"))));
                throw any(error);
            }
        }
        , TEST_TIMEOUTS["PLUGIN_INSTALLATION"] + PLUGIN_INSTALLATION_BUFFER);
    }
    );
}

MAIN
