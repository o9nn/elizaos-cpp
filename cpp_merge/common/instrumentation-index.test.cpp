#include "instrumentation-index.test.h"
#include <string>

void Main(void)
{
    describe(std::string("instrumentation index exports"), [=]() mutable
    {
        it(std::string("exports service"), [=]() mutable
        {
            expect(instrumentation->InstrumentationService)->toBeDefined();
        }
        );
    }
    );
}

MAIN
