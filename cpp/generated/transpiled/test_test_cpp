#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/tests/commands/test.test.h"

void Main(void)
{
    describe(std::string("ElizaOS Test Commands"), [=]() mutable
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
        it(std::string("test --help shows usage"), [=]() mutable
        {
            auto result = runCliCommand(context->elizaosCmd, std::string("test --help"));
            expectHelpOutput(result, std::string("test"));
        }
        );
        it(std::string("test command accepts -n option with quotes"), [=]() mutable
        {
            auto result = runCliCommand(context->elizaosCmd, std::string("test -n "filter-name" --help"));
            expect(result)->toContain(std::string("Filter tests by name"));
        }
        );
        it(std::string("test command accepts -n option without quotes"), [=]() mutable
        {
            auto result = runCliCommand(context->elizaosCmd, std::string("test -n filter-name --help"));
            expect(result)->toContain(std::string("Filter tests by name"));
        }
        );
        it(std::string("test command accepts --name option"), [=]() mutable
        {
            auto result = runCliCommand(context->elizaosCmd, std::string("test --name filter-name --help"));
            expect(result)->toContain(std::string("Filter tests by name"));
        }
        );
        it(std::string("test component command accepts -n option"), [=]() mutable
        {
            auto result = runCliCommand(context->elizaosCmd, std::string("test component -n filter-name --help"));
            expect(result)->toContain(std::string("component"));
        }
        );
        it(std::string("test e2e command accepts -n option"), [=]() mutable
        {
            auto result = runCliCommand(context->elizaosCmd, std::string("test e2e -n filter-name --help"));
            expect(result)->toContain(std::string("e2e"));
        }
        );
        it(std::string("test command accepts --skip-build option"), [=]() mutable
        {
            auto result = runCliCommand(context->elizaosCmd, std::string("test --skip-build --help"));
            expect(result)->toContain(std::string("Skip building before running tests"));
        }
        );
        it(std::string("test command accepts combination of options"), [=]() mutable
        {
            auto result = runCliCommand(context->elizaosCmd, std::string("test -n filter-name --skip-build --help"));
            expect(result)->toContain(std::string("Filter tests by name"));
            expect(result)->toContain(std::string("Skip building before running tests"));
        }
        );
        it(std::string("test command handles basic name format"), [=]() mutable
        {
            auto result = runCliCommand(context->elizaosCmd, std::string("test -n basic --help"));
            expectHelpOutput(result, std::string("test"));
        }
        );
        it(std::string("test command handles .test name format"), [=]() mutable
        {
            auto result = runCliCommand(context->elizaosCmd, std::string("test -n basic.test --help"));
            expectHelpOutput(result, std::string("test"));
        }
        );
        it(std::string("test command handles .test.ts name format"), [=]() mutable
        {
            auto result = runCliCommand(context->elizaosCmd, std::string("test -n basic.test.ts --help"));
            expectHelpOutput(result, std::string("test"));
        }
        );
    }
    );
}

MAIN
