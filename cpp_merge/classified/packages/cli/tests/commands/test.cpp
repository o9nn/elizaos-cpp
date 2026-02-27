#include "test.test.h"
#include <string>

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
            auto result = bunExecSync(std::string("elizaos test --help"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
            });
            expect(result)->toContain(std::string("Usage: elizaos test"));
        }
        );
        it(std::string("test command accepts -n option with quotes"), [=]() mutable
        {
            auto result = bunExecSync(std::string("elizaos test -n "filter-name" --help"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
            });
            expect(result)->toContain(std::string("Filter tests by name"));
        }
        );
        it(std::string("test command accepts -n option without quotes"), [=]() mutable
        {
            auto result = bunExecSync(std::string("elizaos test -n filter-name --help"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
            });
            expect(result)->toContain(std::string("Filter tests by name"));
        }
        );
        it(std::string("test command accepts --name option"), [=]() mutable
        {
            auto result = bunExecSync(std::string("elizaos test --name filter-name --help"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
            });
            expect(result)->toContain(std::string("Filter tests by name"));
        }
        );
        it(std::string("test component command accepts -n option"), [=]() mutable
        {
            auto result = bunExecSync(std::string("elizaos test component -n filter-name --help"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
            });
            expect(result)->toContain(std::string("component"));
        }
        );
        it(std::string("test e2e command accepts -n option"), [=]() mutable
        {
            auto result = bunExecSync(std::string("elizaos test e2e -n filter-name --help"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
            });
            expect(result)->toContain(std::string("e2e"));
        }
        );
        it(std::string("test command accepts --skip-build option"), [=]() mutable
        {
            auto result = bunExecSync(std::string("elizaos test --skip-build --help"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
            });
            expect(result)->toContain(std::string("Skip building before running tests"));
        }
        );
        it(std::string("test command accepts combination of options"), [=]() mutable
        {
            auto result = bunExecSync(std::string("elizaos test -n filter-name --skip-build --help"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
            });
            expect(result)->toContain(std::string("Filter tests by name"));
            expect(result)->toContain(std::string("Skip building before running tests"));
        }
        );
        it(std::string("test command handles basic name format"), [=]() mutable
        {
            auto result = bunExecSync(std::string("elizaos test -n basic --help"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
            });
            expect(result)->toContain(std::string("Usage: elizaos test"));
        }
        );
        it(std::string("test command handles .test name format"), [=]() mutable
        {
            auto result = bunExecSync(std::string("elizaos test -n basic.test --help"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
            });
            expect(result)->toContain(std::string("Usage: elizaos test"));
        }
        );
        it(std::string("test command handles .test.ts name format"), [=]() mutable
        {
            auto result = bunExecSync(std::string("elizaos test -n basic.test.ts --help"), object{
                object::pair{std::string("encoding"), std::string("utf8")}
            });
            expect(result)->toContain(std::string("Usage: elizaos test"));
        }
        );
    }
    );
}

MAIN
