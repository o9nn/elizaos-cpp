#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/tests/commands/monorepo.test.h"

void Main(void)
{
    describe(std::string("ElizaOS Monorepo Commands"), [=]() mutable
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
        it(std::string("monorepo --help shows usage"), [=]() mutable
        {
            auto result = bunExecSync(std::string("elizaos monorepo --help"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
            });
            expectHelpOutput(result, std::string("monorepo"), array<string>{ std::string("-b"), std::string("--branch"), std::string("-d"), std::string("--dir") });
        }
        );
        it(std::string("monorepo uses default branch and directory"), [=]() mutable
        {
            auto result = bunExecSync(std::string("elizaos monorepo --help"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
            });
            expect(result)->toContain(std::string("Branch to install"));
            expect(result)->toContain(std::string("develop"));
        }
        );
        it(std::string("monorepo fails when directory is not empty"), [=]() mutable
        {
            std::async([=]() { mkdir(std::string("not-empty-dir")); });
            std::async([=]() { writeFile(std::string("not-empty-dir/placeholder"), string_empty); });
            try
            {
                bunExecSync(std::string("elizaos monorepo --dir not-empty-dir"), object{
                    object::pair{std::string("encoding"), std::string("utf8")}
                });
                throw any(std::make_shared<Error>(std::string("Command should have failed but succeeded")));
            }
            catch (const any& e)
            {
                expect(e["message"])->toContain(std::string("Command failed"));
            }
        }
        );
    }
    );
}

MAIN
