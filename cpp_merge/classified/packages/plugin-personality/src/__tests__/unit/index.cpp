#include "index.test.h"
#include <string>

void Main(void)
{
    describe(std::string("Self Modification Plugin"), [=]() mutable
    {
        it(std::string("should a valid plugin object"), [=]() mutable
        {
            expect(selfModificationPlugin)->toBeDefined();
            expect(selfModificationPlugin->name)->toBe(std::string("@elizaos/plugin-personality"));
            expect(selfModificationPlugin->description)->toBeDefined();
        }
        );
        it(std::string("should have actions array"), [=]() mutable
        {
            expect(selfModificationPlugin->actions)->toBeDefined();
            expect(Array->isArray(selfModificationPlugin->actions))->toBe(true);
            expect(selfModificationPlugin->actions->length)->toBeGreaterThan(0);
        }
        );
        it(std::string("should have evaluators array"), [=]() mutable
        {
            expect(selfModificationPlugin->evaluators)->toBeDefined();
            expect(Array->isArray(selfModificationPlugin->evaluators))->toBe(true);
            expect(selfModificationPlugin->evaluators->length)->toBeGreaterThan(0);
        }
        );
        it(std::string("should have services array"), [=]() mutable
        {
            expect(selfModificationPlugin->services)->toBeDefined();
            expect(Array->isArray(selfModificationPlugin->services))->toBe(true);
            expect(selfModificationPlugin->services->length)->toBeGreaterThan(0);
        }
        );
        it(std::string("should contain MODIFY_CHARACTER action"), [=]() mutable
        {
            auto modifyAction = selfModificationPlugin->actions->find([=](auto action) mutable
            {
                return action["name"] == std::string("MODIFY_CHARACTER");
            }
            );
            expect(modifyAction)->toBeDefined();
            expect(modifyAction->description)->toBeDefined();
            expect(modifyAction->validate)->toBeDefined();
            expect(modifyAction->handler)->toBeDefined();
        }
        );
        it(std::string("should contain CHARACTER_EVOLUTION evaluator"), [=]() mutable
        {
            auto evaluator = selfModificationPlugin->evaluators->find([=](auto evaluatorItem) mutable
            {
                return evaluatorItem["name"] == std::string("CHARACTER_EVOLUTION");
            }
            );
            expect(evaluator)->toBeDefined();
            expect(evaluator->description)->toBeDefined();
            expect(evaluator->validate)->toBeDefined();
            expect(evaluator->handler)->toBeDefined();
        }
        );
        it(std::string("should contain CharacterFileManager service"), [=]() mutable
        {
            auto service = selfModificationPlugin->services->find([=](auto svc) mutable
            {
                return svc == CharacterFileManager;
            }
            );
            expect(service)->toBeDefined();
        }
        );
        it(std::string("should have proper plugin metadata"), [=]() mutable
        {
            expect(selfModificationPlugin->name)->toBe(std::string("@elizaos/plugin-personality"));
            expect(selfModificationPlugin->description)->toContain(std::string("self-modification"));
        }
        );
    }
    );
}

MAIN
