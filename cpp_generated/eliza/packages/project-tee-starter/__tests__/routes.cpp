#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-tee-starter/__tests__/routes.test.h"

void Main(void)
{
    describe(std::string("Plugin Routes"), [=]() mutable
    {
        it(std::string("should not have custom routes (relies on other plugins for HTTP endpoints)"), [=]() mutable
        {
            expect(teeStarterPlugin->routes)->toBeDefined();
            expect(teeStarterPlugin->routes->length)->toBe(1);
            expect(const_(teeStarterPlugin->routes)[0]->name)->toBe(std::string("mr-tee-status-route"));
            expect(const_(teeStarterPlugin->routes)[0]->path)->toBe(std::string("/mr-tee-status"));
            expect(const_(teeStarterPlugin->routes)[0]->type)->toBe(std::string("GET"));
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
