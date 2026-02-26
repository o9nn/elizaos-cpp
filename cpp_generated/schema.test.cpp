#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-todo/src/tests/schema.test.h"

void Main(void)
{
    describe(std::string("Todo Schema"), [=]() mutable
    {
        describe(std::string("todosTable"), [=]() mutable
        {
            it(std::string("should have all required columns"), [=]() mutable
            {
                auto columns = getTableColumns(todosTable);
                expect(columns->id)->toBeDefined();
                expect(columns->agentId)->toBeDefined();
                expect(columns->worldId)->toBeDefined();
                expect(columns->roomId)->toBeDefined();
                expect(columns->entityId)->toBeDefined();
                expect(columns->name)->toBeDefined();
                expect(columns->description)->toBeDefined();
                expect(columns->type)->toBeDefined();
                expect(columns->priority)->toBeDefined();
                expect(columns->isUrgent)->toBeDefined();
                expect(columns->isCompleted)->toBeDefined();
                expect(columns->dueDate)->toBeDefined();
                expect(columns->completedAt)->toBeDefined();
                expect(columns->createdAt)->toBeDefined();
                expect(columns->updatedAt)->toBeDefined();
                expect(columns->metadata)->toBeDefined();
            }
            );
            it(std::string("should have proper column types"), [=]() mutable
            {
                auto columns = getTableColumns(todosTable);
                expect(columns->id->dataType)->toBe(std::string("string"));
                expect(columns->agentId->dataType)->toBe(std::string("string"));
                expect(columns->worldId->dataType)->toBe(std::string("string"));
                expect(columns->roomId->dataType)->toBe(std::string("string"));
                expect(columns->entityId->dataType)->toBe(std::string("string"));
                expect(columns->name->dataType)->toBe(std::string("string"));
                expect(columns->type->dataType)->toBe(std::string("string"));
                expect(columns->isUrgent->dataType)->toBe(std::string("boolean"));
                expect(columns->isCompleted->dataType)->toBe(std::string("boolean"));
                expect(columns->createdAt->dataType)->toBe(std::string("date"));
                expect(columns->updatedAt->dataType)->toBe(std::string("date"));
            }
            );
        }
        );
        describe(std::string("todoTagsTable"), [=]() mutable
        {
            it(std::string("should have all required columns"), [=]() mutable
            {
                auto columns = getTableColumns(todoTagsTable);
                expect(columns->id)->toBeDefined();
                expect(columns->todoId)->toBeDefined();
                expect(columns->tag)->toBeDefined();
                expect(columns->createdAt)->toBeDefined();
            }
            );
            it(std::string("should have proper column types"), [=]() mutable
            {
                auto columns = getTableColumns(todoTagsTable);
                expect(columns->id->dataType)->toBe(std::string("string"));
                expect(columns->todoId->dataType)->toBe(std::string("string"));
                expect(columns->tag->dataType)->toBe(std::string("string"));
                expect(columns->createdAt->dataType)->toBe(std::string("date"));
            }
            );
        }
        );
        describe(std::string("todoSchemaExport export"), [=]() mutable
        {
            it(std::string("should export correct schema structure"), [=]() mutable
            {
                expect(todoSchemaExport)->toBeDefined();
                expect(todoSchemaExport["todosTable"])->toBeDefined();
                expect(todoSchemaExport["todoTagsTable"])->toBeDefined();
                expect(todoSchemaExport["tables"])->toBeDefined();
                expect(todoSchemaExport["tables"]["todos"])->toBeDefined();
                expect(todoSchemaExport["tables"]["todoTags"])->toBeDefined();
            }
            );
        }
        );
    }
    );
}

MAIN
