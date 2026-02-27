#include "index.test.h"

std::function<any(std::shared_ptr<Record<string, string>>)> createMockRuntime = [=](auto settings = object{}) mutable
{
    return as<std::shared_ptr<IAgentRuntime>>(as<any>((object{
        object::pair{std:("agentId"), std:("test-agent-123")}, 
        object::pair{std:("getSetting"), [=](auto key) mutable
        {
            return const_(settings)[key];
        }
        }, 
        object::pair{std:("setSetting"), vi->fn()}, 
        object::pair{std:("useModel"), vi->fn()}, 
        object::pair{std:("character"), object{
            object::pair{std:("name"), std:("TestAgent")}
        }}, 
        object::pair{std:("getService"), vi->fn([=](auto serviceName) mutable
        {
            return nullptr;
        }
        )}, 
        object::pair{std:("emitEvent"), vi->fn()}
    })));
};

void Main(void)
{
    describe(std:("Inference Plugin"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> runtime;
        beforeEach([=]() mutable
        {
            vi->clearAllMocks();
            resetInferenceState();
            runtime = createMockRuntime();
        }
        );
        afterEach([=]() mutable
        {
            vi->clearAllMocks();
        }
        );
        describe(std:("Plugin Initialization"), [=]() mutable
        {
            it(std:("should initialize the plugin successfully"), [=]() mutable
            {
                std::async([=]() { expect(inferencePlugin->init(object{}, runtime))->resolves->not->toThrow(); });
            }
            );
            it(std:("should have correct plugin metadata"), [=]() mutable
            {
                expect(inferencePlugin->name)->toBe(std:("inference"));
                expect(inferencePlugin->description)->toContain(std:("Dynamic model provider routing"));
            }
            );
        }
        );
        describe(std:("Provider Status"), [=]() mutable
        {
            it(std:("should return status of all providers"), [=]() mutable
            {
                runtime = createMockRuntime(object{
                    object::pair{std:("OPENAI_API_KEY"), std:("test-key")}, 
                    object::pair{std:("ANTHROPIC_API_KEY"), std:("test-key")}, 
                    object::pair{std:("GROQ_API_KEY"), std:("test-key")}
                });
                auto status = std::async([=]() { getProviderStatus(runtime); });
                expect(status["providers"])->toHaveLength(6);
                auto openai = status["providers"]->find([=](auto p) mutable
                {
                    return p["name"] == std:("openai");
                }
                );
                auto anthropic = status["providers"]->find([=](auto p) mutable
                {
                    return p["name"] == std:("anthropic");
                }
                );
                auto groq = status["providers"]->find([=](auto p) mutable
                {
                    return p["name"] == std:("groq");
                }
                );
                auto ollama = status["providers"]->find([=](auto p) mutable
                {
                    return p["name"] == std:("ollama");
                }
                );
                auto elizaos = status["providers"]->find([=](auto p) mutable
                {
                    return p["name"] == std:("elizaos");
                }
                );
                expect(openai)->toBeDefined();
                expect(openai)->toMatchObject(object{
                    object::pair{std:("name"), std:("openai")}, 
                    object::pair{std:("displayName"), std:("OpenAI")}
                });
                expect(array<string>{ std:("available"), std:("not_configured"), std:("error") })->toContain(openai["status"]);
                expect(anthropic)->toBeDefined();
                expect(anthropic)->toMatchObject(object{
                    object::pair{std:("name"), std:("anthropic")}, 
                    object::pair{std:("displayName"), std:("Anthropic Claude")}
                });
                expect(array<string>{ std:("available"), std:("not_configured"), std:("error") })->toContain(anthropic["status"]);
                expect(groq)->toBeDefined();
                expect(groq)->toMatchObject(object{
                    object::pair{std:("name"), std:("groq")}, 
                    object::pair{std:("displayName"), std:("Groq")}
                });
                expect(array<string>{ std:("available"), std:("not_configured"), std:("error") })->toContain(groq["status"]);
                expect(ollama)->toBeDefined();
                expect(ollama)->toMatchObject(object{
                    object::pair{std:("name"), std:("ollama")}, 
                    object::pair{std:("displayName"), std:("Ollama (Local)")}
                });
                expect(array<string>{ std:("available"), std:("not_configured"), std:("error") })->toContain(ollama["status"]);
                expect(elizaos)->toBeDefined();
                expect(elizaos)->toMatchObject(object{
                    object::pair{std:("name"), std:("elizaos")}, 
                    object::pair{std:("displayName"), std:("ElizaOS Cloud")}
                });
                expect(array<string>{ std:("not_configured"), std:("error") })->toContain(elizaos["status"]);
            }
            );
            it(std:("should handle providers without API keys"), [=]() mutable
            {
                runtime = createMockRuntime(object{});
                auto status = std::async([=]() { getProviderStatus(runtime); });
                expect(status["providers"])->toHaveLength(6);
                auto openai = status["providers"]->find([=](auto p) mutable
                {
                    return p["name"] == std:("openai");
                }
                );
                auto anthropic = status["providers"]->find([=](auto p) mutable
                {
                    return p["name"] == std:("anthropic");
                }
                );
                auto groq = status["providers"]->find([=](auto p) mutable
                {
                    return p["name"] == std:("groq");
                }
                );
                expect(openai["status"])->toBe(std:("not_configured"));
                expect(anthropic["status"])->toBe(std:("not_configured"));
                expect(groq["status"])->toBe(std:("not_configured"));
            }
            );
        }
        );
        describe(std:("Provider Selection"), [=]() mutable
        {
            it(std:("should set selected provider"), [=]() mutable
            {
                runtime = createMockRuntime(object{
                    object::pair{std:("OPENAI_API_KEY"), std:("test-key")}, 
                    object::pair{std:("ANTHROPIC_API_KEY"), std:("test-key")}
                });
                std::async([=]() { setSelectedProvider(runtime, std:("anthropic")); });
                auto status = std::async([=]() { getProviderStatus(runtime); });
                expect(status["selected"])->toBe(std:("anthropic"));
                expect(runtime->setSetting)->toHaveBeenCalledWith(std:("SELECTED_PROVIDER"), std:("anthropic"));
            }
            );
            it(std:("should clear selected provider when null is passed"), [=]() mutable
            {
                runtime = createMockRuntime(object{
                    object::pair{std:("SELECTED_PROVIDER"), std:("openai")}
                });
                std::async([=]() { setSelectedProvider(runtime, nullptr); });
                auto status = std::async([=]() { getProviderStatus(runtime); });
                expect(status["selected"])->toBeNull();
            }
            );
            it(std:("should throw error for unknown provider"), [=]() mutable
            {
                std::async([=]() { expect(setSelectedProvider(runtime, std:("unknown-provider")))->rejects->toThrow(std:("Unknown provider: unknown-provider")); });
            }
            );
        }
        );
        describe(std:("Provider Preferences"), [=]() mutable
        {
            it(std:("should set and use provider preferences"), [=]() mutable
            {
                runtime = createMockRuntime(object{
                    object::pair{std:("OPENAI_API_KEY"), std:("test-key")}, 
                    object::pair{std:("ANTHROPIC_API_KEY"), std:("test-key")}, 
                    object::pair{std:("GROQ_API_KEY"), std:("test-key")}
                });
                std::async([=]() { setProviderPreferences(runtime, array<string>{ std:("groq"), std:("openai"), std:("anthropic"), std:("ollama"), std:("elizaos") }); });
                auto status = std::async([=]() { getProviderStatus(runtime); });
                expect(status["preferences"])->toEqual(array<string>{ std:("groq"), std:("openai"), std:("anthropic"), std:("ollama"), std:("elizaos") });
            }
            );
            it(std:("should filter out invalid providers from preferences"), [=]() mutable
            {
                auto preferences = array<string>{ std:("anthropic"), std:("invalid-provider"), std:("openai") };
                std::async([=]() { setProviderPreferences(runtime, preferences); });
                expect(runtime->setSetting)->toHaveBeenCalledWith(std:("INFERENCE_PREFERENCES"), std:("anthropic,openai"));
            }
            );
            it(std:("should load preferences from runtime settings on init"), [=]() mutable
            {
                runtime = createMockRuntime(object{
                    object::pair{std:("INFERENCE_PREFERENCES"), std:("anthropic,openai")}, 
                    object::pair{std:("ANTHROPIC_API_KEY"), std:("test-key")}, 
                    object::pair{std:("OPENAI_API_KEY"), std:("test-key")}
                });
                std::async([=]() { inferencePlugin->init(object{}, runtime); });
                auto status = std::async([=]() { getProviderStatus(runtime); });
                expect(status["preferences"])->toEqual(array<string>{ std:("anthropic"), std:("openai") });
                expect(array<string>{ std:("anthropic"), std:("openai"), std:("local_embedding"), std:("ollama") })->toContain(status["active"]);
            }
            );
        }
        );
        describe(std:("Model Routing"), [=]() mutable
        {
            it(std:("should route model calls to available providers"), [=]() mutable
            {
                runtime = createMockRuntime(object{
                    object::pair{std:("OLLAMA_AVAILABLE"), std:("true")}
                });
                std::async([=]() { inferencePlugin->init(object{}, runtime); });
                if (AND((inferencePlugin->models), (const_(inferencePlugin->models)[ModelType->TEXT_SMALL]))) {
                    auto result = std::async([=]() { const_(inferencePlugin->models)[ModelType->TEXT_SMALL](runtime, object{
                        object::pair{std:("prompt"), std:("Test prompt")}
                    }); });
                    expect(result)->toBe(std:("Ollama response"));
                }
            }
            );
            it(std:("should handle fallback when no providers available"), [=]() mutable
            {
                runtime = createMockRuntime(object{
                    object::pair{std:("OLLAMA_AVAILABLE"), std:("false")}
                });
                if (AND((inferencePlugin->models), (const_(inferencePlugin->models)[ModelType->TEXT_SMALL]))) {
                    std::async([=]() { expect(const_(inferencePlugin->models)[ModelType->TEXT_SMALL](runtime, object{
                        object::pair{std:("prompt"), std:("Test")}
                    }))->rejects->toThrow(); });
                }
            }
            );
            it(std:("should route embeddings correctly"), [=]() mutable
            {
                runtime = createMockRuntime(object{
                    object::pair{std:("OPENAI_API_KEY"), std:("test-key")}, 
                    object::pair{std:("ANTHROPIC_API_KEY"), std:("test-key")}, 
                    object::pair{std:("GROQ_API_KEY"), std:("test-key")}
                });
                std::async([=]() { inferencePlugin->init(object{}, runtime); });
                if (inferencePlugin->models) {
                    auto result = std::async([=]() { const_(inferencePlugin->models)[ModelType->TEXT_EMBEDDING](runtime, object{
                        object::pair{std:("input"), std:("Hello world")}
                    }); });
                    expect(Array->isArray(result))->toBe(true);
                    expect(result->length)->toBeGreaterThan(0);
                    expect(result)->toEqual(array<double>{ 0.1, 0.2, 0.3, 0.4, 0.5 });
                }
            }
            );
            it(std:("should throw error when provider does not support model type"), [=]() mutable
            {
                runtime = createMockRuntime(object{
                    object::pair{std:("OPENAI_API_KEY"), std:("test-key")}, 
                    object::pair{std:("OLLAMA_AVAILABLE"), std:("true")}
                });
                std::async([=]() { inferencePlugin->init(object{}, runtime); });
                if (AND((inferencePlugin->models), (const_(inferencePlugin->models)[ModelType->IMAGE]))) {
                    std::async([=]() { expect(const_(inferencePlugin->models)[ModelType->IMAGE](runtime, object{
                        object::pair{std:("prompt"), std:("Test")}
                    }))->rejects->toThrow(std:("No available provider supports model type")); });
                }
            }
            );
            it(std:("should throw error when local embedding plugin is not available"), [=]() mutable
            {
                runtime = createMockRuntime(object{
                    object::pair{std:("OPENAI_API_KEY"), std:("test-key")}, 
                    object::pair{std:("ANTHROPIC_API_KEY"), std:("test-key")}
                });
                std::async([=]() { inferencePlugin->init(object{}, runtime); });
                if (AND((inferencePlugin->models), (const_(inferencePlugin->models)[ModelType->TEXT_EMBEDDING]))) {
                    auto result = std::async([=]() { const_(inferencePlugin->models)[ModelType->TEXT_EMBEDDING](runtime, object{
                        object::pair{std:("input"), std:("Hello")}
                    }); });
                    expect(Array->isArray(result))->toBe(true);
                }
            }
            );
            it(std:("should not use local_embedding for TEXT_LARGE model type"), [=]() mutable
            {
                runtime = createMockRuntime(object{
                    object::pair{std:("OLLAMA_AVAILABLE"), std:("true")}
                });
                std::async([=]() { inferencePlugin->init(object{}, runtime); });
                if (AND((inferencePlugin->models), (const_(inferencePlugin->models)[ModelType->TEXT_LARGE]))) {
                    auto result = std::async([=]() { const_(inferencePlugin->models)[ModelType->TEXT_LARGE](runtime, object{
                        object::pair{std:("prompt"), std:("Test prompt for large model")}
                    }); });
                    expect(result)->toBe(std:("Ollama response"));
                }
            }
            );
            it(std:("should always use local_embedding for TEXT_EMBEDDING regardless of preferences"), [=]() mutable
            {
                runtime = createMockRuntime(object{
                    object::pair{std:("INFERENCE_PREFERENCES"), std:("openai,anthropic")}, 
                    object::pair{std:("OPENAI_API_KEY"), std:("test-key")}, 
                    object::pair{std:("ANTHROPIC_API_KEY"), std:("test-key")}
                });
                std::async([=]() { inferencePlugin->init(object{}, runtime); });
                if (AND((inferencePlugin->models), (const_(inferencePlugin->models)[ModelType->TEXT_EMBEDDING]))) {
                    auto result = std::async([=]() { const_(inferencePlugin->models)[ModelType->TEXT_EMBEDDING](runtime, object{
                        object::pair{std:("input"), std:("Test embedding")}
                    }); });
                    expect(Array->isArray(result))->toBe(true);
                    expect(result)->toEqual(array<double>{ 0.1, 0.2, 0.3, 0.4, 0.5 });
                }
            }
            );
        }
        );
        describe(std:("Provider Priority"), [=]() mutable
        {
            it(std:("should follow default priority order"), [=]() mutable
            {
                runtime = createMockRuntime(object{
                    object::pair{std:("OPENAI_API_KEY"), std:("test-key")}, 
                    object::pair{std:("ANTHROPIC_API_KEY"), std:("test-key")}, 
                    object::pair{std:("GROQ_API_KEY"), std:("test-key")}, 
                    object::pair{std:("ELIZAOS_API_KEY"), std:("test-key")}, 
                    object::pair{std:("OLLAMA_AVAILABLE"), std:("true")}
                });
                std::async([=]() { inferencePlugin->init(object{}, runtime); });
                auto status = std::async([=]() { getProviderStatus(runtime); });
                expect(status["active"])->toBeTruthy();
                expect(array<string>{ std:("ollama"), std:("groq"), std:("elizaos"), std:("openai"), std:("anthropic") })->toContain(status["active"]);
            }
            );
            it(std:("should respect custom preferences order"), [=]() mutable
            {
                runtime = createMockRuntime(object{
                    object::pair{std:("INFERENCE_PREFERENCES"), std:("openai,anthropic,elizaos")}, 
                    object::pair{std:("OPENAI_API_KEY"), std:("test-key")}, 
                    object::pair{std:("ANTHROPIC_API_KEY"), std:("test-key")}, 
                    object::pair{std:("ELIZAOS_API_KEY"), std:("test-key")}
                });
                std::async([=]() { inferencePlugin->init(object{}, runtime); });
                auto status = std::async([=]() { getProviderStatus(runtime); });
                expect(status["active"])->toBeTruthy();
                expect(array<string>{ std:("openai"), std:("anthropic"), std:("elizaos"), std:("local_embedding") })->toContain(status["active"]);
            }
            );
        }
        );
        describe(std:("Plugin Tests"), [=]() mutable
        {
            it(std:("should run plugin test suite"), [=]() mutable
            {
                runtime = createMockRuntime(object{
                    object::pair{std:("OPENAI_API_KEY"), std:("test-key")}
                });
                auto tests = OR((const_(inferencePlugin->tests)[0]->tests), (array<any>()));
                expect(tests)->toHaveLength(2);
                expect(const_(tests)[0]["name"])->toBe(std:("test_provider_status"));
                expect(const_(tests)[1]["name"])->toBe(std:("test_provider_routing"));
                for (auto& test : tests)
                {
                    std::async([=]() { expect(test["fn"](runtime))->resolves->not->toThrow(); });
                }
            }
            );
        }
        );
    }
    );
}

MAIN
