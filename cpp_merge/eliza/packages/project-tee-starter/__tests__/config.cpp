#include "config.test.h"

void Main(void)
{
    describe(std:("Plugin Configuration"), [=]() mutable
    {
        it(std:("should not have custom configuration (relies on character settings)"), [=]() mutable
        {
            expect(teeStarterPlugin->config)->toBeDefined();
            expect(teeStarterPlugin->config->TEE_MODE)->toBe(process->env->TEE_MODE);
            expect(teeStarterPlugin->config->WALLET_SECRET_SALT)->toBe(process->env->WALLET_SECRET_SALT);
            expect(teeStarterPlugin->init)->toBeDefined();
        }
        );
        it(std:("should have correct plugin metadata"), [=]() mutable
        {
            expect(teeStarterPlugin)->toBeDefined();
            expect(teeStarterPlugin->name)->toBe(std:("mr-tee-starter-plugin"));
            expect(teeStarterPlugin->description)->toBe(std:("Mr. TEE's starter plugin - using plugin-tee for attestation"));
        }
        );
        it(std:("should be a minimal plugin focused on character definition"), [=]() mutable
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
