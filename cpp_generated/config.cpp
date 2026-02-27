#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-tee-starter/__tests__/config.test.h"

void Main(void)
{
    describe(std::string("Plugin Configuration"), [=]() mutable
    {
        it(std::string("should not have custom configuration (relies on character settings)"), [=]() mutable
        {
            expect(teeStarterPlugin->config)->toBeDefined();
            expect(teeStarterPlugin->config->TEE_MODE)->toBe(process->env->TEE_MODE);
            expect(teeStarterPlugin->config->WALLET_SECRET_SALT)->toBe(process->env->WALLET_SECRET_SALT);
            expect(teeStarterPlugin->init)->toBeDefined();
        }
        );
        it(std::string("should have correct plugin metadata"), [=]() mutable
        {
            expect(teeStarterPlugin)->toBeDefined();
            expect(teeStarterPlugin->name)->toBe(std::string("mr-tee-starter-plugin"));
            expect(teeStarterPlugin->description)->toBe(std::string("Mr. TEE's starter plugin - using plugin-tee for attestation"));
        }
        );
        it(std::string("should be a minimal plugin focused on character definition"), [=]() mutable
        {
            expect(teeStarterPlugin->actions)->toEqual(array<any>());
            expect(teeStarterPlugin->providers)->toEqual(array<any>());
            expect(teeStarterPlugin->evaluators)->toBeUndefined();
            expect(teeStarterPlugin->services)->toEqual(array<any>());
            expect(teeStarterPlugin->routes)->toBeDefined();
            expect(teeStarterPlugin->routes->length)->toBeGreaterThan(0);
            expect(teeStarterPlugin->events)->toBeDefined();
        }
        );
    }
    );
}

MAIN
