#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-personality/src/__tests__/unit/character-file-manager.test.h"

void Main(void)
{
    describe(std:("CharacterFileManager"), [=]() mutable
    {
        shared<std::shared_ptr<CharacterFileManager>> fileManager;
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        beforeEach([=]() mutable
        {
            mockRuntime = as<any>(object{
                object::pair{std:("getSetting"), [=]() mutable
                {
                    return nullptr;
                }
                }, 
                object::pair{std:("character"), object{
                    object::pair{std:("bio"), array<string>{ std:("Original bio") }}, 
                    object::pair{std:("topics"), array<string>{ std:("topic1"), std:("topic2") }}, 
                    object::pair{std:("name"), std:("TestAgent")}
                }}, 
                object::pair{std:("agentId"), std:("test-agent-id")}
            });
            fileManager = std::make_shared<CharacterFileManager>(mockRuntime);
        }
        );
        it(std:("should validate valid modifications"), [=]() mutable
        {
            auto modification = object{
                object::pair{std:("bio"), array<string>{ std:("New bio line") }}, 
                object::pair{std:("topics"), array<string>{ std:("new topic") }}
            };
            auto result = fileManager->validateModification(modification);
            expect(result["valid"])->toBe(true);
            expect(result["errors"])->toHaveLength(0);
        }
        );
        it(std:("should reject modifications with XSS attempts"), [=]() mutable
        {
            auto modification = object{
                object::pair{std:("bio"), array<string>{ std:("<script>alert("xss")</script>") }}, 
                object::pair{std:("topics"), array<string>{ std:("javascript(0)") }}
            };
            auto result = fileManager->validateModification(modification);
            expect(result["valid"])->toBe(false);
            expect(result["errors"])->toContain(std:("Invalid bio: failed validation rules"));
        }
        );
        it(std:("should reject modifications exceeding limits"), [=]() mutable
        {
            auto modification = object{
                object::pair{std:("bio"), ((array(21)))->fill(std:("Too many bio elements"))}, 
                object::pair{std:("topics"), ((array(51)))->fill(std:("Too many topics"))}
            };
            auto result = fileManager->validateModification(modification);
            expect(result["valid"])->toBe(false);
            expect(result["errors"])->toContain(std:("Too many bio elements - maximum 20 allowed"));
            expect(result["errors"])->toContain(std:("Too many topics - maximum 50 allowed"));
        }
        );
        it(std:("should reject empty string values"), [=]() mutable
        {
            auto modification = object{
                object::pair{std:("bio"), array<string>{ string_empty, std:("Valid bio") }}, 
                object::pair{std:("topics"), array<string>{ std:("valid topic"), string_empty }}
            };
            auto result = fileManager->validateModification(modification);
            expect(result["valid"])->toBe(false);
            expect(result["errors"])->toContain(std:("Invalid bio: failed validation rules"));
        }
        );
        it(std:("should accept edge case of maximum allowed elements"), [=]() mutable
        {
            auto modification = object{
                object::pair{std:("bio"), ((array(20)))->fill(std:("Valid bio element"))}, 
                object::pair{std:("topics"), ((array(50)))->fill(std:("validtopic"))}
            };
            auto result = fileManager->validateModification(modification);
            expect(result["valid"])->toBe(true);
            expect(result["errors"])->toHaveLength(0);
        }
        );
        it(std:("should validate system prompt modifications"), [=]() mutable
        {
            auto validSystem = object{
                object::pair{std:("system"), std:("You are a helpful assistant that provides accurate information.")}
            };
            auto invalidSystem = object{
                object::pair{std:("system"), std:("<script>alert("xss")</script>")}
            };
            auto shortSystem = object{
                object::pair{std:("system"), std:("Too short")}
            };
            expect(fileManager->validateModification(validSystem)["valid"])->toBe(true);
            expect(fileManager->validateModification(invalidSystem)["valid"])->toBe(false);
            expect(fileManager->validateModification(shortSystem)["valid"])->toBe(false);
        }
        );
    }
    );
    describe(std:("CharacterFileManager static methods"), [=]() mutable
    {
        it(std:("should have correct service type"), [=]() mutable
        {
            expect(CharacterFileManager::serviceType)->toBe(std:("CHARACTER_MANAGEMENT"));
        }
        );
    }
    );
}

MAIN
