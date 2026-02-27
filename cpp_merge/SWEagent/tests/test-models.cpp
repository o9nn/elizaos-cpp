#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tests/test-models.test.h"

void Main(void)
{
    describe(std:("Models"), [=]() mutable
    {
        describe(std:("LiteLLM Mock"), [=]() mutable
        {
            it(std:("should handle mock responses"), [=]() mutable
            {
                auto config = object{
                    object::pair{std:("name"), std:("gpt-4o")}, 
                    object::pair{std:("completionKwargs"), object{
                        object::pair{std:("mockResponse"), std:("Hello, world!")}
                    }}, 
                    object::pair{std:("apiKey"), std:("dummy_key")}, 
                    object::pair{std:("topP"), nullptr}, 
                    object::pair{std:("perInstanceCostLimit"), 3}, 
                    object::pair{std:("totalCostLimit"), 0}, 
                    object::pair{std:("perInstanceCallLimit"), 0}, 
                    object::pair{std:("temperature"), 0}, 
                    object::pair{std:("stop"), array<any>()}, 
                    object::pair{std:("convertSystemToUser"), false}, 
                    object::pair{std:("retry"), object{
                        object::pair{std:("retries"), 20}, 
                        object::pair{std:("minWait"), 10}, 
                        object::pair{std:("maxWait"), 120}
                    }}, 
                    object::pair{std:("delay"), 0}, 
                    object::pair{std:("fallbacks"), array<any>()}, 
                    object::pair{std:("chooseApiKeyByThread"), true}
                };
                auto tools = object{
                    object::pair{std:("commands"), array<any>()}, 
                    object::pair{std:("useFunctionCalling"), false}, 
                    object::pair{std:("submitCommand"), std:("submit")}, 
                    object::pair{std:("executionTimeout"), 60}, 
                    object::pair{std:("maxConsecutiveExecutionTimeouts"), 3}, 
                    object::pair{std:("totalExecutionTimeout"), 300}, 
                    object::pair{std:("formatErrorTemplate"), std:("Error: {error}")}
                };
                auto model = getModel(config, tools);
                expect(model)->toBeDefined();
                expect(config->name)->toBe(std:("gpt-4o"));
            }
            );
        }
        );
        describe(std:("Model configuration"), [=]() mutable
        {
            it(std:("should create model with custom configuration"), [=]() mutable
            {
                auto config = object{
                    object::pair{std:("name"), std:("custom-model")}, 
                    object::pair{std:("apiBase"), std:("https://api.custom.com")}, 
                    object::pair{std:("apiKey"), std:("test-key")}, 
                    object::pair{std:("temperature"), 0.7}, 
                    object::pair{std:("topP"), 0.9}, 
                    object::pair{std:("maxOutputTokens"), 2000}, 
                    object::pair{std:("perInstanceCostLimit"), 3}, 
                    object::pair{std:("totalCostLimit"), 0}, 
                    object::pair{std:("perInstanceCallLimit"), 0}, 
                    object::pair{std:("stop"), array<any>()}, 
                    object::pair{std:("completionKwargs"), object{}}, 
                    object::pair{std:("convertSystemToUser"), false}, 
                    object::pair{std:("retry"), object{
                        object::pair{std:("retries"), 5}, 
                        object::pair{std:("minWait"), 10}, 
                        object::pair{std:("maxWait"), 120}
                    }}, 
                    object::pair{std:("delay"), 1}, 
                    object::pair{std:("fallbacks"), array<any>()}, 
                    object::pair{std:("chooseApiKeyByThread"), true}
                };
                expect(config->name)->toBe(std:("custom-model"));
                expect(config->apiBase)->toBe(std:("https://api.custom.com"));
                expect(config->temperature)->toBe(0.7);
                expect(config->topP)->toBe(0.9);
                expect(config->maxOutputTokens)->toBe(2000);
                expect(config->retry->retries)->toBe(5);
            }
            );
            it(std:("should handle human model configuration"), [=]() mutable
            {
                auto humanConfig = object{
                    object::pair{std:("type"), std:("human")}, 
                    object::pair{std:("name"), std:("human")}
                };
                expect(humanConfig["type"])->toBe(std:("human"));
                expect(humanConfig["name"])->toBe(std:("human"));
            }
            );
        }
        );
        describe(std:("Model stats tracking"), [=]() mutable
        {
            it(std:("should track API usage stats"), [=]() mutable
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
            it(std:("should track instance stats addition"), [=]() mutable
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
            it(std:("should track global stats"), [=]() mutable
            {
                auto globalStats = std::make_shared<GlobalStats>();
                globalStats->addCost(0.005);
                globalStats->addCost(0.01);
                expect(globalStats->totalCost)->toBeCloseTo(0.015);
            }
            );
        }
        );
        describe(std:("Model retry logic"), [=]() mutable
        {
            it(std:("should handle retry configuration"), [=]() mutable
            {
                auto config = object{
                    object::pair{std:("name"), std:("test-model")}, 
                    object::pair{std:("apiKey"), std:("test-key")}, 
                    object::pair{std:("perInstanceCostLimit"), 3}, 
                    object::pair{std:("totalCostLimit"), 0}, 
                    object::pair{std:("perInstanceCallLimit"), 0}, 
                    object::pair{std:("temperature"), 0}, 
                    object::pair{std:("topP"), 1}, 
                    object::pair{std:("stop"), array<any>()}, 
                    object::pair{std:("completionKwargs"), object{}}, 
                    object::pair{std:("convertSystemToUser"), false}, 
                    object::pair{std:("retry"), object{
                        object::pair{std:("retries"), 3}, 
                        object::pair{std:("minWait"), 10}, 
                        object::pair{std:("maxWait"), 120}
                    }}, 
                    object::pair{std:("delay"), 0.5}, 
                    object::pair{std:("fallbacks"), array<any>()}, 
                    object::pair{std:("chooseApiKeyByThread"), true}
                };
                expect(config->retry->retries)->toBe(3);
                expect(config->delay)->toBe(0.5);
                expect(config->name)->toBe(std:("test-model"));
            }
            );
        }
        );
    }
    );
}

MAIN
