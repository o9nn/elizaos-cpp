#include "routes.test.h"

void Main(void)
{
    describe(std:("Plugin Routes"), [=]() mutable
    {
        it(std:("should not have custom routes (relies on other plugins for HTTP endpoints)"), [=]() mutable
        {
            expect(teeStarterPlugin->routes)->toBeDefined();
            expect(teeStarterPlugin->routes->length)->toBe(1);
            expect(const_(teeStarterPlugin->routes)[0]->name)->toBe(std:("mr-tee-status-route"));
            expect(const_(teeStarterPlugin->routes)[0]->path)->toBe(std:("/mr-tee-status"));
            expect(const_(teeStarterPlugin->routes)[0]->type)->toBe(std:("GET"));
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
