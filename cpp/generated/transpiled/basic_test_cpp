#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/tests/unit/basic.test.h"

void Main(void)
{
    describe(std::string("Basic CLI Tests"), [=]() mutable
    {
        it(std::string("should run basic test"), [=]() mutable
        {
            expect(true)->toBe(true);
        }
        );
        it(std::string("should have process.env available"), [=]() mutable
        {
            expect(process->env)->toBeDefined();
            expect(type_of(process->env->NODE_ENV))->toBe(std::string("string"));
        }
        );
        it(std::string("should be able to set environment variables"), [=]() mutable
        {
            process->env->TEST_VAR = std::string("test-value");
            expect(process->env->TEST_VAR)->toBe(std::string("test-value"));
        }
        );
    }
    );
}

MAIN
