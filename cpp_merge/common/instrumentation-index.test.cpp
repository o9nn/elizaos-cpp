#include "instrumentation-index.test.h"

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
