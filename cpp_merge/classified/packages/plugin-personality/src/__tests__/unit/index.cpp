#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-personality/src/__tests__/unit/index.test.h"

void Main(void)
{
    describe(std:("Self Modification Plugin"), [=]() mutable
    {
        it(std:("should a valid plugin object"), [=]() mutable
        {
            expect(selfModificationPlugin)->toBeDefined();
            expect(selfModificationPlugin->name)->toBe(std:("@elizaos/plugin-personality"));
            expect(selfModificationPlugin->description)->toBeDefined();
        }
        );
        it(std:("should have actions array"), [=]() mutable
        {
            expect(selfModificationPlugin->actions)->toBeDefined();
            expect(Array->isArray(selfModificationPlugin->actions))->toBe(true);
            expect(selfModificationPlugin->actions->length)->toBeGreaterThan(0);
        }
        );
        it(std:("should have evaluators array"), [=]() mutable
        {
            expect(selfModificationPlugin->evaluators)->toBeDefined();
            expect(Array->isArray(selfModificationPlugin->evaluators))->toBe(true);
            expect(selfModificationPlugin->evaluators->length)->toBeGreaterThan(0);
        }
        );
        it(std:("should have services array"), [=]() mutable
        {
            expect(selfModificationPlugin->services)->toBeDefined();
            expect(Array->isArray(selfModificationPlugin->services))->toBe(true);
            expect(selfModificationPlugin->services->length)->toBeGreaterThan(0);
        }
        );
        it(std:("should contain MODIFY_CHARACTER action"), [=]() mutable
        {
            auto modifyAction = selfModificationPlugin->actions->find([=](auto action) mutable
            {
                return action["name"] == std:("MODIFY_CHARACTER");
            }
            );
            expect(modifyAction)->toBeDefined();
            expect(modifyAction->description)->toBeDefined();
            expect(modifyAction->validate)->toBeDefined();
            expect(modifyAction->handler)->toBeDefined();
        }
        );
        it(std:("should contain CHARACTER_EVOLUTION evaluator"), [=]() mutable
        {
            auto evaluator = selfModificationPlugin->evaluators->find([=](auto evaluatorItem) mutable
            {
                return evaluatorItem["name"] == std:("CHARACTER_EVOLUTION");
            }
            );
            expect(evaluator)->toBeDefined();
            expect(evaluator->description)->toBeDefined();
            expect(evaluator->validate)->toBeDefined();
            expect(evaluator->handler)->toBeDefined();
        }
        );
        it(std:("should contain CharacterFileManager service"), [=]() mutable
        {
            auto service = selfModificationPlugin->services->find([=](auto svc) mutable
            {
                return svc == CharacterFileManager;
            }
            );
            expect(service)->toBeDefined();
        }
        );
        it(std:("should have proper plugin metadata"), [=]() mutable
        {
            expect(selfModificationPlugin->name)->toBe(std:("@elizaos/plugin-personality"));
            expect(selfModificationPlugin->description)->toContain(std:("self-modification"));
        }
        );
    }
    );
}

MAIN
