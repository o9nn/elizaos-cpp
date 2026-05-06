#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/tests/commands/monorepo.test.h"

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
            auto result = runCliCommand(context->elizaosCmd, std::string("monorepo --help"));
            expectHelpOutput(result, std::string("monorepo"), array<string>{ std::string("-b"), std::string("--branch"), std::string("-d"), std::string("--dir") });
        }
        );
        it(std::string("monorepo uses default branch and directory"), [=]() mutable
        {
            auto result = runCliCommand(context->elizaosCmd, std::string("monorepo --help"));
            expect(result)->toContain(std::string("Branch to install"));
            expect(result)->toContain(std::string("develop"));
        }
        );
        it(std::string("monorepo fails when directory is not empty"), [=]() mutable
        {
            std::async([=]() { mkdir(std::string("not-empty-dir")); });
            std::async([=]() { writeFile(std::string("not-empty-dir/placeholder"), string_empty); });
            auto result = expectCliCommandToFail(context->elizaosCmd, std::string("monorepo --dir not-empty-dir"), object{
                object::pair{std::string("timeout"), TEST_TIMEOUTS["QUICK_COMMAND"]}
            });
            expect(result["status"])->not->toBe(0);
            expect(result["output"])->toMatch((new RegExp(std::string("not empt"))));
        }
        );
    }
    );
}

MAIN
