#include "basic.test.h"

void Main(void)
{
    describe(std:("Basic CLI Tests"), [=]() mutable
    {
        it(std:("should run basic test"), [=]() mutable
        {
            expect(true)->toBe(true);
        }
        );
        it(std:("should have process.env available"), [=]() mutable
        {
            expect(process->env)->toBeDefined();
            expect(type_of(process->env->NODE_ENV))->toBe(std:("string"));
        }
        );
        it(std:("should be able to set environment variables"), [=]() mutable
        {
            process->env->TEST_VAR = std:("test-value");
            expect(process->env->TEST_VAR)->toBe(std:("test-value"));
        }
        );
    }
    );
}

MAIN
