#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-tee-starter/__tests__/events.test.h"

void Main(void)
{
    describe(std:("Plugin Events"), [=]() mutable
    {
        it(std:("should not have custom events (relies on plugin-tee for TEE events)"), [=]() mutable
        {
            expect(teeStarterPlugin->events)->toBeDefined();
            expect(teeStarterPlugin->events->MESSAGE_RECEIVED)->toBeDefined();
            expect(teeStarterPlugin->events->VOICE_MESSAGE_RECEIVED)->toBeDefined();
            expect(teeStarterPlugin->events->WORLD_CONNECTED)->toBeDefined();
            expect(teeStarterPlugin->events->WORLD_JOINED)->toBeDefined();
        }
        );
        it(std:("should have correct plugin configuration"), [=]() mutable
        {
            expect(teeStarterPlugin)->toBeDefined();
            expect(teeStarterPlugin->name)->toBe(std:("mr-tee-starter-plugin"));
            expect(teeStarterPlugin->description)->toBe(std:("Mr. TEE's starter plugin - using plugin-tee for attestation"));
        }
        );
    }
    );
}

MAIN
