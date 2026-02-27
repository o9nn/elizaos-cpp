#include "plugins.test.h"

void Main(void)
{
    describe(std:("ElizaOS Plugin Commands"), [=]() mutable
    {
        shared<string> testTmpDir;
        shared<string> projectDir;
        shared<string> elizaosCmd;
        shared<string> originalCwd;
        beforeAll([=]() mutable
        {
            originalCwd = process->cwd();
            testTmpDir = std::async([=]() { mkdtemp(join(tmpdir(), std:("eliza-test-plugins-"))); });
            auto scriptDir = join(__dirname, std:(".."));
            elizaosCmd = std:("bun "") + join(scriptDir, std:("../dist/index.js")) + std:(""");
            projectDir = join(testTmpDir, std:("shared-test-project"));
            process->chdir(testTmpDir);
            console->log(std:("Creating shared test project..."));
            execSync(string_empty + elizaosCmd + std:(" create shared-test-project --yes"), getPlatformOptions(object{
                object::pair{std:("stdio"), std:("pipe")}, 
                object::pair{std:("timeout"), TEST_TIMEOUTS["PROJECT_CREATION"]}
            }));
            process->chdir(projectDir);
            console->log(std:("Shared test project created at:"), projectDir);
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
            auto result = execSync(string_empty + elizaosCmd + std:(" plugins"), getPlatformOptions(object{
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
            auto result = execSync(string_empty + elizaosCmd + std:(" plugins --help"), getPlatformOptions(object{
                object::pair{std:("encoding"), std:("utf8")}
            }));
            expect(result)->toContain(std:("Manage ElizaOS plugins"));
        }
        );
        it(std:("plugins list shows available plugins"), [=]() mutable
        {
            auto result = execSync(string_empty + elizaosCmd + std:(" plugins list"), getPlatformOptions(object{
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
                auto result = execSync(string_empty + elizaosCmd + std:(" plugins ") + alias + string_empty, getPlatformOptions(object{
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
                execSync(string_empty + elizaosCmd + std:(" plugins add @elizaos/plugin-xmtp --skip-env-prompt --skip-verification"), object{
                    object::pair{std:("stdio"), std:("pipe")}, 
                    object::pair{std:("timeout"), TEST_TIMEOUTS["PLUGIN_INSTALLATION"]}, 
                    object::pair{std:("cwd"), projectDir}
                });
                auto packageJson = std::async([=]() { readFile(join(projectDir, std:("package.json")), std:("utf8")); });
                expect(packageJson)->toContain(std:("@elizaos/plugin-xmtp"));
            }
            catch (const any& error)
            {
                console->error(std:("[ERROR] Plugin installation failed:"), error["message"]);
                console->error(std:("[ERROR] stdout:"), OR((error["stdout"]["toString"]()), (std:("none"))));
                console->error(std:("[ERROR] stderr:"), OR((error["stderr"]["toString"]()), (std:("none"))));
                throw any(error);
            }
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std:("plugins install alias works"), [=]() mutable
        {
            try
            {
                execSync(string_empty + elizaosCmd + std:(" plugins install @elizaos/plugin-mcp --skip-env-prompt --skip-verification"), object{
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
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std:("plugins add supports third-party plugins"), [=]() mutable
        {
            try
            {
                execSync(string_empty + elizaosCmd + std:(" plugins add @fleek-platform/eliza-plugin-mcp --skip-env-prompt --skip-verification"), object{
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
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std:("plugins add supports GitHub URL installation"), [=]() mutable
        {
            try
            {
                execSync(string_empty + elizaosCmd + std:(" plugins add https://github.com/elizaos-plugins/plugin-video-understanding --skip-env-prompt --skip-verification"), object{
                    object::pair{std:("stdio"), std:("pipe")}, 
                    object::pair{std:("timeout"), TEST_TIMEOUTS["PLUGIN_INSTALLATION"]}, 
                    object::pair{std:("cwd"), projectDir}
                });
                auto packageJson1 = std::async([=]() { readFile(join(projectDir, std:("package.json")), std:("utf8")); });
                expect(packageJson1)->toContain(std:("plugin-video-understanding"));
                execSync(string_empty + elizaosCmd + std:(" plugins add github:elizaos-plugins/plugin-openrouter#1.x --skip-env-prompt --skip-verification"), object{
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
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std:("plugins installed-plugins shows installed plugins"), [=]() mutable
        {
            auto result = execSync(string_empty + elizaosCmd + std:(" plugins installed-plugins"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(result)->toMatch((new RegExp(std:("@elizaos\/plugin-|github"))));
        }
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std:("plugins remove uninstalls a plugin"), [=]() mutable
        {
            try
            {
                execSync(string_empty + elizaosCmd + std:(" plugins add @elizaos/plugin-elevenlabs --skip-env-prompt --skip-verification"), object{
                    object::pair{std:("stdio"), std:("pipe")}, 
                    object::pair{std:("timeout"), TEST_TIMEOUTS["PLUGIN_INSTALLATION"]}, 
                    object::pair{std:("cwd"), projectDir}
                });
                auto packageJson = std::async([=]() { readFile(join(projectDir, std:("package.json")), std:("utf8")); });
                expect(packageJson)->toContain(std:("@elizaos/plugin-elevenlabs"));
                execSync(string_empty + elizaosCmd + std:(" plugins remove @elizaos/plugin-elevenlabs"), object{
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
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std:("plugins remove aliases (delete, del, rm) work"), [=]() mutable
        {
            try
            {
                auto plugins = array<string>{ std:("@elizaos/plugin-bedrock"), std:("@elizaos/plugin-knowledge"), std:("@elizaos/plugin-farcaster") };
                for (auto& plugin : plugins)
                {
                    execSync(string_empty + elizaosCmd + std:(" plugins add ") + plugin + std:(" --skip-env-prompt --skip-verification"), object{
                        object::pair{std:("stdio"), std:("pipe")}, 
                        object::pair{std:("timeout"), TEST_TIMEOUTS["PLUGIN_INSTALLATION"]}, 
                        object::pair{std:("cwd"), projectDir}
                    });
                }
                auto removeCommands = array<array<string>>{ array<string>{ std:("delete"), std:("@elizaos/plugin-bedrock") }, array<string>{ std:("del"), std:("@elizaos/plugin-knowledge") }, array<string>{ std:("rm"), std:("@elizaos/plugin-farcaster") } };
                for (auto& [command, plugin] : removeCommands)
                {
                    execSync(string_empty + elizaosCmd + std:(" plugins ") + command + std:(" ") + plugin + string_empty, object{
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
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std:("plugins add fails for missing plugin"), [=]() mutable
        {
            try
            {
                execSync(string_empty + elizaosCmd + std:(" plugins add missing --skip-env-prompt"), object{
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
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
        it(std:("plugins add via GitHub shorthand URL"), [=]() mutable
        {
            try
            {
                execSync(string_empty + elizaosCmd + std:(" plugins add github:elizaos-plugins/plugin-farcaster#1.x --skip-env-prompt --skip-verification"), object{
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
        , TEST_TIMEOUTS["INDIVIDUAL_TEST"]);
    }
    );
}

MAIN
