#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-starter/src/__tests__/models.test.h"

std::function<std::shared_ptr<Promise<object>>(any, std::function<std::shared_ptr<Promise<string>>(any, std::shared_ptr<TestGenerateParams>)>)> runCoreModelTests = [=](auto modelType, auto modelFn) mutable
{
    auto mockRuntime = createMockRuntime();
    auto basicParams = object{
        object::pair{std::string("prompt"), std::string("Test prompt for ") + modelType + string_empty}, 
        object::pair{std::string("stopSequences"), array<string>{ std::string("STOP") }}, 
        object::pair{std::string("maxTokens"), 100}
    };
    auto basicResponse = nullptr;
    auto basicError = nullptr;
    try
    {
        basicResponse = std::async([=]() { modelFn(mockRuntime, basicParams); });
        expect(basicResponse)->toBeTruthy();
        expect(type_of(basicResponse))->toBe(std::string("string"));
    }
    catch (const any& e)
    {
        basicError = as<std::shared_ptr<Error>>(e);
        logger->error(string_empty + modelType + std::string(" model call failed:"), e);
    }
    auto emptyParams = object{
        object::pair{std::string("prompt"), string_empty}
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
        logger->error(string_empty + modelType + std::string(" empty prompt test failed:"), e);
    }
    auto fullParams = object{
        object::pair{std::string("prompt"), std::string("Comprehensive test prompt for ") + modelType + string_empty}, 
        object::pair{std::string("stopSequences"), array<string>{ std::string("STOP1"), std::string("STOP2") }}, 
        object::pair{std::string("maxTokens"), 200}, 
        object::pair{std::string("temperature"), 0.8}, 
        object::pair{std::string("frequencyPenalty"), 0.6}, 
        object::pair{std::string("presencePenalty"), 0.4}
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
        logger->error(string_empty + modelType + std::string(" all parameters test failed:"), e);
    }
    return object{
        object::pair{std::string("basic"), object{
            object::pair{std::string("response"), basicResponse}, 
            object::pair{std::string("error"), basicError}
        }}, 
        object::pair{std::string("empty"), object{
            object::pair{std::string("response"), emptyResponse}, 
            object::pair{std::string("error"), emptyError}
        }}, 
        object::pair{std::string("full"), object{
            object::pair{std::string("response"), fullResponse}, 
            object::pair{std::string("error"), fullError}
        }}
    };
};

void Main(void)
{
    dotenv->config();
    beforeAll([=]() mutable
    {
        spyOn(logger, std::string("info"));
        spyOn(logger, std::string("error"));
        spyOn(logger, std::string("warn"));
    }
    );
    afterAll([=]() mutable
    {
    }
    );
    describe(std::string("Plugin Models"), [=]() mutable
    {
        it(std::string("should have models defined"), [=]() mutable
        {
            expect(plugin->models)->toBeDefined();
            if (plugin->models) {
                expect(type_of(plugin->models))->toBe(std::string("object"));
            }
        }
        );
        describe(std::string("TEXT_SMALL Model"), [=]() mutable
        {
            it(std::string("should have a TEXT_SMALL model defined"), [=]() mutable
            {
                if (plugin->models) {
                    expect(plugin->models)->toHaveProperty(ModelType->TEXT_SMALL);
                    expect(type_of(const_(plugin->models)[ModelType->TEXT_SMALL]))->toBe(std::string("function"));
                }
            }
            );
            it(std::string("should run core tests for TEXT_SMALL model"), [=]() mutable
            {
                if (AND((plugin->models), (const_(plugin->models)[ModelType->TEXT_SMALL]))) {
                    auto results = std::async([=]() { runCoreModelTests(ModelType->TEXT_SMALL, const_(plugin->models)[ModelType->TEXT_SMALL]); });
                    documentTestResult(std::string("TEXT_SMALL core model tests"), results);
                }
            }
            );
        }
        );
        describe(std::string("TEXT_LARGE Model"), [=]() mutable
        {
            it(std::string("should have a TEXT_LARGE model defined"), [=]() mutable
            {
                if (plugin->models) {
                    expect(plugin->models)->toHaveProperty(ModelType->TEXT_LARGE);
                    expect(type_of(const_(plugin->models)[ModelType->TEXT_LARGE]))->toBe(std::string("function"));
                }
            }
            );
            it(std::string("should run core tests for TEXT_LARGE model"), [=]() mutable
            {
                if (AND((plugin->models), (const_(plugin->models)[ModelType->TEXT_LARGE]))) {
                    auto results = std::async([=]() { runCoreModelTests(ModelType->TEXT_LARGE, const_(plugin->models)[ModelType->TEXT_LARGE]); });
                    documentTestResult(std::string("TEXT_LARGE core model tests"), results);
                }
            }
            );
        }
        );
    }
    );
}

MAIN
