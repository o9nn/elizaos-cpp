#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/tests/commands/test.test.h"

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
            auto result = bunExecSync(std:("elizaos test --help"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(result)->toContain(std:("Usage: elizaos test"));
        }
        );
        it(std:("test command accepts -n option with quotes"), [=]() mutable
        {
            auto result = bunExecSync(std:("elizaos test -n "filter-name" --help"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(result)->toContain(std:("Filter tests by name"));
        }
        );
        it(std:("test command accepts -n option without quotes"), [=]() mutable
        {
            auto result = bunExecSync(std:("elizaos test -n filter-name --help"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(result)->toContain(std:("Filter tests by name"));
        }
        );
        it(std:("test command accepts --name option"), [=]() mutable
        {
            auto result = bunExecSync(std:("elizaos test --name filter-name --help"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(result)->toContain(std:("Filter tests by name"));
        }
        );
        it(std:("test component command accepts -n option"), [=]() mutable
        {
            auto result = bunExecSync(std:("elizaos test component -n filter-name --help"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(result)->toContain(std:("component"));
        }
        );
        it(std:("test e2e command accepts -n option"), [=]() mutable
        {
            auto result = bunExecSync(std:("elizaos test e2e -n filter-name --help"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(result)->toContain(std:("e2e"));
        }
        );
        it(std:("test command accepts --skip-build option"), [=]() mutable
        {
            auto result = bunExecSync(std:("elizaos test --skip-build --help"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(result)->toContain(std:("Skip building before running tests"));
        }
        );
        it(std:("test command accepts combination of options"), [=]() mutable
        {
            auto result = bunExecSync(std:("elizaos test -n filter-name --skip-build --help"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(result)->toContain(std:("Filter tests by name"));
            expect(result)->toContain(std:("Skip building before running tests"));
        }
        );
        it(std:("test command handles basic name format"), [=]() mutable
        {
            auto result = bunExecSync(std:("elizaos test -n basic --help"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(result)->toContain(std:("Usage: elizaos test"));
        }
        );
        it(std:("test command handles .test name format"), [=]() mutable
        {
            auto result = bunExecSync(std:("elizaos test -n basic.test --help"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(result)->toContain(std:("Usage: elizaos test"));
        }
        );
        it(std:("test command handles .test.ts name format"), [=]() mutable
        {
            auto result = bunExecSync(std:("elizaos test -n basic.test.ts --help"), object{
                object::pair{std:("encoding"), std:("utf8")}
            });
            expect(result)->toContain(std:("Usage: elizaos test"));
        }
        );
    }
    );
}

MAIN
