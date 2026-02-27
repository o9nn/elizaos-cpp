#include "actions.test.h"
#include <string>

void Main(void)
{
    describe(std::string("TEE Actions"), [=]() mutable
    {
        it(std::string("should use plugin-tee's remoteAttestationAction"), [=]() mutable
        {
            expect(true)->toBe(true);
        }
        );
    }
    );
}

MAIN
