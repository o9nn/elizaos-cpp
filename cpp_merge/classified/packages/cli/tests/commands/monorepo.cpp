#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/tests/commands/monorepo.test.h"

void Main(void)
{
    describe(std:("ElizaOS Monorepo Commands"), [=]() mutable
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
        it(std:("monorepo --help shows usage"), [=]() mutable
        {
            auto result = bunExecSync(std:("elizaos monorepo --help"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expectHelpOutput(result, std:("monorepo"), array<string>{ std:("-b"), std:("--branch"), std:("-d"), std:("--dir") });
        }
        );
        it(std:("monorepo uses default branch and directory"), [=]() mutable
        {
            auto result = bunExecSync(std:("elizaos monorepo --help"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(result)->toContain(std:("Branch to install"));
            expect(result)->toContain(std:("develop"));
        }
        );
        it(std:("monorepo fails when directory is not empty"), [=]() mutable
        {
            std::async([=]() { mkdir(std:("not-empty-dir")); });
            std::async([=]() { writeFile(std:("not-empty-dir/placeholder"), string_empty); });
            try
            {
                bunExecSync(std:("elizaos monorepo --dir not-empty-dir"), object{
                    object::pair{std:("encoding"), std:("utf8")}
                });
                throw any(std::make_shared<Error>(std:("Command should have failed but succeeded")));
            }
            catch (const any& e)
            {
                expect(e["message"])->toContain(std:("Command failed"));
            }
        }
        );
    }
    );
}

MAIN
