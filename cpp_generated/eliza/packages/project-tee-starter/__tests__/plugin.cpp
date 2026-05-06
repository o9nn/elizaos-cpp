#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-tee-starter/__tests__/plugin.test.h"

void Main(void)
{
    spyOn(logger, std::string("info"));
    spyOn(logger, std::string("error"));
    spyOn(logger, std::string("warn"));
    describe(std::string("TEE Starter Plugin"), [=]() mutable
    {
        it(std::string("should have the correct name"), [=]() mutable
        {
            expect(teeStarterPlugin->name)->toBe(std::string("mr-tee-starter-plugin"));
        }
        );
        it(std::string("should have the correct description"), [=]() mutable
        {
            expect(teeStarterPlugin->description)->toBe(std::string("Mr. TEE's starter plugin - using plugin-tee for attestation"));
        }
        );
        it(std::string("should have no custom actions (using plugin-tee instead)"), [=]() mutable
        {
            expect(teeStarterPlugin->actions)->toEqual(array<any>());
        }
        );
        it(std::string("should have no custom providers"), [=]() mutable
        {
            expect(teeStarterPlugin->providers)->toEqual(array<any>());
        }
        );
        it(std::string("should have no custom evaluators"), [=]() mutable
        {
            expect(teeStarterPlugin->evaluators)->toBeUndefined();
        }
        );
        it(std::string("should have no custom services"), [=]() mutable
        {
            expect(teeStarterPlugin->services)->toEqual(array<any>());
        }
        );
    }
    );
}

MAIN
