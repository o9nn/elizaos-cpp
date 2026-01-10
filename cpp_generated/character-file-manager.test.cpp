#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-personality/src/__tests__/unit/character-file-manager.test.h"

void Main(void)
{
    describe(std::string("CharacterFileManager"), [=]() mutable
    {
        shared<std::shared_ptr<CharacterFileManager>> fileManager;
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        beforeEach([=]() mutable
        {
            mockRuntime = as<any>(object{
                object::pair{std::string("getSetting"), [=]() mutable
                {
                    return nullptr;
                }
                }, 
                object::pair{std::string("character"), object{
                    object::pair{std::string("bio"), array<string>{ std::string("Original bio") }}, 
                    object::pair{std::string("topics"), array<string>{ std::string("topic1"), std::string("topic2") }}, 
                    object::pair{std::string("name"), std::string("TestAgent")}
                }}, 
                object::pair{std::string("agentId"), std::string("test-agent-id")}
            });
            fileManager = std::make_shared<CharacterFileManager>(mockRuntime);
        }
        );
        it(std::string("should validate valid modifications"), [=]() mutable
        {
            auto modification = object{
                object::pair{std::string("bio"), array<string>{ std::string("New bio line") }}, 
                object::pair{std::string("topics"), array<string>{ std::string("new topic") }}
            };
            auto result = fileManager->validateModification(modification);
            expect(result["valid"])->toBe(true);
            expect(result["errors"])->toHaveLength(0);
        }
        );
        it(std::string("should reject modifications with XSS attempts"), [=]() mutable
        {
            auto modification = object{
                object::pair{std::string("bio"), array<string>{ std::string("<script>alert("xss")</script>") }}, 
                object::pair{std::string("topics"), array<string>{ std::string("javascript:void(0)") }}
            };
            auto result = fileManager->validateModification(modification);
            expect(result["valid"])->toBe(false);
            expect(result["errors"])->toContain(std::string("Invalid bio: failed validation rules"));
        }
        );
        it(std::string("should reject modifications exceeding limits"), [=]() mutable
        {
            auto modification = object{
                object::pair{std::string("bio"), ((array(21)))->fill(std::string("Too many bio elements"))}, 
                object::pair{std::string("topics"), ((array(51)))->fill(std::string("Too many topics"))}
            };
            auto result = fileManager->validateModification(modification);
            expect(result["valid"])->toBe(false);
            expect(result["errors"])->toContain(std::string("Too many bio elements - maximum 20 allowed"));
            expect(result["errors"])->toContain(std::string("Too many topics - maximum 50 allowed"));
        }
        );
        it(std::string("should reject empty string values"), [=]() mutable
        {
            auto modification = object{
                object::pair{std::string("bio"), array<string>{ string_empty, std::string("Valid bio") }}, 
                object::pair{std::string("topics"), array<string>{ std::string("valid topic"), string_empty }}
            };
            auto result = fileManager->validateModification(modification);
            expect(result["valid"])->toBe(false);
            expect(result["errors"])->toContain(std::string("Invalid bio: failed validation rules"));
        }
        );
        it(std::string("should accept edge case of maximum allowed elements"), [=]() mutable
        {
            auto modification = object{
                object::pair{std::string("bio"), ((array(20)))->fill(std::string("Valid bio element"))}, 
                object::pair{std::string("topics"), ((array(50)))->fill(std::string("validtopic"))}
            };
            auto result = fileManager->validateModification(modification);
            expect(result["valid"])->toBe(true);
            expect(result["errors"])->toHaveLength(0);
        }
        );
        it(std::string("should validate system prompt modifications"), [=]() mutable
        {
            auto validSystem = object{
                object::pair{std::string("system"), std::string("You are a helpful assistant that provides accurate information.")}
            };
            auto invalidSystem = object{
                object::pair{std::string("system"), std::string("<script>alert("xss")</script>")}
            };
            auto shortSystem = object{
                object::pair{std::string("system"), std::string("Too short")}
            };
            expect(fileManager->validateModification(validSystem)["valid"])->toBe(true);
            expect(fileManager->validateModification(invalidSystem)["valid"])->toBe(false);
            expect(fileManager->validateModification(shortSystem)["valid"])->toBe(false);
        }
        );
    }
    );
    describe(std::string("CharacterFileManager static methods"), [=]() mutable
    {
        it(std::string("should have correct service type"), [=]() mutable
        {
            expect(CharacterFileManager::serviceType)->toBe(std::string("CHARACTER_MANAGEMENT"));
        }
        );
    }
    );
}

MAIN
