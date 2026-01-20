#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tests/test-models.test.h"

void Main(void)
{
    describe(std::string("Models"), [=]() mutable
    {
        describe(std::string("LiteLLM Mock"), [=]() mutable
        {
            it(std::string("should handle mock responses"), [=]() mutable
            {
                auto config = object{
                    object::pair{std::string("name"), std::string("gpt-4o")}, 
                    object::pair{std::string("completionKwargs"), object{
                        object::pair{std::string("mockResponse"), std::string("Hello, world!")}
                    }}, 
                    object::pair{std::string("apiKey"), std::string("dummy_key")}, 
                    object::pair{std::string("topP"), nullptr}, 
                    object::pair{std::string("perInstanceCostLimit"), 3}, 
                    object::pair{std::string("totalCostLimit"), 0}, 
                    object::pair{std::string("perInstanceCallLimit"), 0}, 
                    object::pair{std::string("temperature"), 0}, 
                    object::pair{std::string("stop"), array<any>()}, 
                    object::pair{std::string("convertSystemToUser"), false}, 
                    object::pair{std::string("retry"), object{
                        object::pair{std::string("retries"), 20}, 
                        object::pair{std::string("minWait"), 10}, 
                        object::pair{std::string("maxWait"), 120}
                    }}, 
                    object::pair{std::string("delay"), 0}, 
                    object::pair{std::string("fallbacks"), array<any>()}, 
                    object::pair{std::string("chooseApiKeyByThread"), true}
                };
                auto tools = object{
                    object::pair{std::string("commands"), array<any>()}, 
                    object::pair{std::string("useFunctionCalling"), false}, 
                    object::pair{std::string("submitCommand"), std::string("submit")}, 
                    object::pair{std::string("executionTimeout"), 60}, 
                    object::pair{std::string("maxConsecutiveExecutionTimeouts"), 3}, 
                    object::pair{std::string("totalExecutionTimeout"), 300}, 
                    object::pair{std::string("formatErrorTemplate"), std::string("Error: {error}")}
                };
                auto model = getModel(config, tools);
                expect(model)->toBeDefined();
                expect(config->name)->toBe(std::string("gpt-4o"));
            }
            );
        }
        );
        describe(std::string("Model configuration"), [=]() mutable
        {
            it(std::string("should create model with custom configuration"), [=]() mutable
            {
                auto config = object{
                    object::pair{std::string("name"), std::string("custom-model")}, 
                    object::pair{std::string("apiBase"), std::string("https://api.custom.com")}, 
                    object::pair{std::string("apiKey"), std::string("test-key")}, 
                    object::pair{std::string("temperature"), 0.7}, 
                    object::pair{std::string("topP"), 0.9}, 
                    object::pair{std::string("maxOutputTokens"), 2000}, 
                    object::pair{std::string("perInstanceCostLimit"), 3}, 
                    object::pair{std::string("totalCostLimit"), 0}, 
                    object::pair{std::string("perInstanceCallLimit"), 0}, 
                    object::pair{std::string("stop"), array<any>()}, 
                    object::pair{std::string("completionKwargs"), object{}}, 
                    object::pair{std::string("convertSystemToUser"), false}, 
                    object::pair{std::string("retry"), object{
                        object::pair{std::string("retries"), 5}, 
                        object::pair{std::string("minWait"), 10}, 
                        object::pair{std::string("maxWait"), 120}
                    }}, 
                    object::pair{std::string("delay"), 1}, 
                    object::pair{std::string("fallbacks"), array<any>()}, 
                    object::pair{std::string("chooseApiKeyByThread"), true}
                };
                expect(config->name)->toBe(std::string("custom-model"));
                expect(config->apiBase)->toBe(std::string("https://api.custom.com"));
                expect(config->temperature)->toBe(0.7);
                expect(config->topP)->toBe(0.9);
                expect(config->maxOutputTokens)->toBe(2000);
                expect(config->retry->retries)->toBe(5);
            }
            );
            it(std::string("should handle human model configuration"), [=]() mutable
            {
                auto humanConfig = object{
                    object::pair{std::string("type"), std::string("human")}, 
                    object::pair{std::string("name"), std::string("human")}
                };
                expect(humanConfig["type"])->toBe(std::string("human"));
                expect(humanConfig["name"])->toBe(std::string("human"));
            }
            );
        }
        );
        describe(std::string("Model stats tracking"), [=]() mutable
        {
            it(std::string("should track API usage stats"), [=]() mutable
            {
                auto stats = std::make_shared<InstanceStats>();
                stats->tokensSent = 100;
                stats->tokensReceived = 50;
                stats->apiCalls = 1;
                stats->instanceCost = 0.005;
                expect(stats->tokensSent)->toBe(100);
                expect(stats->tokensReceived)->toBe(50);
                expect(stats->apiCalls)->toBe(1);
                expect(stats->instanceCost)->toBe(0.005);
            }
            );
            it(std::string("should track instance stats addition"), [=]() mutable
            {
                auto stats1 = std::make_shared<InstanceStats>();
                stats1->tokensSent = 100;
                stats1->tokensReceived = 50;
                stats1->apiCalls = 1;
                stats1->instanceCost = 0.005;
                auto stats2 = std::make_shared<InstanceStats>();
                stats2->tokensSent = 200;
                stats2->tokensReceived = 100;
                stats2->apiCalls = 2;
                stats2->instanceCost = 0.01;
                auto combined = stats1->add(stats2);
                expect(combined->tokensSent)->toBe(300);
                expect(combined->tokensReceived)->toBe(150);
                expect(combined->apiCalls)->toBe(3);
                expect(combined->instanceCost)->toBeCloseTo(0.015);
            }
            );
            it(std::string("should track global stats"), [=]() mutable
            {
                auto globalStats = std::make_shared<GlobalStats>();
                globalStats->addCost(0.005);
                globalStats->addCost(0.01);
                expect(globalStats->totalCost)->toBeCloseTo(0.015);
            }
            );
        }
        );
        describe(std::string("Model retry logic"), [=]() mutable
        {
            it(std::string("should handle retry configuration"), [=]() mutable
            {
                auto config = object{
                    object::pair{std::string("name"), std::string("test-model")}, 
                    object::pair{std::string("apiKey"), std::string("test-key")}, 
                    object::pair{std::string("perInstanceCostLimit"), 3}, 
                    object::pair{std::string("totalCostLimit"), 0}, 
                    object::pair{std::string("perInstanceCallLimit"), 0}, 
                    object::pair{std::string("temperature"), 0}, 
                    object::pair{std::string("topP"), 1}, 
                    object::pair{std::string("stop"), array<any>()}, 
                    object::pair{std::string("completionKwargs"), object{}}, 
                    object::pair{std::string("convertSystemToUser"), false}, 
                    object::pair{std::string("retry"), object{
                        object::pair{std::string("retries"), 3}, 
                        object::pair{std::string("minWait"), 10}, 
                        object::pair{std::string("maxWait"), 120}
                    }}, 
                    object::pair{std::string("delay"), 0.5}, 
                    object::pair{std::string("fallbacks"), array<any>()}, 
                    object::pair{std::string("chooseApiKeyByThread"), true}
                };
                expect(config->retry->retries)->toBe(3);
                expect(config->delay)->toBe(0.5);
                expect(config->name)->toBe(std::string("test-model"));
            }
            );
        }
        );
    }
    );
}

MAIN
