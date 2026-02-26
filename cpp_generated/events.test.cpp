#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-tee-starter/__tests__/events.test.h"

void Main(void)
{
    describe(std::string("Plugin Events"), [=]() mutable
    {
        it(std::string("should not have custom events (relies on plugin-tee for TEE events)"), [=]() mutable
        {
            expect(teeStarterPlugin->events)->toBeDefined();
            expect(teeStarterPlugin->events->MESSAGE_RECEIVED)->toBeDefined();
            expect(teeStarterPlugin->events->VOICE_MESSAGE_RECEIVED)->toBeDefined();
            expect(teeStarterPlugin->events->WORLD_CONNECTED)->toBeDefined();
            expect(teeStarterPlugin->events->WORLD_JOINED)->toBeDefined();
        }
        );
        it(std::string("should have correct plugin configuration"), [=]() mutable
        {
            expect(teeStarterPlugin)->toBeDefined();
            expect(teeStarterPlugin->name)->toBe(std::string("mr-tee-starter-plugin"));
            expect(teeStarterPlugin->description)->toBe(std::string("Mr. TEE's starter plugin - using plugin-tee for attestation"));
        }
        );
    }
    );
}

MAIN
