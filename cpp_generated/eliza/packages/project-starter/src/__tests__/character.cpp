#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-starter/src/__tests__/character.test.h"

void Main(void)
{
    describe(std::string("Character Configuration"), [=]() mutable
    {
        it(std::string("should have all required fields"), [=]() mutable
        {
            expect(character)->toHaveProperty(std::string("name"));
            expect(character)->toHaveProperty(std::string("bio"));
            expect(character)->toHaveProperty(std::string("plugins"));
            expect(character)->toHaveProperty(std::string("system"));
            expect(character)->toHaveProperty(std::string("messageExamples"));
        }
        );
        it(std::string("should have the correct name"), [=]() mutable
        {
            expect(character->name)->toBe(std::string("Eliza"));
        }
        );
        it(std::string("should have plugins defined as an array"), [=]() mutable
        {
            expect(Array->isArray(character->plugins))->toBe(true);
        }
        );
        it(std::string("should have conditionally included plugins based on environment variables"), [=]() mutable
        {
            auto originalOpenAIKey = process->env->OPENAI_API_KEY;
            auto originalAnthropicKey = process->env->ANTHROPIC_API_KEY;
            {
                utils::finally __finally1559_1718([&]() mutable
                {
                    process->env->OPENAI_API_KEY = originalOpenAIKey;
                    process->env->ANTHROPIC_API_KEY = originalAnthropicKey;
                });
                try
                {
                    expect(character->plugins)->toContain(std::string("@elizaos/plugin-sql"));
                    if (process->env->OPENAI_API_KEY) {
                        expect(character->plugins)->toContain(std::string("@elizaos/plugin-openai"));
                    }
                    if (process->env->ANTHROPIC_API_KEY) {
                        expect(character->plugins)->toContain(std::string("@elizaos/plugin-anthropic"));
                    }
                }
                catch (...)
                {
                    throw;
                }
            }
        }
        );
        it(std::string("should have a non-empty system prompt"), [=]() mutable
        {
            expect(character->system)->toBeTruthy();
            if (character->system) {
                expect(type_of(character->system))->toBe(std::string("string"));
                expect(character->system->length)->toBeGreaterThan(0);
            }
        }
        );
        it(std::string("should have personality traits in bio array"), [=]() mutable
        {
            expect(Array->isArray(character->bio))->toBe(true);
            if (AND((character->bio), (Array->isArray(character->bio)))) {
                expect(character->bio->length)->toBeGreaterThan(0);
                character->bio->forEach([=](auto trait) mutable
                {
                    expect(type_of(trait))->toBe(std::string("string"));
                    expect(trait["length"])->toBeGreaterThan(0);
                }
                );
            }
        }
        );
        it(std::string("should have message examples for training"), [=]() mutable
        {
            expect(Array->isArray(character->messageExamples))->toBe(true);
            if (AND((character->messageExamples), (Array->isArray(character->messageExamples)))) {
                expect(character->messageExamples->length)->toBeGreaterThan(0);
                auto firstExample = const_(character->messageExamples)[0];
                expect(Array->isArray(firstExample))->toBe(true);
                expect(firstExample->length)->toBeGreaterThan(1);
                firstExample->forEach([=](auto message) mutable
                {
                    expect(message)->toHaveProperty(std::string("name"));
                    expect(message)->toHaveProperty(std::string("content"));
                    expect(message["content"])->toHaveProperty(std::string("text"));
                }
                );
            }
        }
        );
    }
    );
}

MAIN
