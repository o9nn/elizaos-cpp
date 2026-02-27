#include "models.test.h"

std::function<std::shared_ptr<Promise<object>>(any, std::function<std::shared_ptr<Promise<string>>(any, std::shared_ptr<TestGenerateParams>)>)> runCoreModelTests = [=](auto modelType, auto modelFn) mutable
{
    auto mockRuntime = createMockRuntime();
    auto basicParams = object{
        object::pair{std:("prompt"), std:("Test prompt for ") + modelType + string_empty}, 
        object::pair{std:("stopSequences"), array<string>{ std:("STOP") }}, 
        object::pair{std:("maxTokens"), 100}
    };
    auto basicResponse = nullptr;
    auto basicError = nullptr;
    try
    {
        basicResponse = std::async([=]() { modelFn(mockRuntime, basicParams); });
        expect(basicResponse)->toBeTruthy();
        expect(type_of(basicResponse))->toBe(std:("string"));
    }
    catch (const any& e)
    {
        basicError = as<std::shared_ptr<Error>>(e);
        logger->error(string_empty + modelType + std:(" model call failed:"), e);
    }
    auto emptyParams = object{
        object::pair{std:("prompt"), string_empty}
    };
    auto emptyResponse = nullptr;
    auto emptyError = nullptr;
    try
    {
        emptyResponse = std::async([=]() { modelFn(mockRuntime, emptyParams); });
    }
    catch (const any& e)
    {
        emptyError = as<std::shared_ptr<Error>>(e);
        logger->error(string_empty + modelType + std:(" empty prompt test failed:"), e);
    }
    auto fullParams = object{
        object::pair{std:("prompt"), std:("Comprehensive test prompt for ") + modelType + string_empty}, 
        object::pair{std:("stopSequences"), array<string>{ std:("STOP1"), std:("STOP2") }}, 
        object::pair{std:("maxTokens"), 200}, 
        object::pair{std:("temperature"), 0.8}, 
        object::pair{std:("frequencyPenalty"), 0.6}, 
        object::pair{std:("presencePenalty"), 0.4}
    };
    auto fullResponse = nullptr;
    auto fullError = nullptr;
    try
    {
        fullResponse = std::async([=]() { modelFn(mockRuntime, fullParams); });
    }
    catch (const any& e)
    {
        fullError = as<std::shared_ptr<Error>>(e);
        logger->error(string_empty + modelType + std:(" all parameters test failed:"), e);
    }
    return object{
        object::pair{std:("basic"), object{
            object::pair{std:("response"), basicResponse}, 
            object::pair{std:("error"), basicError}
        }}, 
        object::pair{std:("empty"), object{
            object::pair{std:("response"), emptyResponse}, 
            object::pair{std:("error"), emptyError}
        }}, 
        object::pair{std:("full"), object{
            object::pair{std:("response"), fullResponse}, 
            object::pair{std:("error"), fullError}
        }}
    };
};

void Main(void)
{
    dotenv->config();
    beforeAll([=]() mutable
    {
        spyOn(logger, std:("info"));
        spyOn(logger, std:("error"));
        spyOn(logger, std:("warn"));
    }
    );
    afterAll([=]() mutable
    {
    }
    );
    describe(std:("Plugin Models"), [=]() mutable
    {
        it(std:("should have models defined"), [=]() mutable
        {
            expect(plugin->models)->toBeDefined();
            if (plugin->models) {
                expect(type_of(plugin->models))->toBe(std:("object"));
            }
        }
        );
        describe(std:("TEXT_SMALL Model"), [=]() mutable
        {
            it(std:("should have a TEXT_SMALL model defined"), [=]() mutable
            {
                if (plugin->models) {
                    expect(plugin->models)->toHaveProperty(ModelType->TEXT_SMALL);
                    expect(type_of(const_(plugin->models)[ModelType->TEXT_SMALL]))->toBe(std:("function"));
                }
            }
            );
            it(std:("should run core tests for TEXT_SMALL model"), [=]() mutable
            {
                if (AND((plugin->models), (const_(plugin->models)[ModelType->TEXT_SMALL]))) {
                    auto results = std::async([=]() { runCoreModelTests(ModelType->TEXT_SMALL, const_(plugin->models)[ModelType->TEXT_SMALL]); });
                    documentTestResult(std:("TEXT_SMALL core model tests"), results);
                }
            }
            );
        }
        );
        describe(std:("TEXT_LARGE Model"), [=]() mutable
        {
            it(std:("should have a TEXT_LARGE model defined"), [=]() mutable
            {
                if (plugin->models) {
                    expect(plugin->models)->toHaveProperty(ModelType->TEXT_LARGE);
                    expect(type_of(const_(plugin->models)[ModelType->TEXT_LARGE]))->toBe(std:("function"));
                }
            }
            );
            it(std:("should run core tests for TEXT_LARGE model"), [=]() mutable
            {
                if (AND((plugin->models), (const_(plugin->models)[ModelType->TEXT_LARGE]))) {
                    auto results = std::async([=]() { runCoreModelTests(ModelType->TEXT_LARGE, const_(plugin->models)[ModelType->TEXT_LARGE]); });
                    documentTestResult(std:("TEXT_LARGE core model tests"), results);
                }
            }
            );
        }
        );
    }
    );
}

MAIN
