#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-goals/src/tests/action-handlers.test.h"

void Main(void)
{
    describe(std::string("Goal Action Handlers"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<std::shared_ptr<Memory>> mockMessage;
        shared<std::shared_ptr<State>> mockState;
        shared<std::shared_ptr<HandlerCallback>> mockCallback;
        beforeEach([=]() mutable
        {
            mock->restore();
            mockRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
                object::pair{std::string("agentId"), asUUID(std::string("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std::string("character"), object{
                    object::pair{std::string("id"), asUUID(std::string("12345678-1234-1234-1234-123456789012"))}, 
                    object::pair{std::string("name"), std::string("TestAgent")}
                }}, 
                object::pair{std::string("useModel"), mock()}, 
                object::pair{std::string("composeState"), mock()->mockResolvedValue(object{
                    object::pair{std::string("data"), object{
                        object::pair{std::string("messages"), array<any>()}, 
                        object::pair{std::string("entities"), array<any>()}
                    }}, 
                    object::pair{std::string("values"), object{}}, 
                    object::pair{std::string("text"), string_empty}
                })}, 
                object::pair{std::string("getService"), mock()->mockReturnValue(nullptr)}, 
                object::pair{std::string("getSetting"), mock()->mockReturnValue(undefined)}
            }));
            mockMessage = as<std::shared_ptr<Memory>>(object{
                object::pair{std::string("id"), asUUID(std::string("12345678-1234-1234-1234-123456789013"))}, 
                object::pair{std::string("entityId"), asUUID(std::string("12345678-1234-1234-1234-123456789014"))}, 
                object::pair{std::string("roomId"), asUUID(std::string("12345678-1234-1234-1234-123456789015"))}, 
                object::pair{std::string("agentId"), asUUID(std::string("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("test message")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            });
            mockState = as<std::shared_ptr<State>>(object{
                object::pair{std::string("data"), object{
                    object::pair{std::string("messages"), array<any>()}, 
                    object::pair{std::string("entities"), array<any>()}
                }}, 
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("text"), string_empty}
            });
            mockCallback = mock()->mockResolvedValue(array<any>());
        }
        );
        describe(std::string("createGoalAction"), [=]() mutable
        {
            it(std::string("should have correct action properties"), [=]() mutable
            {
                expect(createGoalAction->name)->toBe(std::string("CREATE_GOAL"));
                expect(createGoalAction->similes)->toContain(std::string("ADD_GOAL"));
                expect(createGoalAction->similes)->toContain(std::string("NEW_GOAL"));
                expect(createGoalAction->description)->toBeDefined();
                expect(createGoalAction->examples)->toBeDefined();
                expect(createGoalAction->examples)->toHaveLength(5);
            }
            );
            it(std::string("should validate based on service availability"), [=]() mutable
            {
                auto result = std::async([=]() { createGoalAction->validate(mockRuntime, mockMessage); });
                expect(result)->toBe(false);
            }
            );
            it(std::string("should handle missing database gracefully"), [=]() mutable
            {
                mockRuntime->useModel = mock()->mockResolvedValue(std::string("Invalid response"));
                auto result = std::async([=]() { createGoalAction->handler(mockRuntime, mockMessage, mockState, object{}, mockCallback); });
                expect(result)->toBeDefined();
                expect(type_of(result))->not->toBe(std::string("boolean"));
                if (AND((AND((AND((result), (type_of(result) == std::string("object")))), (in(std::string("values"), result)))), (result->values))) {
                    expect(result->values->success)->toBe(false);
                    expect(result->values->error)->toBe(std::string("Goal tracking is not available at the moment."));
                } else {
                    throw any(std::make_shared<Error>(std::string("Expected result to have values property")));
                }
            }
            );
        }
        );
        describe(std::string("completeGoalAction"), [=]() mutable
        {
            it(std::string("should have correct action properties"), [=]() mutable
            {
                expect(completeGoalAction->name)->toBe(std::string("COMPLETE_GOAL"));
                expect(completeGoalAction->similes)->toContain(std::string("FINISH_GOAL"));
                expect(completeGoalAction->similes)->toContain(std::string("CHECK_OFF_GOAL"));
                expect(completeGoalAction->description)->toBeDefined();
            }
            );
            it(std::string("should validate based on database availability"), [=]() mutable
            {
                auto result = std::async([=]() { completeGoalAction->validate(mockRuntime, mockMessage); });
                expect(result)->toBe(false);
                auto resultWithDb = std::async([=]() { completeGoalAction->validate(mockRuntime, mockMessage); });
                expect(resultWithDb)->toBe(false);
            }
            );
        }
        );
    }
    );
}

MAIN
