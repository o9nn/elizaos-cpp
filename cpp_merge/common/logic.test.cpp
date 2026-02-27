#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-bootstrap/src/__tests__/logic.test.h"

void Main(void)
{
    describe(std:("Message Handler Logic"), [=]() mutable
    {
        shared<MockRuntime> mockRuntime;
        shared<Partial<std::shared_ptr<Memory>>> mockMessage;
        shared<std::shared_ptr<HandlerCallback>> mockCallback;
        beforeEach([=]() mutable
        {
            auto setup = setupActionTest(object{
                object::pair{std:("runtimeOverrides"), object{
                    object::pair{std:("useModel"), mock()->mockImplementation([=](auto modelType, auto params) mutable
                    {
                        if (params["prompt"]["includes"](std:("should respond template"))) {
                            return Promise->resolve(JSON->stringify(object{
                                object::pair{std:("action"), std:("RESPOND")}, 
                                object::pair{std:("providers"), array<string>{ std:("facts"), std:("time") }}, 
                                object::pair{std:("reasoning"), std:("Message requires a response")}
                            }));
                        } else if (modelType == ModelType->TEXT_SMALL) {
                            return Promise->resolve(JSON->stringify(object{
                                object::pair{std:("thought"), std:("I will respond to this message")}, 
                                object::pair{std:("actions"), array<string>{ std:("reply") }}, 
                                object::pair{std:("content"), std:("Hello, how can I help you today?")}
                            }));
                        } else if (modelType == ModelType->TEXT_EMBEDDING) {
                            return Promise->resolve(array<double>{ 0.1, 0.2, 0.3 });
                        }
                        return Promise->resolve(object{});
                    }
                    )}, 
                    object::pair{std:("composeState"), mock()->mockResolvedValue(object{
                        object::pair{std:("values"), object{
                            object::pair{std:("agentName"), std:("Test Agent")}, 
                            object::pair{std:("recentMessages"), std:("User: Test message")}
                        }}, 
                        object::pair{std:("data"), object{
                            object::pair{std:("room"), object{
                                object::pair{std:("id"), std:("test-room-id")}, 
                                object::pair{std:("type"), ChannelType->GROUP}
                            }}
                        }}
                    })}, 
                    object::pair{std:("getRoom"), mock()->mockResolvedValue(object{
                        object::pair{std:("id"), std:("test-room-id")}, 
                        object::pair{std:("name"), std:("Test Room")}, 
                        object::pair{std:("type"), ChannelType->GROUP}, 
                        object::pair{std:("worldId"), std:("test-world-id")}, 
                        object::pair{std:("serverId"), std:("test-server-id")}, 
                        object::pair{std:("source"), std:("test")}
                    })}, 
                    object::pair{std:("getParticipantUserState"), mock()->mockResolvedValue(std:("ACTIVE"))}
                }}, 
                object::pair{std:("messageOverrides"), object{
                    object::pair{std:("content"), as<std::shared_ptr<Content>>(object{
                        object::pair{std:("text"), std:("Hello, bot!")}, 
                        object::pair{std:("channelType"), ChannelType->GROUP}
                    })}
                }}
            });
            mockRuntime = setup["mockRuntime"];
            mockMessage = setup["mockMessage"];
            mockCallback = as<std::shared_ptr<HandlerCallback>>(setup["callbackFn"]);
            mockRuntime->character = utils::assign(object{
                , 
                object::pair{std:("templates"), utils::assign(object{
                    , 
                    object::pair{std:("messageHandlerTemplate"), std:("Test message handler template {{recentMessages}}")}, 
                    object::pair{std:("shouldRespondTemplate"), std:("Test should respond template {{recentMessages}}")}
                }, mockRuntime->character->templates)}
            }, mockRuntime->character);
        }
        );
        afterEach([=]() mutable
        {
            mock->restore();
        }
        );
        it(std:("should register all expected event handlers"), [=]() mutable
        {
            expect(bootstrapPlugin->events)->toBeDefined();
            auto requiredEvents = array<any>{ EventType->MESSAGE_RECEIVED, EventType->VOICE_MESSAGE_RECEIVED, EventType->REACTION_RECEIVED, EventType->MESSAGE_SENT, EventType->WORLD_JOINED, EventType->ENTITY_JOINED, EventType->ENTITY_LEFT };
            requiredEvents->forEach([=](auto eventType) mutable
            {
                expect(const_(bootstrapPlugin->events)[eventType])->toBeDefined();
                expect(const_(bootstrapPlugin->events)[eventType]->length)->toBeGreaterThan(0);
            }
            );
        }
        );
        it(std:("should process MESSAGE_RECEIVED event and save message to memory"), [=]() mutable
        {
            auto messageHandler = const_(const_(bootstrapPlugin->events)[EventType->MESSAGE_RECEIVED])[0];
            expect(messageHandler)->toBeDefined();
            if (messageHandler) {
                std::async([=]() { messageHandler(as<std::shared_ptr<MessagePayload>>(object{
                    object::pair{std:("runtime"), as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))}, 
                    object::pair{std:("message"), as<std::shared_ptr<Memory>>(mockMessage)}, 
                    object::pair{std:("callback"), mockCallback}, 
                    object::pair{std:("source"), std:("test")}
                })); });
                expect(mockRuntime->createMemory)->toHaveBeenCalledWith(mockMessage, std:("messages"));
                expect(mockRuntime->useModel)->toHaveBeenCalledWith(ModelType->TEXT_SMALL, expect->objectContaining(object{
                    object::pair{std:("prompt"), expect->stringContaining(std:("Test should respond template"))}
                }));
                expect(mockRuntime->composeState)->toHaveBeenCalled();
                expect(mockCallback)->toHaveBeenCalled();
            }
        }
        );
        it(std:("should not respond to messages when agent is muted"), [=]() mutable
        {
            auto messageHandler = const_(const_(bootstrapPlugin->events)[EventType->MESSAGE_RECEIVED])[0];
            expect(messageHandler)->toBeDefined();
            mockRuntime->getParticipantUserState = mock()->mockResolvedValue(std:("MUTED"));
            if (messageHandler) {
                std::async([=]() { messageHandler(as<std::shared_ptr<MessagePayload>>(object{
                    object::pair{std:("runtime"), as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))}, 
                    object::pair{std:("message"), as<std::shared_ptr<Memory>>(mockMessage)}, 
                    object::pair{std:("callback"), mockCallback}, 
                    object::pair{std:("source"), std:("test")}
                })); });
                expect(mockRuntime->createMemory)->toHaveBeenCalledWith(mockMessage, std:("messages"));
                expect(mockRuntime->useModel)->not->toHaveBeenCalledWith(ModelType->TEXT_SMALL, expect->objectContaining(object{
                    object::pair{std:("prompt"), expect->stringContaining(std:("message handler template"))}
                }));
                expect(mockCallback)->not->toHaveBeenCalled();
            }
        }
        );
        it(std:("should handle errors gracefully during message processing"), [=]() mutable
        {
            auto messageHandler = const_(const_(bootstrapPlugin->events)[EventType->MESSAGE_RECEIVED])[0];
            expect(messageHandler)->toBeDefined();
            if (!messageHandler) return std::shared_ptr<Promise<void>>();
            mockRuntime->emitEvent = mock()->mockResolvedValue(undefined);
            mockRuntime->useModel = mock()->mockImplementation([=](auto modelType, auto params) mutable
            {
                if (params["prompt"]["includes"](std:("should respond template"))) {
                    return Promise->reject(std::make_shared<Error>(std:("Test error in useModel for shouldRespond")));
                }
                if (modelType == ModelType->TEXT_SMALL) {
                    return Promise->resolve(JSON->stringify(object{
                        object::pair{std:("action"), std:("RESPOND")}, 
                        object::pair{std:("providers"), array<any>()}, 
                        object::pair{std:("reasoning"), string_empty}
                    }));
                }
                if (modelType == ModelType->TEXT_LARGE) {
                    return Promise->resolve(JSON->stringify(object{
                        object::pair{std:("thought"), std:("Default thought")}, 
                        object::pair{std:("actions"), array<string>{ std:("REPLY") }}, 
                        object::pair{std:("content"), std:("Default content")}
                    }));
                }
                return Promise->resolve(object{});
            }
            );
            try
            {
                std::async([=]() { messageHandler(as<std::shared_ptr<MessagePayload>>(object{
                    object::pair{std:("runtime"), as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))}, 
                    object::pair{std:("message"), as<std::shared_ptr<Memory>>(mockMessage)}, 
                    object::pair{std:("callback"), mockCallback}, 
                    object::pair{std:("source"), std:("test")}
                })); });
            }
            catch (const any& e)
            {
                console->log(std:("messageHandler threw an error directly in test:"), e);
            }
            expect(mockRuntime->emitEvent)->toHaveBeenCalledWith(EventType->RUN_ENDED, expect->objectContaining(object{
                object::pair{std:("status"), std:("error")}, 
                object::pair{std:("error"), std:("Test error in useModel for shouldRespond")}
            }));
        }
        );
        it(std:("should handle mal-formatted response from LLM"), [=]() mutable
        {
            auto messageHandler = const_(const_(bootstrapPlugin->events)[EventType->MESSAGE_RECEIVED])[0];
            expect(messageHandler)->toBeDefined();
            mockRuntime->useModel = mock()->mockImplementation([=](auto modelType, auto params) mutable
            {
                if (params["prompt"]["includes"](std:("should respond template"))) {
                    return Promise->resolve(std:("This is not valid JSON"));
                } else if (modelType == ModelType->TEXT_SMALL) {
                    return Promise->resolve(std:("Also not valid JSON"));
                }
                return Promise->resolve(object{});
            }
            );
            if (messageHandler) {
                any error;
                try
                {
                    std::async([=]() { messageHandler(as<std::shared_ptr<MessagePayload>>(object{
                        object::pair{std:("runtime"), as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))}, 
                        object::pair{std:("message"), as<std::shared_ptr<Memory>>(mockMessage)}, 
                        object::pair{std:("callback"), mockCallback}, 
                        object::pair{std:("source"), std:("test")}
                    })); });
                }
                catch (const any& e)
                {
                    error = as<std::shared_ptr<Error>>(e);
                }
                expect(error)->toBeUndefined();
            }
        }
        );
    }
    );
    describe(std:("Reaction Events"), [=]() mutable
    {
        shared<MockRuntime> mockRuntime;
        shared<Partial<std::shared_ptr<Memory>>> mockReaction;
        beforeEach([=]() mutable
        {
            auto setup = setupActionTest(object{
                object::pair{std:("messageOverrides"), object{
                    object::pair{std:("content"), as<std::shared_ptr<Content>>(object{
                        object::pair{std:("text"), std:("👍")}, 
                        object::pair{std:("reaction"), true}, 
                        object::pair{std:("referencedMessageId"), std:("original-message-id")}
                    })}
                }}
            });
            mockRuntime = setup["mockRuntime"];
            mockReaction = setup["mockMessage"];
        }
        );
        afterEach([=]() mutable
        {
            mock->restore();
        }
        );
        it(std:("should store reaction messages correctly"), [=]() mutable
        {
            auto reactionHandler = const_(const_(bootstrapPlugin->events)[EventType->REACTION_RECEIVED])[0];
            expect(reactionHandler)->toBeDefined();
            if (reactionHandler) {
                std::async([=]() { reactionHandler(as<std::shared_ptr<MessagePayload>>(object{
                    object::pair{std:("runtime"), as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))}, 
                    object::pair{std:("message"), as<std::shared_ptr<Memory>>(mockReaction)}, 
                    object::pair{std:("source"), std:("test")}
                })); });
                expect(mockRuntime->createMemory)->toHaveBeenCalledWith(mockReaction, std:("messages"));
            }
        }
        );
        it(std:("should handle duplicate reaction errors"), [=]() mutable
        {
            auto reactionHandler = const_(const_(bootstrapPlugin->events)[EventType->REACTION_RECEIVED])[0];
            expect(reactionHandler)->toBeDefined();
            mockRuntime->createMemory = mock()->mockRejectedValue(object{
                object::pair{std:("code"), std:("23505")}
            });
            if (reactionHandler) {
                any error;
                try
                {
                    std::async([=]() { reactionHandler(as<std::shared_ptr<MessagePayload>>(object{
                        object::pair{std:("runtime"), as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))}, 
                        object::pair{std:("message"), as<std::shared_ptr<Memory>>(mockReaction)}, 
                        object::pair{std:("source"), std:("test")}
                    })); });
                }
                catch (const any& e)
                {
                    error = as<std::shared_ptr<Error>>(e);
                }
                expect(error)->toBeUndefined();
            }
        }
        );
    }
    );
    describe(std:("World and Entity Events"), [=]() mutable
    {
        shared<MockRuntime> mockRuntime;
        beforeEach([=]() mutable
        {
            auto setup = setupActionTest(object{
                object::pair{std:("runtimeOverrides"), object{
                    object::pair{std:("ensureConnection"), mock()->mockResolvedValue(undefined)}, 
                    object::pair{std:("ensureWorldExists"), mock()->mockResolvedValue(undefined)}, 
                    object::pair{std:("ensureRoomExists"), mock()->mockResolvedValue(undefined)}, 
                    object::pair{std:("getEntityById"), mock()->mockImplementation([=](auto entityId) mutable
                    {
                        return Promise->resolve(object{
                            object::pair{std:("id"), entityId}, 
                            object::pair{std:("names"), array<string>{ std:("Test User") }}, 
                            object::pair{std:("metadata"), object{
                                object::pair{std:("status"), std:("ACTIVE")}, 
                                object::pair{std:("test"), object{
                                    object::pair{std:("username"), std:("testuser")}, 
                                    object::pair{std:("name"), std:("Test User")}, 
                                    object::pair{std:("userId"), std:("original-id-123")}
                                }}
                            }}
                        });
                    }
                    )}, 
                    object::pair{std:("updateEntity"), mock()->mockResolvedValue(undefined)}
                }}
            });
            mockRuntime = setup["mockRuntime"];
        }
        );
        afterEach([=]() mutable
        {
            mock->restore();
        }
        );
        it(std:("should handle ENTITY_JOINED events"), [=]() mutable
        {
            auto entityJoinedHandler = const_(const_(bootstrapPlugin->events)[EventType->ENTITY_JOINED])[0];
            expect(entityJoinedHandler)->toBeDefined();
            if (entityJoinedHandler) {
                std::async([=]() { entityJoinedHandler(as<std::shared_ptr<EntityPayload>>(object{
                    object::pair{std:("runtime"), as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))}, 
                    object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("test-entity-id"))}, 
                    object::pair{std:("worldId"), as<std::shared_ptr<UUID>>(std:("test-world-id"))}, 
                    object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("test-room-id"))}, 
                    object::pair{std:("metadata"), object{
                        object::pair{std:("type"), std:("user")}, 
                        object::pair{std:("orginalId"), std:("original-id-123")}, 
                        object::pair{std:("username"), std:("testuser")}, 
                        object::pair{std:("displayName"), std:("Test User")}, 
                        object::pair{std:("avatarUrl"), std:("https://example.com/avatar.png")}
                    }}, 
                    object::pair{std:("source"), std:("test")}
                })); });
                expect(mockRuntime->ensureConnection)->toHaveBeenCalled();
            }
        }
        );
        it(std:("should handle ENTITY_LEFT events"), [=]() mutable
        {
            auto entityLeftHandler = const_(const_(bootstrapPlugin->events)[EventType->ENTITY_LEFT])[0];
            expect(entityLeftHandler)->toBeDefined();
            if (entityLeftHandler) {
                std::async([=]() { entityLeftHandler(as<std::shared_ptr<EntityPayload>>(object{
                    object::pair{std:("runtime"), as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))}, 
                    object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("test-entity-id"))}, 
                    object::pair{std:("worldId"), as<std::shared_ptr<UUID>>(std:("test-world-id"))}, 
                    object::pair{std:("source"), std:("test")}
                })); });
                expect(mockRuntime->getEntityById)->toHaveBeenCalledWith(std:("test-entity-id"));
                expect(mockRuntime->updateEntity)->toHaveBeenCalledWith(expect->objectContaining(object{
                    object::pair{std:("metadata"), expect->objectContaining(object{
                        object::pair{std:("status"), std:("INACTIVE")}, 
                        object::pair{std:("leftAt"), expect->any(Number)}
                    })}
                }));
            }
        }
        );
        it(std:("should handle errors in ENTITY_LEFT events"), [=]() mutable
        {
            auto entityLeftHandler = const_(const_(bootstrapPlugin->events)[EventType->ENTITY_LEFT])[0];
            expect(entityLeftHandler)->toBeDefined();
            mockRuntime->getEntityById = mock()->mockRejectedValue(std::make_shared<Error>(std:("Entity not found")));
            if (entityLeftHandler) {
                any error;
                try
                {
                    std::async([=]() { entityLeftHandler(as<std::shared_ptr<EntityPayload>>(object{
                        object::pair{std:("runtime"), as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))}, 
                        object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("test-entity-id"))}, 
                        object::pair{std:("worldId"), as<std::shared_ptr<UUID>>(std:("test-world-id"))}, 
                        object::pair{std:("source"), std:("test")}
                    })); });
                }
                catch (const any& e)
                {
                    error = as<std::shared_ptr<Error>>(e);
                }
                expect(error)->toBeUndefined();
                expect(mockRuntime->updateEntity)->not->toHaveBeenCalled();
            }
        }
        );
    }
    );
    describe(std:("Event Lifecycle Events"), [=]() mutable
    {
        shared<MockRuntime> mockRuntime;
        beforeEach([=]() mutable
        {
            auto setup = setupActionTest();
            mockRuntime = setup["mockRuntime"];
        }
        );
        afterEach([=]() mutable
        {
            mock->restore();
        }
        );
        it(std:("should handle ACTION_STARTED events"), [=]() mutable
        {
            auto actionStartedHandler = const_(const_(bootstrapPlugin->events)[EventType->ACTION_STARTED])[0];
            expect(actionStartedHandler)->toBeDefined();
            if (actionStartedHandler) {
                std::async([=]() { actionStartedHandler(as<std::shared_ptr<ActionEventPayload>>(object{
                    object::pair{std:("runtime"), as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))}, 
                    object::pair{std:("actionId"), as<std::shared_ptr<UUID>>(std:("test-action-id"))}, 
                    object::pair{std:("actionName"), std:("test-action")}, 
                    object::pair{std:("startTime"), Date->now()}, 
                    object::pair{std:("source"), std:("test")}
                })); });
                expect(true)->toBe(true);
            }
        }
        );
        it(std:("should handle ACTION_COMPLETED events"), [=]() mutable
        {
            auto actionCompletedHandler = const_(const_(bootstrapPlugin->events)[EventType->ACTION_COMPLETED])[0];
            expect(actionCompletedHandler)->toBeDefined();
            if (actionCompletedHandler) {
                std::async([=]() { actionCompletedHandler(as<std::shared_ptr<ActionEventPayload>>(object{
                    object::pair{std:("runtime"), as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))}, 
                    object::pair{std:("actionId"), as<std::shared_ptr<UUID>>(std:("test-action-id"))}, 
                    object::pair{std:("actionName"), std:("test-action")}, 
                    object::pair{std:("completed"), true}, 
                    object::pair{std:("source"), std:("test")}
                })); });
                expect(true)->toBe(true);
            }
        }
        );
        it(std:("should handle ACTION_COMPLETED events with errors"), [=]() mutable
        {
            auto actionCompletedHandler = const_(const_(bootstrapPlugin->events)[EventType->ACTION_COMPLETED])[0];
            expect(actionCompletedHandler)->toBeDefined();
            if (actionCompletedHandler) {
                std::async([=]() { actionCompletedHandler(as<std::shared_ptr<ActionEventPayload>>(object{
                    object::pair{std:("runtime"), as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))}, 
                    object::pair{std:("actionId"), as<std::shared_ptr<UUID>>(std:("test-action-id"))}, 
                    object::pair{std:("actionName"), std:("test-action")}, 
                    object::pair{std:("completed"), false}, 
                    object::pair{std:("error"), std::make_shared<Error>(std:("Action failed"))}, 
                    object::pair{std:("source"), std:("test")}
                })); });
                expect(true)->toBe(true);
            }
        }
        );
        it(std:("should handle EVALUATOR_STARTED events"), [=]() mutable
        {
            auto evaluatorStartedHandler = const_(const_(bootstrapPlugin->events)[EventType->EVALUATOR_STARTED])[0];
            expect(evaluatorStartedHandler)->toBeDefined();
            if (evaluatorStartedHandler) {
                std::async([=]() { evaluatorStartedHandler(as<std::shared_ptr<EvaluatorEventPayload>>(object{
                    object::pair{std:("runtime"), as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))}, 
                    object::pair{std:("evaluatorId"), as<std::shared_ptr<UUID>>(std:("test-evaluator-id"))}, 
                    object::pair{std:("evaluatorName"), std:("test-evaluator")}, 
                    object::pair{std:("startTime"), Date->now()}, 
                    object::pair{std:("source"), std:("test")}
                })); });
                expect(true)->toBe(true);
            }
        }
        );
        it(std:("should handle EVALUATOR_COMPLETED events"), [=]() mutable
        {
            auto evaluatorCompletedHandler = const_(const_(bootstrapPlugin->events)[EventType->EVALUATOR_COMPLETED])[0];
            expect(evaluatorCompletedHandler)->toBeDefined();
            if (evaluatorCompletedHandler) {
                std::async([=]() { evaluatorCompletedHandler(as<std::shared_ptr<EvaluatorEventPayload>>(object{
                    object::pair{std:("runtime"), as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))}, 
                    object::pair{std:("evaluatorId"), as<std::shared_ptr<UUID>>(std:("test-evaluator-id"))}, 
                    object::pair{std:("evaluatorName"), std:("test-evaluator")}, 
                    object::pair{std:("completed"), true}, 
                    object::pair{std:("source"), std:("test")}
                })); });
                expect(true)->toBe(true);
            }
        }
        );
    }
    );
}

MAIN
