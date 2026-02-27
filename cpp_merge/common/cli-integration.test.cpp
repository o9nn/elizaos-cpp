#include "cli-integration.test.h"

std::shared_ptr<Promise<std::shared_ptr<TestEnvironment>>> createTestEnvironment(boolean withTestKeys)
{
    auto tempDir = std::async([=]() { mkdtemp(join(tmpdir(), std:("eliza-auth-cli-test-"))); });
    auto envFile = join(tempDir, std:(".env"));
    auto originalEnv = utils::assign(object{
    }, process->env);
    if (withTestKeys) {
        auto envContent = std:("\
OPENAI_API_KEY="sk-test-elizaos-openai-key-for-development-only"\
GROQ_API_KEY="gsk_test-elizaos-groq-key-for-development-only"\
ANTHROPIC_API_KEY="sk-ant-test-elizaos-anthropic-key-for-development-only"\
NODE_ENV="test"\
LOG_LEVEL="error"\
")->trim();
        std::async([=]() { writeFile(envFile, envContent); });
    }
    return object{
        object::pair{std:("tempDir"), std:("tempDir")}, 
        object::pair{std:("envFile"), std:("envFile")}, 
        object::pair{std:("originalEnv"), std:("originalEnv")}
    };
};


std::shared_ptr<Promise<void>> cleanupTestEnvironment(std::shared_ptr<TestEnvironment> env)
{
    try
    {
        std::async([=]() { unlink(env->envFile)->_catch([=]() mutable
        {
        }
        ); });
        process->env = env->originalEnv;
    }
    catch (const any& error)
    {
        logger->warn(std:("Error during test environment cleanup:"), error);
    }
    return std::shared_ptr<Promise<void>>();
};


string execCLICommand(string command, std::shared_ptr<TestEnvironment> env)
{
    try
    {
        auto cliPath = join(__dirname, std:("../../../cli/dist/index.js"));
        auto fullCommand = std:("bun "") + cliPath + std:("" ") + command + string_empty;
        return execSync(fullCommand, object{
            object::pair{std:("encoding"), std:("utf8")}, 
            object::pair{std:("cwd"), env->tempDir}, 
            object::pair{std:("env"), utils::assign(object{
                , 
                object::pair{std:("NODE_ENV"), std:("test")}, 
                object::pair{std:("LOG_LEVEL"), std:("error")}
            }, env->originalEnv)}, 
            object::pair{std:("timeout"), 30000}, 
            object::pair{std:("stdio"), std:("pipe")}
        });
    }
    catch (const any& error)
    {
        return OR((OR((error["stdout"]), (error["stderr"]))), (error["message"]));
    }
};



