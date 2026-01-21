#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-bootstrap/src/__tests__/logic.test.h"

void Main(void)
{
    describe(std::string("Message Handler Logic"), [=]() mutable
    {
        shared<MockRuntime> mockRuntime;
        shared<Partial<std::shared_ptr<Memory>>> mockMessage;
        shared<std::shared_ptr<HandlerCallback>> mockCallback;
        beforeEach([=]() mutable
        {
            auto setup = setupActionTest(object{
                object::pair{std::string("runtimeOverrides"), object{
                    object::pair{std::string("useModel"), mock()->mockImplementation([=](auto modelType, auto params) mutable
                    {
                        if (params["prompt"]["includes"](std::string("should respond template"))) {
                            return Promise->resolve(JSON->stringify(object{
                                object::pair{std::string("action"), std::string("RESPOND")}, 
                                object::pair{std::string("providers"), array<string>{ std::string("facts"), std::string("time") }}, 
                                object::pair{std::string("reasoning"), std::string("Message requires a response")}
                            }));
                        } else if (modelType == ModelType->TEXT_SMALL) {
                            return Promise->resolve(JSON->stringify(object{
                                object::pair{std::string("thought"), std::string("I will respond to this message")}, 
                                object::pair{std::string("actions"), array<string>{ std::string("reply") }}, 
                                object::pair{std::string("content"), std::string("Hello, how can I help you today?")}
                            }));
                        } else if (modelType == ModelType->TEXT_EMBEDDING) {
                            return Promise->resolve(array<double>{ 0.1, 0.2, 0.3 });
                        }
                        return Promise->resolve(object{});
                    }
                    )}, 
                    object::pair{std::string("composeState"), mock()->mockResolvedValue(object{
                        object::pair{std::string("values"), object{
                            object::pair{std::string("agentName"), std::string("Test Agent")}, 
                            object::pair{std::string("recentMessages"), std::string("User: Test message")}
                        }}, 
                        object::pair{std::string("data"), object{
                            object::pair{std::string("room"), object{
                                object::pair{std::string("id"), std::string("test-room-id")}, 
                                object::pair{std::string("type"), ChannelType->GROUP}
                            }}
                        }}
                    })}, 
                    object::pair{std::string("getRoom"), mock()->mockResolvedValue(object{
                        object::pair{std::string("id"), std::string("test-room-id")}, 
                        object::pair{std::string("name"), std::string("Test Room")}, 
                        object::pair{std::string("type"), ChannelType->GROUP}, 
                        object::pair{std::string("worldId"), std::string("test-world-id")}, 
                        object::pair{std::string("serverId"), std::string("test-server-id")}, 
                        object::pair{std::string("source"), std::string("test")}
                    })}, 
                    object::pair{std::string("getParticipantUserState"), mock()->mockResolvedValue(std::string("ACTIVE"))}
                }}, 
                object::pair{std::string("messageOverrides"), object{
                    object::pair{std::string("content"), as<std::shared_ptr<Content>>(object{
                        object::pair{std::string("text"), std::string("Hello, bot!")}, 
                        object::pair{std::string("channelType"), ChannelType->GROUP}
                    })}
                }}
            });
            mockRuntime = setup["mockRuntime"];
            mockMessage = setup["mockMessage"];
            mockCallback = as<std::shared_ptr<HandlerCallback>>(setup["callbackFn"]);
            mockRuntime->character = utils::assign(object{
                , 
                object::pair{std::string("templates"), utils::assign(object{
                    , 
                    object::pair{std::string("messageHandlerTemplate"), std::string("Test message handler template {{recentMessages}}")}, 
                    object::pair{std::string("shouldRespondTemplate"), std::string("Test should respond template {{recentMessages}}")}
                }, mockRuntime->character->templates)}
            }, mockRuntime->character);
        }
        );
        afterEach([=]() mutable
        {
            mock->restore();
        }
        );
        it(std::string("should register all expected event handlers"), [=]() mutable
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
        it(std::string("should process MESSAGE_RECEIVED event and save message to memory"), [=]() mutable
        {
            auto messageHandler = const_(const_(bootstrapPlugin->events)[EventType->MESSAGE_RECEIVED])[0];
            expect(messageHandler)->toBeDefined();
            if (messageHandler) {
                std::async([=]() { messageHandler(as<std::shared_ptr<MessagePayload>>(object{
                    object::pair{std::string("runtime"), as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))}, 
                    object::pair{std::string("message"), as<std::shared_ptr<Memory>>(mockMessage)}, 
                    object::pair{std::string("callback"), mockCallback}, 
                    object::pair{std::string("source"), std::string("test")}
                })); });
                expect(mockRuntime->createMemory)->toHaveBeenCalledWith(mockMessage, std::string("messages"));
                expect(mockRuntime->useModel)->toHaveBeenCalledWith(ModelType->TEXT_SMALL, expect->objectContaining(object{
                    object::pair{std::string("prompt"), expect->stringContaining(std::string("Test should respond template"))}
                }));
                expect(mockRuntime->composeState)->toHaveBeenCalled();
                expect(mockCallback)->toHaveBeenCalled();
            }
        }
        );
        it(std::string("should not respond to messages when agent is muted"), [=]() mutable
        {
            auto messageHandler = const_(const_(bootstrapPlugin->events)[EventType->MESSAGE_RECEIVED])[0];
            expect(messageHandler)->toBeDefined();
            mockRuntime->getParticipantUserState = mock()->mockResolvedValue(std::string("MUTED"));
            if (messageHandler) {
                std::async([=]() { messageHandler(as<std::shared_ptr<MessagePayload>>(object{
                    object::pair{std::string("runtime"), as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))}, 
                    object::pair{std::string("message"), as<std::shared_ptr<Memory>>(mockMessage)}, 
                    object::pair{std::string("callback"), mockCallback}, 
                    object::pair{std::string("source"), std::string("test")}
                })); });
                expect(mockRuntime->createMemory)->toHaveBeenCalledWith(mockMessage, std::string("messages"));
                expect(mockRuntime->useModel)->not->toHaveBeenCalledWith(ModelType->TEXT_SMALL, expect->objectContaining(object{
                    object::pair{std::string("prompt"), expect->stringContaining(std::string("message handler template"))}
                }));
                expect(mockCallback)->not->toHaveBeenCalled();
            }
        }
        );
        it(std::string("should handle errors gracefully during message processing"), [=]() mutable
        {
            auto messageHandler = const_(const_(bootstrapPlugin->events)[EventType->MESSAGE_RECEIVED])[0];
            expect(messageHandler)->toBeDefined();
            if (!messageHandler) return std::shared_ptr<Promise<void>>();
            mockRuntime->emitEvent = mock()->mockResolvedValue(undefined);
            mockRuntime->useModel = mock()->mockImplementation([=](auto modelType, auto params) mutable
            {
                if (params["prompt"]["includes"](std::string("should respond template"))) {
                    return Promise->reject(std::make_shared<Error>(std::string("Test error in useModel for shouldRespond")));
                }
                if (modelType == ModelType->TEXT_SMALL) {
                    return Promise->resolve(JSON->stringify(object{
                        object::pair{std::string("action"), std::string("RESPOND")}, 
                        object::pair{std::string("providers"), array<any>()}, 
                        object::pair{std::string("reasoning"), string_empty}
                    }));
                }
                if (modelType == ModelType->TEXT_LARGE) {
                    return Promise->resolve(JSON->stringify(object{
                        object::pair{std::string("thought"), std::string("Default thought")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("REPLY") }}, 
                        object::pair{std::string("content"), std::string("Default content")}
                    }));
                }
                return Promise->resolve(object{});
            }
            );
            try
            {
                std::async([=]() { messageHandler(as<std::shared_ptr<MessagePayload>>(object{
                    object::pair{std::string("runtime"), as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))}, 
                    object::pair{std::string("message"), as<std::shared_ptr<Memory>>(mockMessage)}, 
                    object::pair{std::string("callback"), mockCallback}, 
                    object::pair{std::string("source"), std::string("test")}
                })); });
            }
            catch (const any& e)
            {
                console->log(std::string("messageHandler threw an error directly in test:"), e);
            }
            expect(mockRuntime->emitEvent)->toHaveBeenCalledWith(EventType->RUN_ENDED, expect->objectContaining(object{
                object::pair{std::string("status"), std::string("error")}, 
                object::pair{std::string("error"), std::string("Test error in useModel for shouldRespond")}
            }));
        }
        );
        it(std::string("should handle mal-formatted response from LLM"), [=]() mutable
        {
            auto messageHandler = const_(const_(bootstrapPlugin->events)[EventType->MESSAGE_RECEIVED])[0];
            expect(messageHandler)->toBeDefined();
            mockRuntime->useModel = mock()->mockImplementation([=](auto modelType, auto params) mutable
            {
                if (params["prompt"]["includes"](std::string("should respond template"))) {
                    return Promise->resolve(std::string("This is not valid JSON"));
                } else if (modelType == ModelType->TEXT_SMALL) {
                    return Promise->resolve(std::string("Also not valid JSON"));
                }
                return Promise->resolve(object{});
            }
            );
            if (messageHandler) {
                any error;
                try
                {
                    std::async([=]() { messageHandler(as<std::shared_ptr<MessagePayload>>(object{
                        object::pair{std::string("runtime"), as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))}, 
                        object::pair{std::string("message"), as<std::shared_ptr<Memory>>(mockMessage)}, 
                        object::pair{std::string("callback"), mockCallback}, 
                        object::pair{std::string("source"), std::string("test")}
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
    describe(std::string("Reaction Events"), [=]() mutable
    {
        shared<MockRuntime> mockRuntime;
        shared<Partial<std::shared_ptr<Memory>>> mockReaction;
        beforeEach([=]() mutable
        {
            auto setup = setupActionTest(object{
                object::pair{std::string("messageOverrides"), object{
                    object::pair{std::string("content"), as<std::shared_ptr<Content>>(object{
                        object::pair{std::string("text"), std::string("👍")}, 
                        object::pair{std::string("reaction"), true}, 
                        object::pair{std::string("referencedMessageId"), std::string("original-message-id")}
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
        it(std::string("should store reaction messages correctly"), [=]() mutable
        {
            auto reactionHandler = const_(const_(bootstrapPlugin->events)[EventType->REACTION_RECEIVED])[0];
            expect(reactionHandler)->toBeDefined();
            if (reactionHandler) {
                std::async([=]() { reactionHandler(as<std::shared_ptr<MessagePayload>>(object{
                    object::pair{std::string("runtime"), as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))}, 
                    object::pair{std::string("message"), as<std::shared_ptr<Memory>>(mockReaction)}, 
                    object::pair{std::string("source"), std::string("test")}
                })); });
                expect(mockRuntime->createMemory)->toHaveBeenCalledWith(mockReaction, std::string("messages"));
            }
        }
        );
        it(std::string("should handle duplicate reaction errors"), [=]() mutable
        {
            auto reactionHandler = const_(const_(bootstrapPlugin->events)[EventType->REACTION_RECEIVED])[0];
            expect(reactionHandler)->toBeDefined();
            mockRuntime->createMemory = mock()->mockRejectedValue(object{
                object::pair{std::string("code"), std::string("23505")}
            });
            if (reactionHandler) {
                any error;
                try
                {
                    std::async([=]() { reactionHandler(as<std::shared_ptr<MessagePayload>>(object{
                        object::pair{std::string("runtime"), as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))}, 
                        object::pair{std::string("message"), as<std::shared_ptr<Memory>>(mockReaction)}, 
                        object::pair{std::string("source"), std::string("test")}
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
    describe(std::string("World and Entity Events"), [=]() mutable
    {
        shared<MockRuntime> mockRuntime;
        beforeEach([=]() mutable
        {
            auto setup = setupActionTest(object{
                object::pair{std::string("runtimeOverrides"), object{
                    object::pair{std::string("ensureConnection"), mock()->mockResolvedValue(undefined)}, 
                    object::pair{std::string("ensureWorldExists"), mock()->mockResolvedValue(undefined)}, 
                    object::pair{std::string("ensureRoomExists"), mock()->mockResolvedValue(undefined)}, 
                    object::pair{std::string("getEntityById"), mock()->mockImplementation([=](auto entityId) mutable
                    {
                        return Promise->resolve(object{
                            object::pair{std::string("id"), entityId}, 
                            object::pair{std::string("names"), array<string>{ std::string("Test User") }}, 
                            object::pair{std::string("metadata"), object{
                                object::pair{std::string("status"), std::string("ACTIVE")}, 
                                object::pair{std::string("test"), object{
                                    object::pair{std::string("username"), std::string("testuser")}, 
                                    object::pair{std::string("name"), std::string("Test User")}, 
                                    object::pair{std::string("userId"), std::string("original-id-123")}
                                }}
                            }}
                        });
                    }
                    )}, 
                    object::pair{std::string("updateEntity"), mock()->mockResolvedValue(undefined)}
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
        it(std::string("should handle ENTITY_JOINED events"), [=]() mutable
        {
            auto entityJoinedHandler = const_(const_(bootstrapPlugin->events)[EventType->ENTITY_JOINED])[0];
            expect(entityJoinedHandler)->toBeDefined();
            if (entityJoinedHandler) {
                std::async([=]() { entityJoinedHandler(as<std::shared_ptr<EntityPayload>>(object{
                    object::pair{std::string("runtime"), as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))}, 
                    object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(std::string("test-entity-id"))}, 
                    object::pair{std::string("worldId"), as<std::shared_ptr<UUID>>(std::string("test-world-id"))}, 
                    object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("test-room-id"))}, 
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("type"), std::string("user")}, 
                        object::pair{std::string("orginalId"), std::string("original-id-123")}, 
                        object::pair{std::string("username"), std::string("testuser")}, 
                        object::pair{std::string("displayName"), std::string("Test User")}, 
                        object::pair{std::string("avatarUrl"), std::string("https://example.com/avatar.png")}
                    }}, 
                    object::pair{std::string("source"), std::string("test")}
                })); });
                expect(mockRuntime->ensureConnection)->toHaveBeenCalled();
            }
        }
        );
        it(std::string("should handle ENTITY_LEFT events"), [=]() mutable
        {
            auto entityLeftHandler = const_(const_(bootstrapPlugin->events)[EventType->ENTITY_LEFT])[0];
            expect(entityLeftHandler)->toBeDefined();
            if (entityLeftHandler) {
                std::async([=]() { entityLeftHandler(as<std::shared_ptr<EntityPayload>>(object{
                    object::pair{std::string("runtime"), as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))}, 
                    object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(std::string("test-entity-id"))}, 
                    object::pair{std::string("worldId"), as<std::shared_ptr<UUID>>(std::string("test-world-id"))}, 
                    object::pair{std::string("source"), std::string("test")}
                })); });
                expect(mockRuntime->getEntityById)->toHaveBeenCalledWith(std::string("test-entity-id"));
                expect(mockRuntime->updateEntity)->toHaveBeenCalledWith(expect->objectContaining(object{
                    object::pair{std::string("metadata"), expect->objectContaining(object{
                        object::pair{std::string("status"), std::string("INACTIVE")}, 
                        object::pair{std::string("leftAt"), expect->any(Number)}
                    })}
                }));
            }
        }
        );
        it(std::string("should handle errors in ENTITY_LEFT events"), [=]() mutable
        {
            auto entityLeftHandler = const_(const_(bootstrapPlugin->events)[EventType->ENTITY_LEFT])[0];
            expect(entityLeftHandler)->toBeDefined();
            mockRuntime->getEntityById = mock()->mockRejectedValue(std::make_shared<Error>(std::string("Entity not found")));
            if (entityLeftHandler) {
                any error;
                try
                {
                    std::async([=]() { entityLeftHandler(as<std::shared_ptr<EntityPayload>>(object{
                        object::pair{std::string("runtime"), as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))}, 
                        object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(std::string("test-entity-id"))}, 
                        object::pair{std::string("worldId"), as<std::shared_ptr<UUID>>(std::string("test-world-id"))}, 
                        object::pair{std::string("source"), std::string("test")}
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
    describe(std::string("Event Lifecycle Events"), [=]() mutable
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
        it(std::string("should handle ACTION_STARTED events"), [=]() mutable
        {
            auto actionStartedHandler = const_(const_(bootstrapPlugin->events)[EventType->ACTION_STARTED])[0];
            expect(actionStartedHandler)->toBeDefined();
            if (actionStartedHandler) {
                std::async([=]() { actionStartedHandler(as<std::shared_ptr<ActionEventPayload>>(object{
                    object::pair{std::string("runtime"), as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))}, 
                    object::pair{std::string("actionId"), as<std::shared_ptr<UUID>>(std::string("test-action-id"))}, 
                    object::pair{std::string("actionName"), std::string("test-action")}, 
                    object::pair{std::string("startTime"), Date->now()}, 
                    object::pair{std::string("source"), std::string("test")}
                })); });
                expect(true)->toBe(true);
            }
        }
        );
        it(std::string("should handle ACTION_COMPLETED events"), [=]() mutable
        {
            auto actionCompletedHandler = const_(const_(bootstrapPlugin->events)[EventType->ACTION_COMPLETED])[0];
            expect(actionCompletedHandler)->toBeDefined();
            if (actionCompletedHandler) {
                std::async([=]() { actionCompletedHandler(as<std::shared_ptr<ActionEventPayload>>(object{
                    object::pair{std::string("runtime"), as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))}, 
                    object::pair{std::string("actionId"), as<std::shared_ptr<UUID>>(std::string("test-action-id"))}, 
                    object::pair{std::string("actionName"), std::string("test-action")}, 
                    object::pair{std::string("completed"), true}, 
                    object::pair{std::string("source"), std::string("test")}
                })); });
                expect(true)->toBe(true);
            }
        }
        );
        it(std::string("should handle ACTION_COMPLETED events with errors"), [=]() mutable
        {
            auto actionCompletedHandler = const_(const_(bootstrapPlugin->events)[EventType->ACTION_COMPLETED])[0];
            expect(actionCompletedHandler)->toBeDefined();
            if (actionCompletedHandler) {
                std::async([=]() { actionCompletedHandler(as<std::shared_ptr<ActionEventPayload>>(object{
                    object::pair{std::string("runtime"), as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))}, 
                    object::pair{std::string("actionId"), as<std::shared_ptr<UUID>>(std::string("test-action-id"))}, 
                    object::pair{std::string("actionName"), std::string("test-action")}, 
                    object::pair{std::string("completed"), false}, 
                    object::pair{std::string("error"), std::make_shared<Error>(std::string("Action failed"))}, 
                    object::pair{std::string("source"), std::string("test")}
                })); });
                expect(true)->toBe(true);
            }
        }
        );
        it(std::string("should handle EVALUATOR_STARTED events"), [=]() mutable
        {
            auto evaluatorStartedHandler = const_(const_(bootstrapPlugin->events)[EventType->EVALUATOR_STARTED])[0];
            expect(evaluatorStartedHandler)->toBeDefined();
            if (evaluatorStartedHandler) {
                std::async([=]() { evaluatorStartedHandler(as<std::shared_ptr<EvaluatorEventPayload>>(object{
                    object::pair{std::string("runtime"), as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))}, 
                    object::pair{std::string("evaluatorId"), as<std::shared_ptr<UUID>>(std::string("test-evaluator-id"))}, 
                    object::pair{std::string("evaluatorName"), std::string("test-evaluator")}, 
                    object::pair{std::string("startTime"), Date->now()}, 
                    object::pair{std::string("source"), std::string("test")}
                })); });
                expect(true)->toBe(true);
            }
        }
        );
        it(std::string("should handle EVALUATOR_COMPLETED events"), [=]() mutable
        {
            auto evaluatorCompletedHandler = const_(const_(bootstrapPlugin->events)[EventType->EVALUATOR_COMPLETED])[0];
            expect(evaluatorCompletedHandler)->toBeDefined();
            if (evaluatorCompletedHandler) {
                std::async([=]() { evaluatorCompletedHandler(as<std::shared_ptr<EvaluatorEventPayload>>(object{
                    object::pair{std::string("runtime"), as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))}, 
                    object::pair{std::string("evaluatorId"), as<std::shared_ptr<UUID>>(std::string("test-evaluator-id"))}, 
                    object::pair{std::string("evaluatorName"), std::string("test-evaluator")}, 
                    object::pair{std::string("completed"), true}, 
                    object::pair{std::string("source"), std::string("test")}
                })); });
                expect(true)->toBe(true);
            }
        }
        );
    }
    );
}

MAIN
