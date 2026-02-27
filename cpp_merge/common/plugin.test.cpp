#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-tee-starter/__tests__/plugin.test.h"

void Main(void)
{
    spyOn(logger, std:("info"));
    spyOn(logger, std:("error"));
    spyOn(logger, std:("warn"));
    describe(std:("TEE Starter Plugin"), [=]() mutable
    {
        it(std:("should have the correct name"), [=]() mutable
        {
            expect(teeStarterPlugin->name)->toBe(std:("mr-tee-starter-plugin"));
        }
        );
        it(std:("should have the correct description"), [=]() mutable
        {
            expect(teeStarterPlugin->description)->toBe(std:("Mr. TEE's starter plugin - using plugin-tee for attestation"));
        }
        );
        it(std:("should have no custom actions (using plugin-tee instead)"), [=]() mutable
        {
            expect(teeStarterPlugin->actions)->toEqual(array<any>());
        }
        );
        it(std:("should have no custom providers"), [=]() mutable
        {
            expect(teeStarterPlugin->providers)->toEqual(array<any>());
        }
        );
        it(std:("should have no custom evaluators"), [=]() mutable
        {
            expect(teeStarterPlugin->evaluators)->toBeUndefined();
        }
        );
        it(std:("should have no custom services"), [=]() mutable
        {
            expect(teeStarterPlugin->services)->toEqual(array<any>());
        }
        );
    }
    );
}

MAIN
