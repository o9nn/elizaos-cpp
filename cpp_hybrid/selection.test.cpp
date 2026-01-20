#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/tests/unit/utils/selection.test.h"

void Main(void)
{
    mock->module(std::string("@clack/prompts"), [=]() mutable
    {
        return (object{
            object::pair{std::string("select"), mock()}, 
            object::pair{std::string("cancel"), mock()}, 
            object::pair{std::string("isCancel"), mock()}
        });
    }
    );
    describe(std::string("selection utilities"), [=]() mutable
    {
        describe(std::string("hasEmbeddingSupport"), [=]() mutable
        {
            it(std::string("should return true for models with embedding support"), [=]() mutable
            {
                expect(hasEmbeddingSupport(std::string("local")))->toBe(true);
                expect(hasEmbeddingSupport(std::string("openai")))->toBe(true);
                expect(hasEmbeddingSupport(std::string("google")))->toBe(true);
            }
            );
            it(std::string("should return false for models without embedding support"), [=]() mutable
            {
                expect(hasEmbeddingSupport(std::string("claude")))->toBe(false);
                expect(hasEmbeddingSupport(std::string("openrouter")))->toBe(false);
            }
            );
            it(std::string("should return false for unknown models"), [=]() mutable
            {
                expect(hasEmbeddingSupport(std::string("unknown-model")))->toBe(false);
                expect(hasEmbeddingSupport(string_empty))->toBe(false);
            }
            );
            it(std::string("should be case sensitive"), [=]() mutable
            {
                expect(hasEmbeddingSupport(std::string("LOCAL")))->toBe(false);
                expect(hasEmbeddingSupport(std::string("OpenAI")))->toBe(false);
                expect(hasEmbeddingSupport(std::string("GOOGLE")))->toBe(false);
            }
            );
        }
        );
        describe(std::string("getAvailableAIModels"), [=]() mutable
        {
            it(std::string("should return all available AI models"), [=]() mutable
            {
                auto models = getAvailableAIModels();
                expect(models)->toBeInstanceOf(Array);
                expect(models->get_length())->toBeGreaterThan(0);
                auto modelValues = models->map([=](auto m) mutable
                {
                    return m->value;
                }
                );
                expect(modelValues)->toContain(std::string("local"));
                expect(modelValues)->toContain(std::string("claude"));
                expect(modelValues)->toContain(std::string("openai"));
                expect(modelValues)->toContain(std::string("google"));
                expect(modelValues)->toContain(std::string("openrouter"));
            }
            );
            it(std::string("should have proper structure for each model option"), [=]() mutable
            {
                auto models = getAvailableAIModels();
                models->forEach([=](auto model) mutable
                {
                    expect(model)->toHaveProperty(std::string("value"));
                    expect(model)->toHaveProperty(std::string("title"));
                    expect(model)->toHaveProperty(std::string("description"));
                    expect(type_of(model->value))->toBe(std::string("string"));
                    expect(type_of(model->title))->toBe(std::string("string"));
                    expect(type_of(model->description))->toBe(std::string("string"));
                }
                );
            }
            );
        }
        );
        describe(std::string("AI model selection flow"), [=]() mutable
        {
            it(std::string("should identify which models need separate embedding providers"), [=]() mutable
            {
                auto modelsNeedingEmbeddings = array<string>{ std::string("claude"), std::string("openrouter") };
                auto modelsWithEmbeddings = array<string>{ std::string("local"), std::string("openai"), std::string("google") };
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
