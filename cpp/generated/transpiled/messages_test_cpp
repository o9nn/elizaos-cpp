#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v2/__tests__/messages.test.h"

void Main(void)
{
    describe(std::string("Messages Library"), [=]() mutable
    {
        shared<any> runtime;
        shared<array<std::shared_ptr<Entity>>> entities;
        shared<std::shared_ptr<UUID>> entityId;
        beforeAll([=]() mutable
        {
            runtime = as<any>(as<any>(object{
                object::pair{std::string("getParticipantsForRoom"), vi->fn()}, 
                object::pair{std::string("getEntityById"), vi->fn()}, 
                object::pair{std::string("getRoom"), vi->fn()}
            }));
            entityId = as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174000"));
            entities = array<object>{ object{
                object::pair{std::string("id"), entityId}, 
                object::pair{std::string("names"), array<string>{ std::string("Test User") }}, 
                object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174001"))}
            } };
        }
        );
        test(std::string("formatEntities should format entities into a readable string"), [=]() mutable
        {
            auto formattedEntities = formatEntities(object{
                object::pair{std::string("entities"), std::string("entities")}
            });
            expect(formattedEntities)->toContain(std::string("Test User"));
        }
        );
        test(std::string("formatMessages should format messages into a readable string"), [=]() mutable
        {
            auto messages = array<std::shared_ptr<Memory>>{ object{
                object::pair{std::string("content"), as<std::shared_ptr<Content>>(object{
                    object::pair{std::string("text"), std::string("Hello, world!")}
                })}, 
                object::pair{std::string("entityId"), entityId}, 
                object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174002"))}, 
                object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
                object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(string_empty)}
            } };
            auto formattedMessages = formatMessages(object{
                object::pair{std::string("messages"), std::string("messages")}, 
                object::pair{std::string("entities"), std::string("entities")}
            });
            expect(formattedMessages)->toContain(std::string("Hello, world!"));
            expect(formattedMessages)->toContain(std::string("Test User"));
        }
        );
        test(std::string("formatTimestamp should return correct time string"), [=]() mutable
        {
            auto timestamp = ((std::make_shared<Date>()))->getTime() - 60000;
            auto result = formatTimestamp(timestamp);
            expect(result)->toBe(std::string("1 minute ago"));
        }
        );
        test(std::string("formatMessages should include attachments if present"), [=]() mutable
        {
            auto messages = array<std::shared_ptr<Memory>>{ object{
                object::pair{std::string("content"), as<std::shared_ptr<Content>>(object{
                    object::pair{std::string("text"), std::string("Check this attachment")}, 
                    object::pair{std::string("attachments"), array<object>{ object{
                        object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174003"))}, 
                        object::pair{std::string("title"), std::string("Image")}, 
                        object::pair{std::string("url"), std::string("http://example.com/image.jpg")}
                    } }}
                })}, 
                object::pair{std::string("entityId"), entityId}, 
                object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174004"))}, 
                object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
                object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(string_empty)}
            } };
            auto formattedMessages = formatMessages(object{
                object::pair{std::string("messages"), std::string("messages")}, 
                object::pair{std::string("entities"), std::string("entities")}
            });
            expect(formattedMessages)->toContain(std::string("Check this attachment"));
            expect(formattedMessages)->toContain(std::string("Attachments: ["));
        }
        );
        test(std::string("formatMessages should handle empty attachments gracefully"), [=]() mutable
        {
            auto messages = array<std::shared_ptr<Memory>>{ object{
                object::pair{std::string("content"), as<std::shared_ptr<Content>>(object{
                    object::pair{std::string("text"), std::string("No attachments here")}
                })}, 
                object::pair{std::string("entityId"), entityId}, 
                object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174005"))}, 
                object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
                object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(string_empty)}
            } };
            auto formattedMessages = formatMessages(object{
                object::pair{std::string("messages"), std::string("messages")}, 
                object::pair{std::string("entities"), std::string("entities")}
            });
            expect(formattedMessages)->toContain(std::string("No attachments here"));
            expect(formattedMessages)->not->toContain(std::string("Attachments"));
        }
        );
    }
    );
    describe(std::string("Messages"), [=]() mutable
    {
        shared mockEntities = array<std::shared_ptr<Entity>>{ object{
            object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174006"))}, 
            object::pair{std::string("names"), array<string>{ std::string("Alice") }}, 
            object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174001"))}
        }, object{
            object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174007"))}, 
            object::pair{std::string("names"), array<string>{ std::string("Bob") }}, 
            object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174002"))}
        } };
        auto mockMessages = array<std::shared_ptr<Memory>>{ object{
            object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174008"))}, 
            object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174009"))}, 
            object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(const_(mockEntities)[0]->id)}, 
            object::pair{std::string("createdAt"), Date->now() - 5000}, 
            object::pair{std::string("content"), as<std::shared_ptr<Content>>(object{
                object::pair{std::string("text"), std::string("Hello everyone!")}, 
                object::pair{std::string("action"), std::string("wave")}
            })}, 
            object::pair{std::string("agentId"), std::string("123e4567-e89b-12d3-a456-426614174001")}
        }, object{
            object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174010"))}, 
            object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174009"))}, 
            object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(const_(mockEntities)[1]->id)}, 
            object::pair{std::string("createdAt"), Date->now() - 60000}, 
            object::pair{std::string("content"), as<std::shared_ptr<Content>>(object{
                object::pair{std::string("text"), std::string("Hi Alice!")}, 
                object::pair{std::string("attachments"), array<object>{ object{
                    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174011"))}, 
                    object::pair{std::string("title"), std::string("Document")}, 
                    object::pair{std::string("url"), std::string("https://example.com/doc.pdf")}
                } }}
            })}, 
            object::pair{std::string("agentId"), std::string("123e4567-e89b-12d3-a456-426614174001")}
        } };
        describe(std::string("formatEntities"), [=]() mutable
        {
            it(std::string("should format entities with complete details"), [=]() mutable
            {
                auto formatted = formatEntities(object{
                    object::pair{std::string("entities"), mockEntities}
                });
                expect(formatted)->toContain(std::string(""Alice"\
ID:"));
                expect(formatted)->toContain(std::string(""Bob"\
ID:"));
            }
            );
            it(std::string("should handle entities without details"), [=]() mutable
            {
                auto actorsWithoutDetails = array<std::shared_ptr<Entity>>{ object{
                    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174013"))}, 
                    object::pair{std::string("names"), array<string>{ std::string("Charlie") }}, 
                    object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174003"))}
                } };
                auto formatted = formatEntities(object{
                    object::pair{std::string("entities"), actorsWithoutDetails}
                });
                expect(formatted)->toContain(std::string(""Charlie"\
ID:"));
            }
            );
            it(std::string("should handle empty entities array"), [=]() mutable
            {
                auto formatted = formatEntities(object{
                    object::pair{std::string("entities"), array<any>()}
                });
                expect(formatted)->toBe(string_empty);
            }
            );
        }
        );
        describe(std::string("formatMessages"), [=]() mutable
        {
            it(std::string("should handle messages from unknown users"), [=]() mutable
            {
                auto messagesWithUnknownUser = array<std::shared_ptr<Memory>>{ object{
                    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174014"))}, 
                    object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174009"))}, 
                    object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174015"))}, 
                    object::pair{std::string("createdAt"), Date->now()}, 
                    object::pair{std::string("content"), as<std::shared_ptr<Content>>(object{
                        object::pair{std::string("text"), std::string("Test message")}
                    })}, 
                    object::pair{std::string("agentId"), std::string("123e4567-e89b-12d3-a456-426614174001")}
                } };
                auto formatted = formatMessages(object{
                    object::pair{std::string("messages"), messagesWithUnknownUser}, 
                    object::pair{std::string("entities"), mockEntities}
                });
                expect(formatted)->toContain(std::string("Unknown User: Test message"));
            }
            );
            it(std::string("should handle messages with no action"), [=]() mutable
            {
                auto messagesWithoutAction = array<std::shared_ptr<Memory>>{ object{
                    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174016"))}, 
                    object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174009"))}, 
                    object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(const_(mockEntities)[0]->id)}, 
                    object::pair{std::string("createdAt"), Date->now()}, 
                    object::pair{std::string("content"), as<std::shared_ptr<Content>>(object{
                        object::pair{std::string("text"), std::string("Simple message")}
                    })}, 
                    object::pair{std::string("agentId"), std::string("123e4567-e89b-12d3-a456-426614174001")}
                } };
                auto formatted = formatMessages(object{
                    object::pair{std::string("messages"), messagesWithoutAction}, 
                    object::pair{std::string("entities"), mockEntities}
                });
                expect(formatted)->not->toContain(std::string("()"));
                expect(formatted)->toContain(std::string("Simple message"));
            }
            );
            it(std::string("should handle empty messages array"), [=]() mutable
            {
                auto formatted = formatMessages(object{
                    object::pair{std::string("messages"), array<any>()}, 
                    object::pair{std::string("entities"), mockEntities}
                });
                expect(formatted)->toBe(string_empty);
            }
            );
        }
        );
        describe(std::string("formatTimestamp"), [=]() mutable
        {
            it(std::string("should handle exact time boundaries"), [=]() mutable
            {
                auto now = Date->now();
                expect(formatTimestamp(now))->toContain(std::string("just now"));
            }
            );
        }
        );
    }
    );
}

MAIN
