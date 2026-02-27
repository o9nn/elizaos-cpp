#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/tests/commands/env.test.h"

void Main(void)
{
    describe(std:("ElizaOS Env Commands"), [=]() mutable
    {
        shared<std::shared_ptr<TestContext>> context;
        beforeEach([=]() mutable
        {
            context = std::async([=]() { setupTestEnvironment(); });
        }
        );
        afterEach([=]() mutable
        {
            std::async([=]() { cleanupTestEnvironment(context); });
        }
        );
        it(std:("env --help shows usage"), [=]() mutable
        {
            auto result = bunExecSync(std:("elizaos env --help"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expectHelpOutput(result, std:("env"));
        }
        );
        it(std:("env list shows environment variables"), [=]() mutable
        {
            auto result = bunExecSync(std:("elizaos env list"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            auto expectedSections = array<string>{ std:("System Information"), std:("Local Environment Variables") };
            for (auto& section : expectedSections)
            {
                expect(result)->toContain(section);
            }
            expect(result)->toMatch((new RegExp(std:("(No local \.env file found|Missing \.env file"))));
            std::async([=]() { writeFile(std:(".env"), std:("TEST_VAR=test_value")); });
            result = bunExecSync(std:("elizaos env list"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(result)->toContain(std:("TEST_VAR"));
            expect(result)->toContain(std:("test_value"));
        }
        );
        it(std:("env list --local shows only local environment"), [=]() mutable
        {
            std::async([=]() { writeFile(std:(".env"), std:("LOCAL_TEST=local_value")); });
            auto result = bunExecSync(std:("elizaos env list --local"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(result)->toContain(std:("LOCAL_TEST"));
            expect(result)->toContain(std:("local_value"));
            expect(result)->not->toContain(std:("System Information"));
        }
        );
        it(std:("env edit-local creates local .env if missing"), [=]() mutable
        {
            if (process->platform == std:("win32")) {
                console->warn(std:("Skipping env edit-local test on Windows due to shell input limitations"));
                return std::shared_ptr<Promise<void>>();
            }
            auto result = bunExecSync(std:("printf "y\n" | elizaos env edit-local"), object{
                object::pair{std:("encoding"), std:("utf8")}, 
                object::pair{std:("shell"), std:("/bin/bash")}
            });
            expect(result)->toBeTruthy();
        }
        );
        it(std:("env reset shows all necessary options"), [=]() mutable
        {
            std::async([=]() { writeFile(std:(".env"), std:("DUMMY=value")); });
            auto result = bunExecSync(std:("elizaos env reset --yes"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(result)->toContain(std:("Reset Summary"));
            expect(result)->toContain(std:("Local environment variables"));
            expect(result)->toContain(std:("Environment reset complete"));
        }
        );
    }
    );
}

MAIN
