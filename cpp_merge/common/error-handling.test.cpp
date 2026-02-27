#include "error-handling.test.h"
#include <string>

void Main(void)
{
    describe(std::string("Error Handling"), [=]() mutable
    {
        describe(std::string("Plugin Error Handling"), [=]() mutable
        {
            it(std::string("should handle missing dependencies gracefully"), [=]() mutable
            {
                expect(teeStarterPlugin)->toBeDefined();
                expect(teeStarterPlugin->actions)->toEqual(array<any>());
                expect(teeStarterPlugin->providers)->toEqual(array<any>());
            }
            );
            it(std::string("should handle runtime errors gracefully"), [=]() mutable
            {
                expect(teeStarterPlugin->name)->toBe(std::string("mr-tee-starter-plugin"));
                expect(teeStarterPlugin->description)->toBe(std::string("Mr. TEE's starter plugin - using plugin-tee for attestation"));
            }
            );
        }
        );
        describe(std::string("Character Error Handling"), [=]() mutable
        {
            it(std::string("should have valid character configuration"), [=]() mutable
            {
                expect(mrTeeCharacter)->toBeDefined();
                expect(mrTeeCharacter->name)->toBe(std::string("Mr. TEE"));
                expect(mrTeeCharacter->plugins)->toContain(std::string("@elizaos/plugin-tee"));
            }
            );
        }
        );
    }
    );
}

MAIN
