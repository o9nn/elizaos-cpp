#include "todoDataService.test.h"

void Main(void)
{
    describe(std:("TodoService"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<std::shared_ptr<TodoDataManager>> service;
        shared<std::shared_ptr<MockDb>> mockDb;
        shared<std::shared_ptr<MockThenable>> mockThenable;
        beforeEach([=]() mutable
        {
            mockThenable = object{
                object::pair{std:("from"), mock()}, 
                object::pair{std:("where"), mock()}, 
                object::pair{std:("orderBy"), mock()}, 
                object::pair{std:("limit"), mock()}, 
                object::pair{std:("returning"), mock()}, 
                object::pair{std:("values"), mock()}, 
                object::pair{std:("set"), mock()}, 
                object::pair{std:("then"), mock()}, 
                object::pair{std:("execute"), mock()}, 
                object::pair{std:("findFirst"), mock()}, 
                object::pair{std:("all"), mock()}, 
                object::pair{std:("$dynamic"), mock()}
            };
            mockThenable->from["mockReturnThis"]();
            mockThenable->where["mockReturnThis"]();
            mockThenable->orderBy["mockReturnThis"]();
            mockThenable->limit["mockReturnThis"]();
            mockThenable->returning["mockReturnThis"]();
            mockThenable->values["mockReturnThis"]();
            mockThenable->set["mockReturnThis"]();
            mockThenable->findFirst["mockReturnThis"]();
            mockThenable->all["mockReturnThis"]();
            mockThenable->$dynamic["mockReturnThis"]();
            mockDb = object{
                object::pair{std:("insert"), mock()->mockReturnValue(mockThenable)}, 
                object::pair{std:("select"), mock()->mockReturnValue(mockThenable)}, 
                object::pair{std:("update"), mock()->mockReturnValue(mockThenable)}, 
                object::pair{std:("delete"), mock()->mockReturnValue(mockThenable)}, 
                object::pair{std:("execute"), mock()}
            };
            mockRuntime = createMockRuntime(object{
                object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("test-agent"))}, 
                object::pair{std:("db"), mockDb}
            });
            service = createTodoDataService(mockRuntime);
        }
        );
        afterEach([=]() mutable
        {
            mock->restore();
        }
        );
        describe(std:("createTodo"), [=]() mutable
        {
            it(std:("should create a new todo with tags"), [=]() mutable
            {
                shared mockTodo = object{
                    object::pair{std:("id"), std:("00000000-0000-0000-0000-000000000001")}
                };
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(array<object>{ mockTodo });
                }
                );
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(true);
                }
                );
                auto todoId = std::async([=]() { service->createTodo(object{
                    object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("agent-1"))}, 
                    object::pair{std:("worldId"), as<std::shared_ptr<UUID>>(std:("world-1"))}, 
                    object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("room-1"))}, 
                    object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("entity-1"))}, 
                    object::pair{std:("name"), std:("Test Todo")}, 
                    object::pair{std:("description"), std:("Test Description")}, 
                    object::pair{std:("type"), std:("one-off")}, 
                    object::pair{std:("priority"), 2}, 
                    object::pair{std:("isUrgent"), true}, 
                    object::pair{std:("dueDate"), std::make_shared<Date>(std:("2024-12-31"))}, 
                    object::pair{std:("metadata"), object{
                        object::pair{std:("custom"), std:("data")}
                    }}, 
                    object::pair{std:("tags"), array<string>{ std:("TODO"), std:("urgent") }}
                }); });
                expect(mockDb->insert)->toHaveBeenCalledWith(todosTable);
                expect(mockThenable->values)->toHaveBeenCalled();
                expect(mockDb->insert)->toHaveBeenCalledWith(todoTagsTable);
                expect(todoId)->toBe(std:("00000000-0000-0000-0000-000000000001"));
            }
            );
            it(std:("should create daily todo"), [=]() mutable
            {
                shared mockTodo = object{
                    object::pair{std:("id"), std:("00000000-0000-0000-0000-000000000002")}
                };
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(array<object>{ mockTodo });
                }
                );
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(true);
                }
                );
                auto todoId = std::async([=]() { service->createTodo(object{
                    object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("agent-1"))}, 
                    object::pair{std:("worldId"), as<std::shared_ptr<UUID>>(std:("world-1"))}, 
                    object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("room-1"))}, 
                    object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("entity-1"))}, 
                    object::pair{std:("name"), std:("Daily Exercise")}, 
                    object::pair{std:("type"), std:("daily")}, 
                    object::pair{std:("tags"), array<string>{ std:("TODO"), std:("daily") }}
                }); });
                expect(mockDb->insert)->toHaveBeenCalledWith(todosTable);
                expect(todoId)->toBe(std:("00000000-0000-0000-0000-000000000002"));
            }
            );
            it(std:("should handle creation failure"), [=]() mutable
            {
                mockThenable->then["mockImplementationOnce"]([=](auto resolve, auto reject) mutable
                {
                    return reject(std::make_shared<Error>(std:("DB error")));
                }
                );
                std::async([=]() { expect(service->createTodo(object{
                    object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("agent-1"))}, 
                    object::pair{std:("worldId"), as<std::shared_ptr<UUID>>(std:("world-1"))}, 
                    object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("room-1"))}, 
                    object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("entity-1"))}, 
                    object::pair{std:("name"), std:("Test Todo")}, 
                    object::pair{std:("type"), std:("one-off")}
                }))->rejects->toThrow(std:("DB error")); });
            }
            );
        }
        );
        describe(std:("getTodos"), [=]() mutable
        {
            it(std:("should get todos with filters"), [=]() mutable
            {
                shared mockTodos = array<object>{ object{
                    object::pair{std:("id"), std:("00000000-0000-0000-0000-000000000001")}, 
                    object::pair{std:("name"), std:("Todo 1")}, 
                    object::pair{std:("type"), std:("one-off")}
                }, object{
                    object::pair{std:("id"), std:("todo-2")}, 
                    object::pair{std:("name"), std:("Todo 2")}, 
                    object::pair{std:("type"), std:("daily")}
                } };
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(mockTodos);
                }
                );
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(array<object>{ object{
                        object::pair{std:("tag"), std:("TODO")}
                    }, object{
                        object::pair{std:("tag"), std:("urgent")}
                    } });
                }
                );
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(array<object>{ object{
                        object::pair{std:("tag"), std:("TODO")}
                    }, object{
                        object::pair{std:("tag"), std:("daily")}
                    } });
                }
                );
                auto todos = std::async([=]() { service->getTodos(object{
                    object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("entity-1"))}, 
                    object::pair{std:("type"), std:("one-off")}, 
                    object::pair{std:("isCompleted"), false}
                }); });
                expect(mockThenable->where)->toHaveBeenCalled();
                expect(todos)->toHaveLength(2);
                expect(const_(todos)[0]->tags)->toEqual(array<string>{ std:("TODO"), std:("urgent") });
                expect(const_(todos)[1]->tags)->toEqual(array<string>{ std:("TODO"), std:("daily") });
            }
            );
            it(std:("should filter by tags"), [=]() mutable
            {
                shared mockTodos = array<object>{ object{
                    object::pair{std:("id"), std:("00000000-0000-0000-0000-000000000001")}, 
                    object::pair{std:("name"), std:("Todo 1")}
                } };
                shared mockTags = array<object>{ object{
                    object::pair{std:("todoId"), std:("00000000-0000-0000-0000-000000000001")}, 
                    object::pair{std:("tag"), std:("urgent")}
                } };
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(mockTodos);
                }
                );
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(mockTags);
                }
                );
                auto todos = std::async([=]() { service->getTodos(object{
                    object::pair{std:("tags"), array<string>{ std:("urgent") }}
                }); });
                expect(mockThenable->where)->toHaveBeenCalled();
                expect(todos)->toHaveLength(1);
                expect(const_(todos)[0]->id)->toBe(std:("00000000-0000-0000-0000-000000000001"));
                expect(const_(todos)[0]->tags)->toEqual(array<string>{ std:("urgent") });
            }
            );
        }
        );
        describe(std:("getTodo"), [=]() mutable
        {
            it(std:("should get a single todo by ID"), [=]() mutable
            {
                shared mockTodo = object{
                    object::pair{std:("id"), std:("00000000-0000-0000-0000-000000000001")}, 
                    object::pair{std:("name"), std:("Test Todo")}
                };
                shared mockTags = array<object>{ object{
                    object::pair{std:("todoId"), std:("00000000-0000-0000-0000-000000000001")}, 
                    object::pair{std:("tag"), std:("TODO")}
                }, object{
                    object::pair{std:("todoId"), std:("00000000-0000-0000-0000-000000000001")}, 
                    object::pair{std:("tag"), std:("urgent")}
                } };
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(array<object>{ mockTodo });
                }
                );
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(mockTags);
                }
                );
                auto todo = std::async([=]() { service->getTodo(as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000001"))); });
                expect(mockThenable->where)->toHaveBeenCalled();
                expect(todo)->not->toBeNull();
                expect(todo->id)->toBe(std:("00000000-0000-0000-0000-000000000001"));
                expect(todo->tags)->toHaveLength(2);
                expect(todo->tags)->toContain(std:("TODO"));
                expect(todo->tags)->toContain(std:("urgent"));
            }
            );
            it(std:("should return null for non-existent todo"), [=]() mutable
            {
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(array<any>());
                }
                );
                auto todo = std::async([=]() { service->getTodo(as<std::shared_ptr<UUID>>(std:("non-existent"))); });
                expect(todo)->toBeNull();
            }
            );
        }
        );
        describe(std:("updateTodo"), [=]() mutable
        {
            it(std:("should update todo fields"), [=]() mutable
            {
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(true);
                }
                );
                auto success = std::async([=]() { service->updateTodo(as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000001")), object{
                    object::pair{std:("name"), std:("Updated Name")}, 
                    object::pair{std:("priority"), 1}, 
                    object::pair{std:("isCompleted"), true}, 
                    object::pair{std:("completedAt"), std::make_shared<Date>()}
                }); });
                expect(mockThenable->set)->toHaveBeenCalled();
                expect(mockThenable->where)->toHaveBeenCalled();
                expect(success)->toBe(true);
            }
            );
            it(std:("should handle update failure"), [=]() mutable
            {
                mockThenable->then["mockImplementationOnce"]([=](auto resolve, auto reject) mutable
                {
                    return reject(std::make_shared<Error>(std:("Update failed")));
                }
                );
                std::async([=]() { expect(service->updateTodo(as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000002")), object{
                    object::pair{std:("name"), std:("Updated Name")}
                }))->rejects->toThrow(std:("Update failed")); });
            }
            );
        }
        );
        describe(std:("deleteTodo"), [=]() mutable
        {
            it(std:("should delete a todo"), [=]() mutable
            {
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(true);
                }
                );
                auto success = std::async([=]() { service->deleteTodo(as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000001"))); });
                expect(mockThenable->where)->toHaveBeenCalled();
                expect(success)->toBe(true);
            }
            );
        }
        );
        describe(std:("addTags"), [=]() mutable
        {
            it(std:("should add new tags to a todo"), [=]() mutable
            {
                shared existingTags = array<object>{ object{
                    object::pair{std:("tag"), std:("TODO")}
                } };
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(existingTags);
                }
                );
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(true);
                }
                );
                auto success = std::async([=]() { service->addTags(as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000001")), array<string>{ std:("urgent"), std:("high-priority") }); });
                expect(mockDb->select)->toHaveBeenCalled();
                expect(mockDb->insert)->toHaveBeenCalledWith(todoTagsTable);
                expect(success)->toBe(true);
            }
            );
            it(std:("should not add duplicate tags"), [=]() mutable
            {
                shared existingTags = array<object>{ object{
                    object::pair{std:("tag"), std:("TODO")}
                }, object{
                    object::pair{std:("tag"), std:("urgent")}
                } };
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(existingTags);
                }
                );
                auto success = std::async([=]() { service->addTags(as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000002")), array<string>{ std:("urgent"), std:("TODO") }); });
                expect(mockDb->select)->toHaveBeenCalled();
                expect(mockDb->insert)->not->toHaveBeenCalled();
                expect(success)->toBe(true);
            }
            );
        }
        );
        describe(std:("removeTags"), [=]() mutable
        {
            it(std:("should remove tags from a todo"), [=]() mutable
            {
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(true);
                }
                );
                auto success = std::async([=]() { service->removeTags(as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000001")), array<string>{ std:("urgent"), std:("outdated") }); });
                expect(mockDb->delete)->toHaveBeenCalledWith(todoTagsTable);
                expect(mockThenable->where)->toHaveBeenCalled();
                expect(success)->toBe(true);
            }
            );
        }
        );
        describe(std:("getOverdueTodos"), [=]() mutable
        {
            it(std:("should get overdue todos"), [=]() mutable
            {
                auto yesterday = std::make_shared<Date>();
                yesterday->setDate(yesterday->getDate() - 1);
                shared mockTodos = array<object>{ object{
                    object::pair{std:("id"), std:("00000000-0000-0000-0000-000000000001")}, 
                    object::pair{std:("name"), std:("Overdue Task")}, 
                    object::pair{std:("type"), std:("one-off")}, 
                    object::pair{std:("dueDate"), yesterday}, 
                    object::pair{std:("isCompleted"), false}
                } };
                shared mockTags = array<object>{ object{
                    object::pair{std:("todoId"), std:("00000000-0000-0000-0000-000000000001")}, 
                    object::pair{std:("tag"), std:("TODO")}
                }, object{
                    object::pair{std:("todoId"), std:("00000000-0000-0000-0000-000000000001")}, 
                    object::pair{std:("tag"), std:("urgent")}
                } };
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(mockTodos);
                }
                );
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(mockTags);
                }
                );
                auto overdueTodos = std::async([=]() { service->getOverdueTodos(); });
                expect(mockThenable->where)->toHaveBeenCalled();
                expect(overdueTodos)->toHaveLength(1);
                expect(const_(overdueTodos)[0]->name)->toBe(std:("Overdue Task"));
            }
            );
        }
        );
        describe(std:("resetDailyTodos"), [=]() mutable
        {
            it(std:("should reset completed daily todos"), [=]() mutable
            {
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(object{
                        object::pair{std:("count"), 3}
                    });
                }
                );
                shared count = std::async([=]() { service->resetDailyTodos(object{
                    object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("agent-1"))}
                }); });
                expect(count)->toBe(0);
            }
            );
            it(std:("should return 0 if no todos to reset"), [=]() mutable
            {
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(object{
                        object::pair{std:("count"), 0}
                    });
                }
                );
                shared count = std::async([=]() { service->resetDailyTodos(object{
                    object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("agent-1"))}
                }); });
                expect(count)->toBe(0);
            }
            );
        }
        );
        describe(std:("error handling"), [=]() mutable
        {
            it(std:("should handle missing database gracefully"), [=]() mutable
            {
                mockRuntime->db = undefined;
                auto service = createTodoDataService(mockRuntime);
                expect(service)->toBeDefined();
            }
            );
            it(std:("should handle database errors gracefully"), [=]() mutable
            {
                mockThenable->then["mockImplementationOnce"]([=](auto resolve, auto reject) mutable
                {
                    return reject(std::make_shared<Error>(std:("Database error")));
                }
                );
                std::async([=]() { expect(service->getTodos())->rejects->toThrow(std:("Database error")); });
            }
            );
        }
        );
    }
    );
}

MAIN
