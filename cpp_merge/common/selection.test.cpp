#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/tests/unit/utils/selection.test.h"

void Main(void)
{
    mock->module(std:("@clack/prompts"), [=]() mutable
    {
        return (object{
            object::pair{std:("select"), mock()}, 
            object::pair{std:("cancel"), mock()}, 
            object::pair{std:("isCancel"), mock()}
        });
    }
    );
    describe(std:("selection utilities"), [=]() mutable
    {
        describe(std:("hasEmbeddingSupport"), [=]() mutable
        {
            it(std:("should return true for models with embedding support"), [=]() mutable
            {
                expect(hasEmbeddingSupport(std:("local")))->toBe(true);
                expect(hasEmbeddingSupport(std:("openai")))->toBe(true);
                expect(hasEmbeddingSupport(std:("google")))->toBe(true);
            }
            );
            it(std:("should return false for models without embedding support"), [=]() mutable
            {
                expect(hasEmbeddingSupport(std:("claude")))->toBe(false);
                expect(hasEmbeddingSupport(std:("openrouter")))->toBe(false);
            }
            );
            it(std:("should return false for unknown models"), [=]() mutable
            {
                expect(hasEmbeddingSupport(std:("unknown-model")))->toBe(false);
                expect(hasEmbeddingSupport(string_empty))->toBe(false);
            }
            );
            it(std:("should be case sensitive"), [=]() mutable
            {
                expect(hasEmbeddingSupport(std:("LOCAL")))->toBe(false);
                expect(hasEmbeddingSupport(std:("OpenAI")))->toBe(false);
                expect(hasEmbeddingSupport(std:("GOOGLE")))->toBe(false);
            }
            );
        }
        );
        describe(std:("getAvailableAIModels"), [=]() mutable
        {
            it(std:("should return all available AI models"), [=]() mutable
            {
                auto models = getAvailableAIModels();
                expect(models)->toBeInstanceOf(Array);
                expect(models->get_length())->toBeGreaterThan(0);
                auto modelValues = models->map([=](auto m) mutable
                {
                    return m->value;
                }
                );
                expect(modelValues)->toContain(std:("local"));
                expect(modelValues)->toContain(std:("claude"));
                expect(modelValues)->toContain(std:("openai"));
                expect(modelValues)->toContain(std:("google"));
                expect(modelValues)->toContain(std:("openrouter"));
            }
            );
            it(std:("should have proper structure for each model option"), [=]() mutable
            {
                auto models = getAvailableAIModels();
                models->forEach([=](auto model) mutable
                {
                    expect(model)->toHaveProperty(std:("value"));
                    expect(model)->toHaveProperty(std:("title"));
                    expect(model)->toHaveProperty(std:("description"));
                    expect(type_of(model->value))->toBe(std:("string"));
                    expect(type_of(model->title))->toBe(std:("string"));
                    expect(type_of(model->description))->toBe(std:("string"));
                }
                );
            }
            );
        }
        );
        describe(std:("AI model selection flow"), [=]() mutable
        {
            it(std:("should identify which models need separate embedding providers"), [=]() mutable
            {
                auto modelsNeedingEmbeddings = array<string>{ std:("claude"), std:("openrouter") };
                auto modelsWithEmbeddings = array<string>{ std:("local"), std:("openai"), std:("google") };
                modelsNeedingEmbeddings->forEach([=](auto model) mutable
                {
                    expect(hasEmbeddingSupport(model))->toBe(false);
                }
                );
                modelsWithEmbeddings->forEach([=](auto model) mutable
                {
                    expect(hasEmbeddingSupport(model))->toBe(true);
                }
                );
            }
            );
        }
        );
    }
    );
}

MAIN
