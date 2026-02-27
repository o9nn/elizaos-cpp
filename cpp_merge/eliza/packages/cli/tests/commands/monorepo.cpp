#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/tests/commands/monorepo.test.h"

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
            auto result = runCliCommand(context->elizaosCmd, std:("monorepo --help"));
            expectHelpOutput(result, std:("monorepo"), array<string>{ std:("-b"), std:("--branch"), std:("-d"), std:("--dir") });
        }
        );
        it(std:("monorepo uses default branch and directory"), [=]() mutable
        {
            auto result = runCliCommand(context->elizaosCmd, std:("monorepo --help"));
            expect(result)->toContain(std:("Branch to install"));
            expect(result)->toContain(std:("develop"));
        }
        );
        it(std:("monorepo fails when directory is not empty"), [=]() mutable
        {
            std::async([=]() { mkdir(std:("not-empty-dir")); });
            std::async([=]() { writeFile(std:("not-empty-dir/placeholder"), string_empty); });
            auto result = expectCliCommandToFail(context->elizaosCmd, std:("monorepo --dir not-empty-dir"), object{
                object::pair{std:("timeout"), TEST_TIMEOUTS["QUICK_COMMAND"]}
            });
            expect(result["status"])->not->toBe(0);
            expect(result["output"])->toMatch((new RegExp(std:("not empt"))));
        }
        );
    }
    );
}

MAIN
