#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-goals/src/tests/schema.test.h"

void Main(void)
{
    describe(std::string("Goals Schema Tests"), [=]() mutable
    {
        describe(std::string("goalsTable"), [=]() mutable
        {
            it(std::string("should have correct columns"), [=]() mutable
            {
                auto columns = getTableColumns(goalsTable);
                expect(columns->id)->toBeDefined();
                expect(columns->agentId)->toBeDefined();
                expect(columns->ownerType)->toBeDefined();
                expect(columns->ownerId)->toBeDefined();
                expect(columns->name)->toBeDefined();
                expect(columns->description)->toBeDefined();
                expect(columns->isCompleted)->toBeDefined();
                expect(columns->completedAt)->toBeDefined();
                expect(columns->createdAt)->toBeDefined();
                expect(columns->updatedAt)->toBeDefined();
                expect(columns->metadata)->toBeDefined();
            }
            );
            it(std::string("should have correct defaults"), [=]() mutable
            {
                auto columns = getTableColumns(goalsTable);
                expect(columns->isCompleted->default)->toBe(false);
                expect(columns->metadata->default)->toEqual(object{});
            }
            );
        }
        );
        describe(std::string("goalTagsTable"), [=]() mutable
        {
            it(std::string("should have correct columns"), [=]() mutable
            {
                auto columns = getTableColumns(goalTagsTable);
                expect(columns->id)->toBeDefined();
                expect(columns->goalId)->toBeDefined();
                expect(columns->tag)->toBeDefined();
                expect(columns->createdAt)->toBeDefined();
            }
            );
            it(std::string("should have foreign key reference to goals"), [=]() mutable
            {
                auto columns = getTableColumns(goalTagsTable);
                auto goalIdColumn = columns->goalId;
                expect(goalIdColumn)->toBeDefined();
            }
            );
        }
        );
    }
    );
}

MAIN
