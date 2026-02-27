#include "character.test.h"

void Main(void)
{
    describe(std:("Character Configuration"), [=]() mutable
    {
        it(std:("should have all required fields"), [=]() mutable
        {
            expect(character)->toHaveProperty(std:("name"));
            expect(character)->toHaveProperty(std:("bio"));
            expect(character)->toHaveProperty(std:("plugins"));
            expect(character)->toHaveProperty(std:("system"));
            expect(character)->toHaveProperty(std:("messageExamples"));
        }
        );
        it(std:("should have the correct name"), [=]() mutable
        {
            expect(character->name)->toBe(std:("Eliza"));
        }
        );
        it(std:("should have plugins defined as an array"), [=]() mutable
        {
            expect(Array->isArray(character->plugins))->toBe(true);
        }
        );
        it(std:("should have conditionally included plugins based on environment variables"), [=]() mutable
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
                    expect(character->plugins)->toContain(std:("@elizaos/plugin-sql"));
                    if (process->env->OPENAI_API_KEY) {
                        expect(character->plugins)->toContain(std:("@elizaos/plugin-openai"));
                    }
                    if (process->env->ANTHROPIC_API_KEY) {
                        expect(character->plugins)->toContain(std:("@elizaos/plugin-anthropic"));
                    }
                }
                catch (...)
                {
                    throw;
                }
            }
        }
        );
        it(std:("should have a non-empty system prompt"), [=]() mutable
        {
            expect(character->system)->toBeTruthy();
            if (character->system) {
                expect(type_of(character->system))->toBe(std:("string"));
                expect(character->system->length)->toBeGreaterThan(0);
            }
        }
        );
        it(std:("should have personality traits in bio array"), [=]() mutable
        {
            expect(Array->isArray(character->bio))->toBe(true);
            if (AND((character->bio), (Array->isArray(character->bio)))) {
                expect(character->bio->length)->toBeGreaterThan(0);
                character->bio->forEach([=](auto trait) mutable
                {
                    expect(type_of(trait))->toBe(std:("string"));
                    expect(trait["length"])->toBeGreaterThan(0);
                }
                );
            }
        }
        );
        it(std:("should have message examples for training"), [=]() mutable
        {
            expect(Array->isArray(character->messageExamples))->toBe(true);
            if (AND((character->messageExamples), (Array->isArray(character->messageExamples)))) {
                expect(character->messageExamples->length)->toBeGreaterThan(0);
                auto firstExample = const_(character->messageExamples)[0];
                expect(Array->isArray(firstExample))->toBe(true);
                expect(firstExample->length)->toBeGreaterThan(1);
                firstExample->forEach([=](auto message) mutable
                {
                    expect(message)->toHaveProperty(std:("name"));
                    expect(message)->toHaveProperty(std:("content"));
                    expect(message["content"])->toHaveProperty(std:("text"));
                }
                );
            }
        }
        );
    }
    );
}

MAIN
