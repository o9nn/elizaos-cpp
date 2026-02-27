#include "messages.test.h"

void Main(void)
{
    describe(std:("Messages Library"), [=]() mutable
    {
        shared<array<std::shared_ptr<Entity>>> entities;
        shared<std::shared_ptr<UUID>> entityId;
        beforeAll([=]() mutable
        {
            entityId = as<std::shared_ptr<UUID>>(std:("123e4567-e89b-12d3-a456-426614174000"));
            entities = array<object>{ object{
                object::pair{std:("id"), entityId}, 
                object::pair{std:("names"), array<string>{ std:("Test User") }}, 
                object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("123e4567-e89b-12d3-a456-426614174001"))}
            } };
        }
        );
        test(std:("formatEntities should format entities into a readable string"), [=]() mutable
        {
            auto formattedEntities = formatEntities(object{
                object::pair{std:("entities"), std:("entities")}
            });
            expect(formattedEntities)->toContain(std:("Test User"));
        }
        );
        test(std:("formatMessages should format messages into a readable string"), [=]() mutable
        {
            auto messages = array<std::shared_ptr<Memory>>{ object{
                object::pair{std:("content"), as<std::shared_ptr<Content>>(object{
                    object::pair{std:("text"), std:("Hello, world!")}
                })}, 
                object::pair{std:("entityId"), entityId}, 
                object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("123e4567-e89b-12d3-a456-426614174002"))}, 
                object::pair{std:("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
                object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(string_empty)}
            } };
            auto formattedMessages = formatMessages(object{
                object::pair{std:("messages"), std:("messages")}, 
                object::pair{std:("entities"), std:("entities")}
            });
            expect(formattedMessages)->toContain(std:("Hello, world!"));
            expect(formattedMessages)->toContain(std:("Test User"));
        }
        );
        test(std:("formatTimestamp should return correct time string"), [=]() mutable
        {
            auto timestamp = ((std::make_shared<Date>()))->getTime() - 60000;
            auto result = formatTimestamp(timestamp);
            expect(result)->toBe(std:("1 minute ago"));
        }
        );
        test(std:("formatMessages should include attachments if present"), [=]() mutable
        {
            auto messages = array<std::shared_ptr<Memory>>{ object{
                object::pair{std:("content"), as<std::shared_ptr<Content>>(object{
                    object::pair{std:("text"), std:("Check this attachment")}, 
                    object::pair{std:("attachments"), array<object>{ object{
                        object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("123e4567-e89b-12d3-a456-426614174003"))}, 
                        object::pair{std:("title"), std:("Image")}, 
                        object::pair{std:("url"), std:("http://example.com/image.jpg")}
                    } }}
                })}, 
                object::pair{std:("entityId"), entityId}, 
                object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("123e4567-e89b-12d3-a456-426614174004"))}, 
                object::pair{std:("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
                object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(string_empty)}
            } };
            auto formattedMessages = formatMessages(object{
                object::pair{std:("messages"), std:("messages")}, 
                object::pair{std:("entities"), std:("entities")}
            });
            expect(formattedMessages)->toContain(std:("Check this attachment"));
            expect(formattedMessages)->toContain(std:("Attachments: ["));
        }
        );
        test(std:("formatMessages should handle empty attachments gracefully"), [=]() mutable
        {
            auto messages = array<std::shared_ptr<Memory>>{ object{
                object::pair{std:("content"), as<std::shared_ptr<Content>>(object{
                    object::pair{std:("text"), std:("No attachments here")}
                })}, 
                object::pair{std:("entityId"), entityId}, 
                object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("123e4567-e89b-12d3-a456-426614174005"))}, 
                object::pair{std:("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
                object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(string_empty)}
            } };
            auto formattedMessages = formatMessages(object{
                object::pair{std:("messages"), std:("messages")}, 
                object::pair{std:("entities"), std:("entities")}
            });
            expect(formattedMessages)->toContain(std:("No attachments here"));
            expect(formattedMessages)->not->toContain(std:("Attachments"));
        }
        );
    }
    );
    describe(std:("Messages"), [=]() mutable
    {
        shared mockEntities = array<std::shared_ptr<Entity>>{ object{
            object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("123e4567-e89b-12d3-a456-426614174006"))}, 
            object::pair{std:("names"), array<string>{ std:("Alice") }}, 
            object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("123e4567-e89b-12d3-a456-426614174001"))}
        }, object{
            object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("123e4567-e89b-12d3-a456-426614174007"))}, 
            object::pair{std:("names"), array<string>{ std:("Bob") }}, 
            object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("123e4567-e89b-12d3-a456-426614174002"))}
        } };
        describe(std:("formatEntities"), [=]() mutable
        {
            it(std:("should format entities with complete details"), [=]() mutable
            {
                auto formatted = formatEntities(object{
                    object::pair{std:("entities"), mockEntities}
                });
                expect(formatted)->toContain(std:(""Alice"\
ID:"));
                expect(formatted)->toContain(std:(""Bob"\
ID:"));
            }
            );
            it(std:("should handle entities without details"), [=]() mutable
            {
                auto actorsWithoutDetails = array<std::shared_ptr<Entity>>{ object{
                    object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("123e4567-e89b-12d3-a456-426614174013"))}, 
                    object::pair{std:("names"), array<string>{ std:("Charlie") }}, 
                    object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("123e4567-e89b-12d3-a456-426614174003"))}
                } };
                auto formatted = formatEntities(object{
                    object::pair{std:("entities"), actorsWithoutDetails}
                });
                expect(formatted)->toContain(std:(""Charlie"\
ID:"));
            }
            );
            it(std:("should handle empty entities array"), [=]() mutable
            {
                auto formatted = formatEntities(object{
                    object::pair{std:("entities"), array<any>()}
                });
                expect(formatted)->toBe(string_empty);
            }
            );
        }
        );
        describe(std:("formatMessages"), [=]() mutable
        {
            it(std:("should handle messages from unknown users"), [=]() mutable
            {
                auto messagesWithUnknownUser = array<std::shared_ptr<Memory>>{ object{
                    object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("123e4567-e89b-12d3-a456-426614174014"))}, 
                    object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("123e4567-e89b-12d3-a456-426614174009"))}, 
                    object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("123e4567-e89b-12d3-a456-426614174015"))}, 
                    object::pair{std:("createdAt"), Date->now()}, 
                    object::pair{std:("content"), as<std::shared_ptr<Content>>(object{
                        object::pair{std:("text"), std:("Test message")}
                    })}, 
                    object::pair{std:("agentId"), std:("123e4567-e89b-12d3-a456-426614174001")}
                } };
                auto formatted = formatMessages(object{
                    object::pair{std:("messages"), messagesWithUnknownUser}, 
                    object::pair{std:("entities"), mockEntities}
                });
                expect(formatted)->toContain(std:("Unknown User: Test message"));
            }
            );
            it(std:("should handle messages with no action"), [=]() mutable
            {
                auto messagesWithoutAction = array<std::shared_ptr<Memory>>{ object{
                    object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("123e4567-e89b-12d3-a456-426614174016"))}, 
                    object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("123e4567-e89b-12d3-a456-426614174009"))}, 
                    object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(const_(mockEntities)[0]->id)}, 
                    object::pair{std:("createdAt"), Date->now()}, 
                    object::pair{std:("content"), as<std::shared_ptr<Content>>(object{
                        object::pair{std:("text"), std:("Simple message")}
                    })}, 
                    object::pair{std:("agentId"), std:("123e4567-e89b-12d3-a456-426614174001")}
                } };
                auto formatted = formatMessages(object{
                    object::pair{std:("messages"), messagesWithoutAction}, 
                    object::pair{std:("entities"), mockEntities}
                });
                expect(formatted)->not->toContain(std:("()"));
                expect(formatted)->toContain(std:("Simple message"));
            }
            );
            it(std:("should handle empty messages array"), [=]() mutable
            {
                auto formatted = formatMessages(object{
                    object::pair{std:("messages"), array<any>()}, 
                    object::pair{std:("entities"), mockEntities}
                });
                expect(formatted)->toBe(string_empty);
            }
            );
        }
        );
        describe(std:("formatTimestamp"), [=]() mutable
        {
            it(std:("should handle exact time boundaries"), [=]() mutable
            {
                auto now = Date->now();
                expect(formatTimestamp(now))->toContain(std:("just now"));
            }
            );
        }
        );
    }
    );
}

MAIN
