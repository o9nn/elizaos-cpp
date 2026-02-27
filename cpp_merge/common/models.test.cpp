#include "models.test.h"
#include <string>

void Main(void)
{
    describe(std::string("Plugin Models"), [=]() mutable
    {
        it(std::string("should not have custom models (using plugin-tee and other plugins for model handling)"), [=]() mutable
        {
            expect(teeStarterPlugin->models)->toBeUndefined();
        }
        );
        it(std::string("should rely on other plugins for model functionality"), [=]() mutable
        {
            expect(teeStarterPlugin->name)->toBe(std::string("mr-tee-starter-plugin"));
            expect(teeStarterPlugin->description)->toBe(std::string("Mr. TEE's starter plugin - using plugin-tee for attestation"));
            expect(teeStarterPlugin->actions)->toEqual(array<any>());
            expect(teeStarterPlugin->providers)->toEqual(array<any>());
        }
        );
    }
    );
}

MAIN