void Main(void)
{
    describe(std:("CLI Authentication Integration"), [=]() mutable
    {
        shared<std::shared_ptr<TestEnvironment>> testEnv;
        afterEach([=]() mutable
        {
            if (testEnv) {
                std::async([=]() { cleanupTestEnvironment(testEnv); });
            }
        }
        );
        describe(std:("auth providers status"), [=]() mutable
        {
            it(std:("should show provider status with test keys"), [=]() mutable
            {
                testEnv = std::async([=]() { createTestEnvironment(true); });
                auto output = execCLICommand(std:("auth providers status"), testEnv);
                expect(output)->toContain(std:("AI Providers Authentication Status"));
                expect(output)->toContain(std:("OPENAI"));
                expect(output)->toContain(std:("GROQ"));
                expect(output)->toContain(std:("ANTHROPIC"));
                expect(output)->toContain(std:("TEST"));
            }
            );
            it(std:("should show failed status without API keys"), [=]() mutable
            {
                testEnv = std::async([=]() { createTestEnvironment(false); });
                auto output = execCLICommand(std:("auth providers status"), testEnv);
                expect(output)->toContain(std:("AI Providers Authentication Status"));
                expect(output->toUpperCase())->toContain(std:("FAILED"));
                expect(output)->toContain(std:("not configured"));
            }
            );
            it(std:("should provide helpful recommendations"), [=]() mutable
            {
                testEnv = std::async([=]() { createTestEnvironment(false); });
                auto output = execCLICommand(std:("auth providers status"), testEnv);
                expect(output)->toContain(std:("setup"));
            }
            );
        }
        );
        describe(std:("auth providers test"), [=]() mutable
        {
            it(std:("should test all providers with test keys"), [=]() mutable
            {
                testEnv = std::async([=]() { createTestEnvironment(true); });
                auto output = execCLICommand(std:("auth providers test"), testEnv);
                expect(output)->toContain(std:("AI Provider Functionality Test"));
                expect(output)->toContain(std:("openai"));
                expect(output)->toContain(std:("groq"));
                expect(output->toUpperCase())->toContain(std:("SUCCESS"));
            }
            );
            it(std:("should test specific provider"), [=]() mutable
            {
                testEnv = std::async([=]() { createTestEnvironment(true); });
                auto output = execCLICommand(std:("auth providers test --provider openai"), testEnv);
                expect(output)->toContain(std:("openai"));
                expect(output)->not->toContain(std:("groq"));
            }
            );
            it(std:("should handle invalid provider gracefully"), [=]() mutable
            {
                testEnv = std::async([=]() { createTestEnvironment(true); });
                auto output = execCLICommand(std:("auth providers test --provider invalid"), testEnv);
                expect(output->toLowerCase())->toContain(std:("invalid"));
            }
            );
            it(std:("should fail tests without API keys"), [=]() mutable
            {
                testEnv = std::async([=]() { createTestEnvironment(false); });
                auto output = execCLICommand(std:("auth providers test"), testEnv);
                expect(output->toLowerCase())->toContain(std:("not configured"));
            }
            );
        }
        );
        describe(std:("auth providers keys"), [=]() mutable
        {
            it(std:("should display test keys information"), [=]() mutable
            {
                testEnv = std::async([=]() { createTestEnvironment(false); });
                auto output = execCLICommand(std:("auth providers keys"), testEnv);
                expect(output)->toContain(std:("Development Test Keys"));
                expect(output)->toContain(std:("sk-test-elizaos-openai-key-for-development-only"));
                expect(output)->toContain(std:("gsk_test-elizaos-groq-key-for-development-only"));
                expect(output)->toContain(std:("sk-ant-test-elizaos-anthropic-key-for-development-only"));
                expect(output)->toContain(std:("OPENAI_API_KEY"));
            }
            );
            it(std:("should provide usage instructions"), [=]() mutable
            {
                testEnv = std::async([=]() { createTestEnvironment(false); });
                auto output = execCLICommand(std:("auth providers keys"), testEnv);
                expect(output)->toContain(std:("export"));
                expect(output)->toContain(std:("simulated responses"));
                expect(output)->toContain(std:("development"));
            }
            );
        }
        );
        describe(std:("auth providers setup"), [=]() mutable
        {
            it(std:("should show setup information"), [=]() mutable
            {
                testEnv = std::async([=]() { createTestEnvironment(false); });
                auto output = execCLICommand(std:("auth providers setup"), testEnv);
                expect(output)->toContain(std:("setup"));
            }
            );
            it(std:("should display provider information"), [=]() mutable
            {
                testEnv = std::async([=]() { createTestEnvironment(false); });
                auto output = execCLICommand(std:("auth providers setup"), testEnv);
                expect(output)->toContain(std:("OpenAI"));
                expect(output)->toContain(std:("Groq"));
                expect(output)->toContain(std:("Anthropic"));
            }
            );
        }
        );
        describe(std:("Integration with main auth command"), [=]() mutable
        {
            it(std:("should work as subcommand of auth"), [=]() mutable
            {
                testEnv = std::async([=]() { createTestEnvironment(true); });
                auto output = execCLICommand(std:("auth providers status"), testEnv);
                expect(output)->toContain(std:("Authentication Status"));
                expect(output)->not->toContain(std:("Command not found"));
                expect(output)->not->toContain(std:("Unknown command"));
            }
            );
            it(std:("should show help for providers subcommand"), [=]() mutable
            {
                testEnv = std::async([=]() { createTestEnvironment(false); });
                auto output = execCLICommand(std:("auth providers --help"), testEnv);
                expect(output)->toContain(std:("providers"));
                expect(output)->toContain(std:("status"));
                expect(output)->toContain(std:("setup"));
                expect(output)->toContain(std:("test"));
                expect(output)->toContain(std:("keys"));
            }
            );
            it(std:("should maintain consistency with main auth command"), [=]() mutable
            {
                testEnv = std::async([=]() { createTestEnvironment(true); });
                auto authOutput = execCLICommand(std:("auth status"), testEnv);
                auto providersOutput = execCLICommand(std:("auth providers status"), testEnv);
                expect(authOutput)->not->toContain(std:("error"));
                expect(providersOutput)->not->toContain(std:("error"));
                expect(authOutput)->not->toBe(providersOutput);
            }
            );
        }
        );
        describe(std:("Error Handling"), [=]() mutable
        {
            it(std:("should handle CLI errors gracefully"), [=]() mutable
            {
                testEnv = std::async([=]() { createTestEnvironment(false); });
                auto output = execCLICommand(std:("auth providers invalid-command"), testEnv);
                expect(output->toLowerCase())->toMatch((new RegExp(std:("error|invalid|unknow"))));
            }
            );
            it(std:("should provide helpful error messages"), [=]() mutable
            {
                testEnv = std::async([=]() { createTestEnvironment(false); });
                auto output = execCLICommand(std:("auth providers status"), testEnv);
                expect(output->toLowerCase())->toMatch((new RegExp(std:("no valid api keys|not configure"))));
                expect(output)->toContain(std:("setup"));
            }
            );
            it(std:("should handle timeout gracefully"), [=]() mutable
            {
                testEnv = std::async([=]() { createTestEnvironment(true); });
                auto startTime = Date->now();
                auto output = execCLICommand(std:("auth providers status"), testEnv);
                auto duration = Date->now() - startTime;
                expect(duration)->toBeLessThan(25000);
                expect(output)->toBeDefined();
            }
            );
        }
        );
        describe(std:("Environment File Integration"), [=]() mutable
        {
            it(std:("should read API keys from .env file"), [=]() mutable
            {
                testEnv = std::async([=]() { createTestEnvironment(true); });
                auto output = execCLICommand(std:("auth providers status"), testEnv);
                expect(output)->toContain(std:("TEST"));
                expect(output)->not->toContain(std:("not configured"));
            }
            );
            it(std:("should handle missing .env file"), [=]() mutable
            {
                testEnv = std::async([=]() { createTestEnvironment(false); });
                auto output = execCLICommand(std:("auth providers status"), testEnv);
                expect(output)->toContain(std:("AI Providers Authentication Status"));
                expect(output->toLowerCase())->toContain(std:("not configured"));
            }
            );
            it(std:("should work with partial .env configuration"), [=]() mutable
            {
                testEnv = std::async([=]() { createTestEnvironment(false); });
                auto partialEnv = std:("OPENAI_API_KEY="sk-test-elizaos-openai-key-for-development-only"");
                std::async([=]() { writeFile(testEnv->envFile, partialEnv); });
                auto output = execCLICommand(std:("auth providers status"), testEnv);
                expect(output->toUpperCase())->toContain(std:("DEGRADED"));
                expect(output)->toContain(std:("OPENAI"));
            }
            );
        }
        );
        describe(std:("Performance"), [=]() mutable
        {
            it(std:("should execute status check within reasonable time"), [=]() mutable
            {
                testEnv = std::async([=]() { createTestEnvironment(true); });
                auto startTime = Date->now();
                auto output = execCLICommand(std:("auth providers status"), testEnv);
                auto duration = Date->now() - startTime;
                expect(duration)->toBeLessThan(10000);
                expect(output)->toContain(std:("Status"));
            }
            );
            it(std:("should handle concurrent commands"), [=]() mutable
            {
                testEnv = std::async([=]() { createTestEnvironment(true); });
                auto output1 = execCLICommand(std:("auth providers status"), testEnv);
                auto output2 = execCLICommand(std:("auth providers keys"), testEnv);
                expect(output1)->toContain(std:("Status"));
                expect(output2)->toContain(std:("Test Keys"));
            }
            );
        }
        );
    }
    );
}

MAIN
