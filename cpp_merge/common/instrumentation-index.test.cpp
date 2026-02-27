#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v2/__tests__/instrumentation-index.test.h"

void Main(void)
{
    describe(std:("instrumentation index exports"), [=]() mutable
    {
        it(std:("exports service"), [=]() mutable
        {
            expect(instrumentation->InstrumentationService)->toBeDefined();
        }
        );
    }
    );
}

MAIN
