#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-todo/src/tests/todoDataService.test.h"

void Main(void)
{
    describe(std::string("TodoService"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<std::shared_ptr<TodoDataManager>> service;
        shared<std::shared_ptr<MockDb>> mockDb;
        shared<std::shared_ptr<MockThenable>> mockThenable;
        beforeEach([=]() mutable
        {
            mockThenable = object{
                object::pair{std::string("from"), mock()}, 
                object::pair{std::string("where"), mock()}, 
                object::pair{std::string("orderBy"), mock()}, 
                object::pair{std::string("limit"), mock()}, 
                object::pair{std::string("returning"), mock()}, 
                object::pair{std::string("values"), mock()}, 
                object::pair{std::string("set"), mock()}, 
                object::pair{std::string("then"), mock()}, 
                object::pair{std::string("execute"), mock()}, 
                object::pair{std::string("findFirst"), mock()}, 
                object::pair{std::string("all"), mock()}, 
                object::pair{std::string("$dynamic"), mock()}
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
                object::pair{std::string("insert"), mock()->mockReturnValue(mockThenable)}, 
                object::pair{std::string("select"), mock()->mockReturnValue(mockThenable)}, 
                object::pair{std::string("update"), mock()->mockReturnValue(mockThenable)}, 
                object::pair{std::string("delete"), mock()->mockReturnValue(mockThenable)}, 
                object::pair{std::string("execute"), mock()}
            };
            mockRuntime = createMockRuntime(object{
                object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("test-agent"))}, 
                object::pair{std::string("db"), mockDb}
            });
            service = createTodoDataService(mockRuntime);
        }
        );
        afterEach([=]() mutable
        {
            mock->restore();
        }
        );
        describe(std::string("createTodo"), [=]() mutable
        {
            it(std::string("should create a new todo with tags"), [=]() mutable
            {
                shared mockTodo = object{
                    object::pair{std::string("id"), std::string("00000000-0000-0000-0000-000000000001")}
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
                    object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("agent-1"))}, 
                    object::pair{std::string("worldId"), as<std::shared_ptr<UUID>>(std::string("world-1"))}, 
                    object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("room-1"))}, 
                    object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(std::string("entity-1"))}, 
                    object::pair{std::string("name"), std::string("Test Todo")}, 
                    object::pair{std::string("description"), std::string("Test Description")}, 
                    object::pair{std::string("type"), std::string("one-off")}, 
                    object::pair{std::string("priority"), 2}, 
                    object::pair{std::string("isUrgent"), true}, 
                    object::pair{std::string("dueDate"), std::make_shared<Date>(std::string("2024-12-31"))}, 
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("custom"), std::string("data")}
                    }}, 
                    object::pair{std::string("tags"), array<string>{ std::string("TODO"), std::string("urgent") }}
                }); });
                expect(mockDb->insert)->toHaveBeenCalledWith(todosTable);
                expect(mockThenable->values)->toHaveBeenCalled();
                expect(mockDb->insert)->toHaveBeenCalledWith(todoTagsTable);
                expect(todoId)->toBe(std::string("00000000-0000-0000-0000-000000000001"));
            }
            );
            it(std::string("should create daily todo"), [=]() mutable
            {
                shared mockTodo = object{
                    object::pair{std::string("id"), std::string("00000000-0000-0000-0000-000000000002")}
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
                    object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("agent-1"))}, 
                    object::pair{std::string("worldId"), as<std::shared_ptr<UUID>>(std::string("world-1"))}, 
                    object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("room-1"))}, 
                    object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(std::string("entity-1"))}, 
                    object::pair{std::string("name"), std::string("Daily Exercise")}, 
                    object::pair{std::string("type"), std::string("daily")}, 
                    object::pair{std::string("tags"), array<string>{ std::string("TODO"), std::string("daily") }}
                }); });
                expect(mockDb->insert)->toHaveBeenCalledWith(todosTable);
                expect(todoId)->toBe(std::string("00000000-0000-0000-0000-000000000002"));
            }
            );
            it(std::string("should handle creation failure"), [=]() mutable
            {
                mockThenable->then["mockImplementationOnce"]([=](auto resolve, auto reject) mutable
                {
                    return reject(std::make_shared<Error>(std::string("DB error")));
                }
                );
                std::async([=]() { expect(service->createTodo(object{
                    object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("agent-1"))}, 
                    object::pair{std::string("worldId"), as<std::shared_ptr<UUID>>(std::string("world-1"))}, 
                    object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("room-1"))}, 
                    object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(std::string("entity-1"))}, 
                    object::pair{std::string("name"), std::string("Test Todo")}, 
                    object::pair{std::string("type"), std::string("one-off")}
                }))->rejects->toThrow(std::string("DB error")); });
            }
            );
        }
        );
        describe(std::string("getTodos"), [=]() mutable
        {
            it(std::string("should get todos with filters"), [=]() mutable
            {
                shared mockTodos = array<object>{ object{
                    object::pair{std::string("id"), std::string("00000000-0000-0000-0000-000000000001")}, 
                    object::pair{std::string("name"), std::string("Todo 1")}, 
                    object::pair{std::string("type"), std::string("one-off")}
                }, object{
                    object::pair{std::string("id"), std::string("todo-2")}, 
                    object::pair{std::string("name"), std::string("Todo 2")}, 
                    object::pair{std::string("type"), std::string("daily")}
                } };
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(mockTodos);
                }
                );
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(array<object>{ object{
                        object::pair{std::string("tag"), std::string("TODO")}
                    }, object{
                        object::pair{std::string("tag"), std::string("urgent")}
                    } });
                }
                );
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(array<object>{ object{
                        object::pair{std::string("tag"), std::string("TODO")}
                    }, object{
                        object::pair{std::string("tag"), std::string("daily")}
                    } });
                }
                );
                auto todos = std::async([=]() { service->getTodos(object{
                    object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(std::string("entity-1"))}, 
                    object::pair{std::string("type"), std::string("one-off")}, 
                    object::pair{std::string("isCompleted"), false}
                }); });
                expect(mockThenable->where)->toHaveBeenCalled();
                expect(todos)->toHaveLength(2);
                expect(const_(todos)[0]->tags)->toEqual(array<string>{ std::string("TODO"), std::string("urgent") });
                expect(const_(todos)[1]->tags)->toEqual(array<string>{ std::string("TODO"), std::string("daily") });
            }
            );
            it(std::string("should filter by tags"), [=]() mutable
            {
                shared mockTodos = array<object>{ object{
                    object::pair{std::string("id"), std::string("00000000-0000-0000-0000-000000000001")}, 
                    object::pair{std::string("name"), std::string("Todo 1")}
                } };
                shared mockTags = array<object>{ object{
                    object::pair{std::string("todoId"), std::string("00000000-0000-0000-0000-000000000001")}, 
                    object::pair{std::string("tag"), std::string("urgent")}
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
                    object::pair{std::string("tags"), array<string>{ std::string("urgent") }}
                }); });
                expect(mockThenable->where)->toHaveBeenCalled();
                expect(todos)->toHaveLength(1);
                expect(const_(todos)[0]->id)->toBe(std::string("00000000-0000-0000-0000-000000000001"));
                expect(const_(todos)[0]->tags)->toEqual(array<string>{ std::string("urgent") });
            }
            );
        }
        );
        describe(std::string("getTodo"), [=]() mutable
        {
            it(std::string("should get a single todo by ID"), [=]() mutable
            {
                shared mockTodo = object{
                    object::pair{std::string("id"), std::string("00000000-0000-0000-0000-000000000001")}, 
                    object::pair{std::string("name"), std::string("Test Todo")}
                };
                shared mockTags = array<object>{ object{
                    object::pair{std::string("todoId"), std::string("00000000-0000-0000-0000-000000000001")}, 
                    object::pair{std::string("tag"), std::string("TODO")}
                }, object{
                    object::pair{std::string("todoId"), std::string("00000000-0000-0000-0000-000000000001")}, 
                    object::pair{std::string("tag"), std::string("urgent")}
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
                auto todo = std::async([=]() { service->getTodo(as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000001"))); });
                expect(mockThenable->where)->toHaveBeenCalled();
                expect(todo)->not->toBeNull();
                expect(todo->id)->toBe(std::string("00000000-0000-0000-0000-000000000001"));
                expect(todo->tags)->toHaveLength(2);
                expect(todo->tags)->toContain(std::string("TODO"));
                expect(todo->tags)->toContain(std::string("urgent"));
            }
            );
            it(std::string("should return null for non-existent todo"), [=]() mutable
            {
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(array<any>());
                }
                );
                auto todo = std::async([=]() { service->getTodo(as<std::shared_ptr<UUID>>(std::string("non-existent"))); });
                expect(todo)->toBeNull();
            }
            );
        }
        );
        describe(std::string("updateTodo"), [=]() mutable
        {
            it(std::string("should update todo fields"), [=]() mutable
            {
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(true);
                }
                );
                auto success = std::async([=]() { service->updateTodo(as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000001")), object{
                    object::pair{std::string("name"), std::string("Updated Name")}, 
                    object::pair{std::string("priority"), 1}, 
                    object::pair{std::string("isCompleted"), true}, 
                    object::pair{std::string("completedAt"), std::make_shared<Date>()}
                }); });
                expect(mockThenable->set)->toHaveBeenCalled();
                expect(mockThenable->where)->toHaveBeenCalled();
                expect(success)->toBe(true);
            }
            );
            it(std::string("should handle update failure"), [=]() mutable
            {
                mockThenable->then["mockImplementationOnce"]([=](auto resolve, auto reject) mutable
                {
                    return reject(std::make_shared<Error>(std::string("Update failed")));
                }
                );
                std::async([=]() { expect(service->updateTodo(as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000002")), object{
                    object::pair{std::string("name"), std::string("Updated Name")}
                }))->rejects->toThrow(std::string("Update failed")); });
            }
            );
        }
        );
        describe(std::string("deleteTodo"), [=]() mutable
        {
            it(std::string("should delete a todo"), [=]() mutable
            {
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(true);
                }
                );
                auto success = std::async([=]() { service->deleteTodo(as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000001"))); });
                expect(mockThenable->where)->toHaveBeenCalled();
                expect(success)->toBe(true);
            }
            );
        }
        );
        describe(std::string("addTags"), [=]() mutable
        {
            it(std::string("should add new tags to a todo"), [=]() mutable
            {
                shared existingTags = array<object>{ object{
                    object::pair{std::string("tag"), std::string("TODO")}
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
                auto success = std::async([=]() { service->addTags(as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000001")), array<string>{ std::string("urgent"), std::string("high-priority") }); });
                expect(mockDb->select)->toHaveBeenCalled();
                expect(mockDb->insert)->toHaveBeenCalledWith(todoTagsTable);
                expect(success)->toBe(true);
            }
            );
            it(std::string("should not add duplicate tags"), [=]() mutable
            {
                shared existingTags = array<object>{ object{
                    object::pair{std::string("tag"), std::string("TODO")}
                }, object{
                    object::pair{std::string("tag"), std::string("urgent")}
                } };
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(existingTags);
                }
                );
                auto success = std::async([=]() { service->addTags(as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000002")), array<string>{ std::string("urgent"), std::string("TODO") }); });
                expect(mockDb->select)->toHaveBeenCalled();
                expect(mockDb->insert)->not->toHaveBeenCalled();
                expect(success)->toBe(true);
            }
            );
        }
        );
        describe(std::string("removeTags"), [=]() mutable
        {
            it(std::string("should remove tags from a todo"), [=]() mutable
            {
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(true);
                }
                );
                auto success = std::async([=]() { service->removeTags(as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000001")), array<string>{ std::string("urgent"), std::string("outdated") }); });
                expect(mockDb->delete)->toHaveBeenCalledWith(todoTagsTable);
                expect(mockThenable->where)->toHaveBeenCalled();
                expect(success)->toBe(true);
            }
            );
        }
        );
        describe(std::string("getOverdueTodos"), [=]() mutable
        {
            it(std::string("should get overdue todos"), [=]() mutable
            {
                auto yesterday = std::make_shared<Date>();
                yesterday->setDate(yesterday->getDate() - 1);
                shared mockTodos = array<object>{ object{
                    object::pair{std::string("id"), std::string("00000000-0000-0000-0000-000000000001")}, 
                    object::pair{std::string("name"), std::string("Overdue Task")}, 
                    object::pair{std::string("type"), std::string("one-off")}, 
                    object::pair{std::string("dueDate"), yesterday}, 
                    object::pair{std::string("isCompleted"), false}
                } };
                shared mockTags = array<object>{ object{
                    object::pair{std::string("todoId"), std::string("00000000-0000-0000-0000-000000000001")}, 
                    object::pair{std::string("tag"), std::string("TODO")}
                }, object{
                    object::pair{std::string("todoId"), std::string("00000000-0000-0000-0000-000000000001")}, 
                    object::pair{std::string("tag"), std::string("urgent")}
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
                expect(const_(overdueTodos)[0]->name)->toBe(std::string("Overdue Task"));
            }
            );
        }
        );
        describe(std::string("resetDailyTodos"), [=]() mutable
        {
            it(std::string("should reset completed daily todos"), [=]() mutable
            {
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(object{
                        object::pair{std::string("count"), 3}
                    });
                }
                );
                shared count = std::async([=]() { service->resetDailyTodos(object{
                    object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("agent-1"))}
                }); });
                expect(count)->toBe(0);
            }
            );
            it(std::string("should return 0 if no todos to reset"), [=]() mutable
            {
                mockThenable->then["mockImplementationOnce"]([=](auto resolve) mutable
                {
                    return resolve(object{
                        object::pair{std::string("count"), 0}
                    });
                }
                );
                shared count = std::async([=]() { service->resetDailyTodos(object{
                    object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("agent-1"))}
                }); });
                expect(count)->toBe(0);
            }
            );
        }
        );
        describe(std::string("error handling"), [=]() mutable
        {
            it(std::string("should handle missing database gracefully"), [=]() mutable
            {
                mockRuntime->db = undefined;
                auto service = createTodoDataService(mockRuntime);
                expect(service)->toBeDefined();
            }
            );
            it(std::string("should handle database errors gracefully"), [=]() mutable
            {
                mockThenable->then["mockImplementationOnce"]([=](auto resolve, auto reject) mutable
                {
                    return reject(std::make_shared<Error>(std::string("Database error")));
                }
                );
                std::async([=]() { expect(service->getTodos())->rejects->toThrow(std::string("Database error")); });
            }
            );
        }
        );
    }
    );
}

MAIN
