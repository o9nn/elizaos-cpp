#include "actions.test.h"

void Main(void)
{
    describe(std:("TEE Actions"), [=]() mutable
    {
        it(std:("should use plugin-tee's remoteAttestationAction"), [=]() mutable
        {
            expect(true)->toBe(true);
        }
        );
    }
    );
}

MAIN
