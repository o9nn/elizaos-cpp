#include "action-handlers.test.h"

void Main(void)
{
    describe(std:("Goal Action Handlers"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<std::shared_ptr<Memory>> mockMessage;
        shared<std::shared_ptr<State>> mockState;
        shared<std::shared_ptr<HandlerCallback>> mockCallback;
        beforeEach([=]() mutable
        {
            mock->restore();
            mockRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
                object::pair{std:("agentId"), asUUID(std:("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std:("character"), object{
                    object::pair{std:("id"), asUUID(std:("12345678-1234-1234-1234-123456789012"))}, 
                    object::pair{std:("name"), std:("TestAgent")}
                }}, 
                object::pair{std:("useModel"), mock()}, 
                object::pair{std:("composeState"), mock()->mockResolvedValue(object{
                    object::pair{std:("data"), object{
                        object::pair{std:("messages"), array<any>()}, 
                        object::pair{std:("entities"), array<any>()}
                    }}, 
                    object::pair{std:("values"), object{}}, 
                    object::pair{std:("text"), string_empty}
                })}, 
                object::pair{std:("getService"), mock()->mockReturnValue(nullptr)}, 
                object::pair{std:("getSetting"), mock()->mockReturnValue(undefined)}
            }));
            mockMessage = as<std::shared_ptr<Memory>>(object{
                object::pair{std:("id"), asUUID(std:("12345678-1234-1234-1234-123456789013"))}, 
                object::pair{std:("entityId"), asUUID(std:("12345678-1234-1234-1234-123456789014"))}, 
                object::pair{std:("roomId"), asUUID(std:("12345678-1234-1234-1234-123456789015"))}, 
                object::pair{std:("agentId"), asUUID(std:("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("test message")}, 
                    object::pair{std:("source"), std:("test")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            });
            mockState = as<std::shared_ptr<State>>(object{
                object::pair{std:("data"), object{
                    object::pair{std:("messages"), array<any>()}, 
                    object::pair{std:("entities"), array<any>()}
                }}, 
                object::pair{std:("values"), object{}}, 
                object::pair{std:("text"), string_empty}
            });
            mockCallback = mock()->mockResolvedValue(array<any>());
        }
        );
        describe(std:("createGoalAction"), [=]() mutable
        {
            it(std:("should have correct action properties"), [=]() mutable
            {
                expect(createGoalAction->name)->toBe(std:("CREATE_GOAL"));
                expect(createGoalAction->similes)->toContain(std:("ADD_GOAL"));
                expect(createGoalAction->similes)->toContain(std:("NEW_GOAL"));
                expect(createGoalAction->description)->toBeDefined();
                expect(createGoalAction->examples)->toBeDefined();
                expect(createGoalAction->examples)->toHaveLength(5);
            }
            );
            it(std:("should validate based on service availability"), [=]() mutable
            {
                auto result = std::async([=]() { createGoalAction->validate(mockRuntime, mockMessage); });
                expect(result)->toBe(false);
            }
            );
            it(std:("should handle missing database gracefully"), [=]() mutable
            {
                mockRuntime->useModel = mock()->mockResolvedValue(std:("Invalid response"));
                auto result = std::async([=]() { createGoalAction->handler(mockRuntime, mockMessage, mockState, object{}, mockCallback); });
                expect(result)->toBeDefined();
                expect(type_of(result))->not->toBe(std:("boolean"));
                if (AND((AND((AND((result), (type_of(result) == std:("object")))), (in(std:("values"), result)))), (result->values))) {
                    expect(result->values->success)->toBe(false);
                    expect(result->values->error)->toBe(std:("Goal tracking is not available at the moment."));
                } else {
                    throw any(std::make_shared<Error>(std:("Expected result to have values property")));
                }
            }
            );
        }
        );
        describe(std:("completeGoalAction"), [=]() mutable
        {
            it(std:("should have correct action properties"), [=]() mutable
            {
                expect(completeGoalAction->name)->toBe(std:("COMPLETE_GOAL"));
                expect(completeGoalAction->similes)->toContain(std:("FINISH_GOAL"));
                expect(completeGoalAction->similes)->toContain(std:("CHECK_OFF_GOAL"));
                expect(completeGoalAction->description)->toBeDefined();
            }
            );
            it(std:("should validate based on database availability"), [=]() mutable
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
