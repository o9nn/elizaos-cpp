#include "index.test.h"
#include <string>

std::function<any(std::shared_ptr<Record<string, string>>)> createMockRuntime = [=](auto settings = object{}) mutable
{
    return as<std::shared_ptr<IAgentRuntime>>(as<any>((object{
        object::pair{std::string("agentId"), std::string("test-agent-123")}, 
        object::pair{std::string("getSetting"), [=](auto key) mutable
        {
            return const_(settings)[key];
        }
        }, 
        object::pair{std::string("setSetting"), vi->fn()}, 
        object::pair{std::string("useModel"), vi->fn()}, 
        object::pair{std::string("character"), object{
            object::pair{std::string("name"), std::string("TestAgent")}
        }}, 
        object::pair{std::string("getService"), vi->fn([=](auto serviceName) mutable
        {
            return nullptr;
        }
        )}, 
        object::pair{std::string("emitEvent"), vi->fn()}
    })));
};

void Main(void)
{
    describe(std::string("Inference Plugin"), [=]() mutable
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
        describe(std::string("Plugin Initialization"), [=]() mutable
        {
            it(std::string("should initialize the plugin successfully"), [=]() mutable
            {
                std::async([=]() { expect(inferencePlugin->init(object{}, runtime))->resolves->not->toThrow(); });
            }
            );
            it(std::string("should have correct plugin metadata"), [=]() mutable
            {
                expect(inferencePlugin->name)->toBe(std::string("inference"));
                expect(inferencePlugin->description)->toContain(std::string("Dynamic model provider routing"));
            }
            );
        }
        );
        describe(std::string("Provider Status"), [=]() mutable
        {
            it(std::string("should return status of all providers"), [=]() mutable
            {
                runtime = createMockRuntime(object{
                    object::pair{std::string("OPENAI_API_KEY"), std::string("test-key")}, 
                    object::pair{std::string("ANTHROPIC_API_KEY"), std::string("test-key")}, 
                    object::pair{std::string("GROQ_API_KEY"), std::string("test-key")}
                });
                auto status = std::async([=]() { getProviderStatus(runtime); });
                expect(status["providers"])->toHaveLength(6);
                auto openai = status["providers"]->find([=](auto p) mutable
                {
                    return p["name"] == std::string("openai");
                }
                );
                auto anthropic = status["providers"]->find([=](auto p) mutable
                {
                    return p["name"] == std::string("anthropic");
                }
                );
                auto groq = status["providers"]->find([=](auto p) mutable
                {
                    return p["name"] == std::string("groq");
                }
                );
                auto ollama = status["providers"]->find([=](auto p) mutable
                {
                    return p["name"] == std::string("ollama");
                }
                );
                auto elizaos = status["providers"]->find([=](auto p) mutable
                {
                    return p["name"] == std::string("elizaos");
                }
                );
                expect(openai)->toBeDefined();
                expect(openai)->toMatchObject(object{
                    object::pair{std::string("name"), std::string("openai")}, 
                    object::pair{std::string("displayName"), std::string("OpenAI")}
                });
                expect(array<string>{ std::string("available"), std::string("not_configured"), std::string("error") })->toContain(openai["status"]);
                expect(anthropic)->toBeDefined();
                expect(anthropic)->toMatchObject(object{
                    object::pair{std::string("name"), std::string("anthropic")}, 
                    object::pair{std::string("displayName"), std::string("Anthropic Claude")}
                });
                expect(array<string>{ std::string("available"), std::string("not_configured"), std::string("error") })->toContain(anthropic["status"]);
                expect(groq)->toBeDefined();
                expect(groq)->toMatchObject(object{
                    object::pair{std::string("name"), std::string("groq")}, 
                    object::pair{std::string("displayName"), std::string("Groq")}
                });
                expect(array<string>{ std::string("available"), std::string("not_configured"), std::string("error") })->toContain(groq["status"]);
                expect(ollama)->toBeDefined();
                expect(ollama)->toMatchObject(object{
                    object::pair{std::string("name"), std::string("ollama")}, 
                    object::pair{std::string("displayName"), std::string("Ollama (Local)")}
                });
                expect(array<string>{ std::string("available"), std::string("not_configured"), std::string("error") })->toContain(ollama["status"]);
                expect(elizaos)->toBeDefined();
                expect(elizaos)->toMatchObject(object{
                    object::pair{std::string("name"), std::string("elizaos")}, 
                    object::pair{std::string("displayName"), std::string("ElizaOS Cloud")}
                });
                expect(array<string>{ std::string("not_configured"), std::string("error") })->toContain(elizaos["status"]);
            }
            );
            it(std::string("should handle providers without API keys"), [=]() mutable
            {
                runtime = createMockRuntime(object{});
                auto status = std::async([=]() { getProviderStatus(runtime); });
                expect(status["providers"])->toHaveLength(6);
                auto openai = status["providers"]->find([=](auto p) mutable
                {
                    return p["name"] == std::string("openai");
                }
                );
                auto anthropic = status["providers"]->find([=](auto p) mutable
                {
                    return p["name"] == std::string("anthropic");
                }
                );
                auto groq = status["providers"]->find([=](auto p) mutable
                {
                    return p["name"] == std::string("groq");
                }
                );
                expect(openai["status"])->toBe(std::string("not_configured"));
                expect(anthropic["status"])->toBe(std::string("not_configured"));
                expect(groq["status"])->toBe(std::string("not_configured"));
            }
            );
        }
        );
        describe(std::string("Provider Selection"), [=]() mutable
        {
            it(std::string("should set selected provider"), [=]() mutable
            {
                runtime = createMockRuntime(object{
                    object::pair{std::string("OPENAI_API_KEY"), std::string("test-key")}, 
                    object::pair{std::string("ANTHROPIC_API_KEY"), std::string("test-key")}
                });
                std::async([=]() { setSelectedProvider(runtime, std::string("anthropic")); });
                auto status = std::async([=]() { getProviderStatus(runtime); });
                expect(status["selected"])->toBe(std::string("anthropic"));
                expect(runtime->setSetting)->toHaveBeenCalledWith(std::string("SELECTED_PROVIDER"), std::string("anthropic"));
            }
            );
            it(std::string("should clear selected provider when null is passed"), [=]() mutable
            {
                runtime = createMockRuntime(object{
                    object::pair{std::string("SELECTED_PROVIDER"), std::string("openai")}
                });
                std::async([=]() { setSelectedProvider(runtime, nullptr); });
                auto status = std::async([=]() { getProviderStatus(runtime); });
                expect(status["selected"])->toBeNull();
            }
            );
            it(std::string("should throw error for unknown provider"), [=]() mutable
            {
                std::async([=]() { expect(setSelectedProvider(runtime, std::string("unknown-provider")))->rejects->toThrow(std::string("Unknown provider: unknown-provider")); });
            }
            );
        }
        );
        describe(std::string("Provider Preferences"), [=]() mutable
        {
            it(std::string("should set and use provider preferences"), [=]() mutable
            {
                runtime = createMockRuntime(object{
                    object::pair{std::string("OPENAI_API_KEY"), std::string("test-key")}, 
                    object::pair{std::string("ANTHROPIC_API_KEY"), std::string("test-key")}, 
                    object::pair{std::string("GROQ_API_KEY"), std::string("test-key")}
                });
                std::async([=]() { setProviderPreferences(runtime, array<string>{ std::string("groq"), std::string("openai"), std::string("anthropic"), std::string("ollama"), std::string("elizaos") }); });
                auto status = std::async([=]() { getProviderStatus(runtime); });
                expect(status["preferences"])->toEqual(array<string>{ std::string("groq"), std::string("openai"), std::string("anthropic"), std::string("ollama"), std::string("elizaos") });
            }
            );
            it(std::string("should filter out invalid providers from preferences"), [=]() mutable
            {
                auto preferences = array<string>{ std::string("anthropic"), std::string("invalid-provider"), std::string("openai") };
                std::async([=]() { setProviderPreferences(runtime, preferences); });
                expect(runtime->setSetting)->toHaveBeenCalledWith(std::string("INFERENCE_PREFERENCES"), std::string("anthropic,openai"));
            }
            );
            it(std::string("should load preferences from runtime settings on init"), [=]() mutable
            {
                runtime = createMockRuntime(object{
                    object::pair{std::string("INFERENCE_PREFERENCES"), std::string("anthropic,openai")}, 
                    object::pair{std::string("ANTHROPIC_API_KEY"), std::string("test-key")}, 
                    object::pair{std::string("OPENAI_API_KEY"), std::string("test-key")}
                });
                std::async([=]() { inferencePlugin->init(object{}, runtime); });
                auto status = std::async([=]() { getProviderStatus(runtime); });
                expect(status["preferences"])->toEqual(array<string>{ std::string("anthropic"), std::string("openai") });
                expect(array<string>{ std::string("anthropic"), std::string("openai"), std::string("local_embedding"), std::string("ollama") })->toContain(status["active"]);
            }
            );
        }
        );
        describe(std::string("Model Routing"), [=]() mutable
        {
            it(std::string("should route model calls to available providers"), [=]() mutable
            {
                runtime = createMockRuntime(object{
                    object::pair{std::string("OLLAMA_AVAILABLE"), std::string("true")}
                });
                std::async([=]() { inferencePlugin->init(object{}, runtime); });
                if (AND((inferencePlugin->models), (const_(inferencePlugin->models)[ModelType->TEXT_SMALL]))) {
                    auto result = std::async([=]() { const_(inferencePlugin->models)[ModelType->TEXT_SMALL](runtime, object{
                        object::pair{std::string("prompt"), std::string("Test prompt")}
                    }); });
                    expect(result)->toBe(std::string("Ollama response"));
                }
            }
            );
            it(std::string("should handle fallback when no providers available"), [=]() mutable
            {
                runtime = createMockRuntime(object{
                    object::pair{std::string("OLLAMA_AVAILABLE"), std::string("false")}
                });
                if (AND((inferencePlugin->models), (const_(inferencePlugin->models)[ModelType->TEXT_SMALL]))) {
                    std::async([=]() { expect(const_(inferencePlugin->models)[ModelType->TEXT_SMALL](runtime, object{
                        object::pair{std::string("prompt"), std::string("Test")}
                    }))->rejects->toThrow(); });
                }
            }
            );
            it(std::string("should route embeddings correctly"), [=]() mutable
            {
                runtime = createMockRuntime(object{
                    object::pair{std::string("OPENAI_API_KEY"), std::string("test-key")}, 
                    object::pair{std::string("ANTHROPIC_API_KEY"), std::string("test-key")}, 
                    object::pair{std::string("GROQ_API_KEY"), std::string("test-key")}
                });
                std::async([=]() { inferencePlugin->init(object{}, runtime); });
                if (inferencePlugin->models) {
                    auto result = std::async([=]() { const_(inferencePlugin->models)[ModelType->TEXT_EMBEDDING](runtime, object{
                        object::pair{std::string("input"), std::string("Hello world")}
                    }); });
                    expect(Array->isArray(result))->toBe(true);
                    expect(result->length)->toBeGreaterThan(0);
                    expect(result)->toEqual(array<double>{ 0.1, 0.2, 0.3, 0.4, 0.5 });
                }
            }
            );
            it(std::string("should throw error when provider does not support model type"), [=]() mutable
            {
                runtime = createMockRuntime(object{
                    object::pair{std::string("OPENAI_API_KEY"), std::string("test-key")}, 
                    object::pair{std::string("OLLAMA_AVAILABLE"), std::string("true")}
                });
                std::async([=]() { inferencePlugin->init(object{}, runtime); });
                if (AND((inferencePlugin->models), (const_(inferencePlugin->models)[ModelType->IMAGE]))) {
                    std::async([=]() { expect(const_(inferencePlugin->models)[ModelType->IMAGE](runtime, object{
                        object::pair{std::string("prompt"), std::string("Test")}
                    }))->rejects->toThrow(std::string("No available provider supports model type")); });
                }
            }
            );
            it(std::string("should throw error when local embedding plugin is not available"), [=]() mutable
            {
                runtime = createMockRuntime(object{
                    object::pair{std::string("OPENAI_API_KEY"), std::string("test-key")}, 
                    object::pair{std::string("ANTHROPIC_API_KEY"), std::string("test-key")}
                });
                std::async([=]() { inferencePlugin->init(object{}, runtime); });
                if (AND((inferencePlugin->models), (const_(inferencePlugin->models)[ModelType->TEXT_EMBEDDING]))) {
                    auto result = std::async([=]() { const_(inferencePlugin->models)[ModelType->TEXT_EMBEDDING](runtime, object{
                        object::pair{std::string("input"), std::string("Hello")}
                    }); });
                    expect(Array->isArray(result))->toBe(true);
                }
            }
            );
            it(std::string("should not use local_embedding for TEXT_LARGE model type"), [=]() mutable
            {
                runtime = createMockRuntime(object{
                    object::pair{std::string("OLLAMA_AVAILABLE"), std::string("true")}
                });
                std::async([=]() { inferencePlugin->init(object{}, runtime); });
                if (AND((inferencePlugin->models), (const_(inferencePlugin->models)[ModelType->TEXT_LARGE]))) {
                    auto result = std::async([=]() { const_(inferencePlugin->models)[ModelType->TEXT_LARGE](runtime, object{
                        object::pair{std::string("prompt"), std::string("Test prompt for large model")}
                    }); });
                    expect(result)->toBe(std::string("Ollama response"));
                }
            }
            );
            it(std::string("should always use local_embedding for TEXT_EMBEDDING regardless of preferences"), [=]() mutable
            {
                runtime = createMockRuntime(object{
                    object::pair{std::string("INFERENCE_PREFERENCES"), std::string("openai,anthropic")}, 
                    object::pair{std::string("OPENAI_API_KEY"), std::string("test-key")}, 
                    object::pair{std::string("ANTHROPIC_API_KEY"), std::string("test-key")}
                });
                std::async([=]() { inferencePlugin->init(object{}, runtime); });
                if (AND((inferencePlugin->models), (const_(inferencePlugin->models)[ModelType->TEXT_EMBEDDING]))) {
                    auto result = std::async([=]() { const_(inferencePlugin->models)[ModelType->TEXT_EMBEDDING](runtime, object{
                        object::pair{std::string("input"), std::string("Test embedding")}
                    }); });
                    expect(Array->isArray(result))->toBe(true);
                    expect(result)->toEqual(array<double>{ 0.1, 0.2, 0.3, 0.4, 0.5 });
                }
            }
            );
        }
        );
        describe(std::string("Provider Priority"), [=]() mutable
        {
            it(std::string("should follow default priority order"), [=]() mutable
            {
                runtime = createMockRuntime(object{
                    object::pair{std::string("OPENAI_API_KEY"), std::string("test-key")}, 
                    object::pair{std::string("ANTHROPIC_API_KEY"), std::string("test-key")}, 
                    object::pair{std::string("GROQ_API_KEY"), std::string("test-key")}, 
                    object::pair{std::string("ELIZAOS_API_KEY"), std::string("test-key")}, 
                    object::pair{std::string("OLLAMA_AVAILABLE"), std::string("true")}
                });
                std::async([=]() { inferencePlugin->init(object{}, runtime); });
                auto status = std::async([=]() { getProviderStatus(runtime); });
                expect(status["active"])->toBeTruthy();
                expect(array<string>{ std::string("ollama"), std::string("groq"), std::string("elizaos"), std::string("openai"), std::string("anthropic") })->toContain(status["active"]);
            }
            );
            it(std::string("should respect custom preferences order"), [=]() mutable
            {
                runtime = createMockRuntime(object{
                    object::pair{std::string("INFERENCE_PREFERENCES"), std::string("openai,anthropic,elizaos")}, 
                    object::pair{std::string("OPENAI_API_KEY"), std::string("test-key")}, 
                    object::pair{std::string("ANTHROPIC_API_KEY"), std::string("test-key")}, 
                    object::pair{std::string("ELIZAOS_API_KEY"), std::string("test-key")}
                });
                std::async([=]() { inferencePlugin->init(object{}, runtime); });
                auto status = std::async([=]() { getProviderStatus(runtime); });
                expect(status["active"])->toBeTruthy();
                expect(array<string>{ std::string("openai"), std::string("anthropic"), std::string("elizaos"), std::string("local_embedding") })->toContain(status["active"]);
            }
            );
        }
        );
        describe(std::string("Plugin Tests"), [=]() mutable
        {
            it(std::string("should run plugin test suite"), [=]() mutable
            {
                runtime = createMockRuntime(object{
                    object::pair{std::string("OPENAI_API_KEY"), std::string("test-key")}
                });
                auto tests = OR((const_(inferencePlugin->tests)[0]->tests), (array<any>()));
                expect(tests)->toHaveLength(2);
                expect(const_(tests)[0]["name"])->toBe(std::string("test_provider_status"));
                expect(const_(tests)[1]["name"])->toBe(std::string("test_provider_routing"));
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
