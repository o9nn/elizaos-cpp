#include "env.test.h"
#include <string>

void Main(void)
{
    describe(std::string("ElizaOS Env Commands"), [=]() mutable
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
        it(std::string("env --help shows usage"), [=]() mutable
        {
            auto result = bunExecSync(std::string("elizaos env --help"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
            });
            expectHelpOutput(result, std::string("env"));
        }
        );
        it(std::string("env list shows environment variables"), [=]() mutable
        {
            auto result = bunExecSync(std::string("elizaos env list"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
            });
            auto expectedSections = array<string>{ std::string("System Information"), std::string("Local Environment Variables") };
            for (auto& section : expectedSections)
            {
                expect(result)->toContain(section);
            }
            expect(result)->toMatch((new RegExp(std::string("(No local \.env file found|Missing \.env file"))));
            std::async([=]() { writeFile(std::string(".env"), std::string("TEST_VAR=test_value")); });
            result = bunExecSync(std::string("elizaos env list"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
            });
            expect(result)->toContain(std::string("TEST_VAR"));
            expect(result)->toContain(std::string("test_value"));
        }
        );
        it(std::string("env list --local shows only local environment"), [=]() mutable
        {
            std::async([=]() { writeFile(std::string(".env"), std::string("LOCAL_TEST=local_value")); });
            auto result = bunExecSync(std::string("elizaos env list --local"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
            });
            expect(result)->toContain(std::string("LOCAL_TEST"));
            expect(result)->toContain(std::string("local_value"));
            expect(result)->not->toContain(std::string("System Information"));
        }
        );
        it(std::string("env edit-local creates local .env if missing"), [=]() mutable
        {
            if (process->platform == std::string("win32")) {
                console->warn(std::string("Skipping env edit-local test on Windows due to shell input limitations"));
                return std::shared_ptr<Promise<void>>();
            }
            auto result = bunExecSync(std::string("printf "y\n" | elizaos env edit-local"), object{
                object::pair{std::string("encoding"), std::string("utf8")}, 
                object::pair{std::string("shell"), std::string("/bin/bash")}
            });
            expect(result)->toBeTruthy();
        }
        );
        it(std::string("env reset shows all necessary options"), [=]() mutable
        {
            std::async([=]() { writeFile(std::string(".env"), std::string("DUMMY=value")); });
            auto result = bunExecSync(std::string("elizaos env reset --yes"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
            });
            expect(result)->toContain(std::string("Reset Summary"));
            expect(result)->toContain(std::string("Local environment variables"));
            expect(result)->toContain(std::string("Environment reset complete"));
        }
        );
    }
    );
}

MAIN
