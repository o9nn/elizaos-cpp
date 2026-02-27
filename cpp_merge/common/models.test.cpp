#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-tee-starter/__tests__/models.test.h"

void Main(void)
{
    describe(std:("Plugin Models"), [=]() mutable
    {
        it(std:("should not have custom models (using plugin-tee and other plugins for model handling)"), [=]() mutable
        {
            expect(teeStarterPlugin->models)->toBeUndefined();
        }
        );
        it(std:("should rely on other plugins for model functionality"), [=]() mutable
        {
            expect(teeStarterPlugin->name)->toBe(std:("mr-tee-starter-plugin"));
            expect(teeStarterPlugin->description)->toBe(std:("Mr. TEE's starter plugin - using plugin-tee for attestation"));
            expect(teeStarterPlugin->actions)->toEqual(array<any>());
            expect(teeStarterPlugin->providers)->toEqual(array<any>());
        }
        );
    }
    );
}

MAIN
