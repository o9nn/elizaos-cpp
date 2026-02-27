#include "test.test.h"

void Main(void)
{
    describe(std:("ElizaOS Test Commands"), [=]() mutable
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
        it(std:("test --help shows usage"), [=]() mutable
        {
            auto result = runCliCommand(context->elizaosCmd, std:("test --help"));
            expectHelpOutput(result, std:("test"));
        }
        );
        it(std:("test command accepts -n option with quotes"), [=]() mutable
        {
            auto result = runCliCommand(context->elizaosCmd, std:("test -n "filter-name" --help"));
            expect(result)->toContain(std:("Filter tests by name"));
        }
        );
        it(std:("test command accepts -n option without quotes"), [=]() mutable
        {
            auto result = runCliCommand(context->elizaosCmd, std:("test -n filter-name --help"));
            expect(result)->toContain(std:("Filter tests by name"));
        }
        );
        it(std:("test command accepts --name option"), [=]() mutable
        {
            auto result = runCliCommand(context->elizaosCmd, std:("test --name filter-name --help"));
            expect(result)->toContain(std:("Filter tests by name"));
        }
        );
        it(std:("test component command accepts -n option"), [=]() mutable
        {
            auto result = runCliCommand(context->elizaosCmd, std:("test component -n filter-name --help"));
            expect(result)->toContain(std:("component"));
        }
        );
        it(std:("test e2e command accepts -n option"), [=]() mutable
        {
            auto result = runCliCommand(context->elizaosCmd, std:("test e2e -n filter-name --help"));
            expect(result)->toContain(std:("e2e"));
        }
        );
        it(std:("test command accepts --skip-build option"), [=]() mutable
        {
            auto result = runCliCommand(context->elizaosCmd, std:("test --skip-build --help"));
            expect(result)->toContain(std:("Skip building before running tests"));
        }
        );
        it(std:("test command accepts combination of options"), [=]() mutable
        {
            auto result = runCliCommand(context->elizaosCmd, std:("test -n filter-name --skip-build --help"));
            expect(result)->toContain(std:("Filter tests by name"));
            expect(result)->toContain(std:("Skip building before running tests"));
        }
        );
        it(std:("test command handles basic name format"), [=]() mutable
        {
            auto result = runCliCommand(context->elizaosCmd, std:("test -n basic --help"));
            expectHelpOutput(result, std:("test"));
        }
        );
        it(std:("test command handles .test name format"), [=]() mutable
        {
            auto result = runCliCommand(context->elizaosCmd, std:("test -n basic.test --help"));
            expectHelpOutput(result, std:("test"));
        }
        );
        it(std:("test command handles .test.ts name format"), [=]() mutable
        {
            auto result = runCliCommand(context->elizaosCmd, std:("test -n basic.test.ts --help"));
            expectHelpOutput(result, std:("test"));
        }
        );
    }
    );
}

MAIN
