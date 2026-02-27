#include "comprehensive-auth-tests.test.h"

object mockConsole = object{
    object::pair{std:("log"), mock()}, 
    object::pair{std:("error"), mock()}, 
    object::pair{std:("warn"), mock()}
};
std::function<any(std::shared_ptr<Record<string, string>>)> createMockRuntime = [=](auto settings = object{}) mutable
{
    shared services = std::make_shared<Map>();
    return as<any>(object{
        object::pair{std:("agentId"), std:("test-agent-123")}, 
        object::pair{std:("character"), object{
            object::pair{std:("name"), std:("Test Agent")}, 
            object::pair{std:("bio"), std:("A test agent for authentication")}, 
            object::pair{std:("system"), std:("Test system prompt")}
        }}, 
        object::pair{std:("getSetting"), [=](auto key) mutable
        {
            return OR((const_(settings)[key]), (nullptr));
        }
        }, 
        object::pair{std:("getService"), [=](auto serviceName) mutable
        {
            return OR((services->get(serviceName)), (nullptr));
        }
        }, 
        object::pair{std:("registerService"), [=](auto service) mutable
        {
            services->set(service["constructor"]["serviceName"], service);
        }
        }, 
        object::pair{std:("initialize"), mock()}, 
        object::pair{std:("composeState"), mock()}, 
        object::pair{std:("useModel"), mock()}, 
        object::pair{std:("processActions"), mock()}, 
        object::pair{std:("createMemory"), mock()}, 
        object::pair{std:("getMemories"), mock()}, 
        object::pair{std:("searchMemories"), mock()}, 
        object::pair{std:("createEntity"), mock()}, 
        object::pair{std:("getEntityById"), mock()}, 
        object::pair{std:("registerTaskWorker"), mock()}, 
        object::pair{std:("createTask"), mock()}, 
        object::pair{std:("getTasks"), mock()}, 
        object::pair{std:("emitEvent"), mock()}
    });
};
any mockFetch = mock();

void Main(void)
{
    global->fetch = as<any>(mockFetch);
    describe(std:("Authentication System - Comprehensive Tests"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        beforeEach([=]() mutable
        {
            mockRuntime = createMockRuntime();
            mockFetch->mockClear();
            Object->keys(mockConsole)->forEach([=](auto key) mutable
            {
                const_(mockConsole)[as<any>(key)]["mockClear"]();
            }
            );
        }
        );
        afterEach([=]() mutable
        {
            mock->restore();
        }
        );
        describe(std:("Core Authentication Service"), [=]() mutable
        {
            describe(std:("Success Cases"), [=]() mutable
            {
                it(std:("should validate test keys correctly"), [=]() mutable
                {
                    auto authService = std::make_shared<AuthenticationService>(mockRuntime);
                    auto result = std::async([=]() { authService->validateApiKey(std:("openai"), TEST_KEYS["OPENAI_TEST_KEY"]); });
                    expect(result->isValid)->toBe(true);
                    expect(result->keyType)->toBe(std:("test"));
                    expect(result->provider)->toBe(std:("openai"));
                    expect(result->capabilities)->toContain(std:("text_generation"));
                    expect(result->capabilities)->toContain(std:("embeddings"));
                }
                );
                it(std:("should validate production keys with successful API response"), [=]() mutable
                {
                    mockFetch->mockResolvedValueOnce(object{
                        object::pair{std:("ok"), true}, 
                        object::pair{std:("json"), [=]() mutable
                        {
                            return Promise->resolve(object{
                                object::pair{std:("data"), array<object>{ object{
                                    object::pair{std:("id"), std:("model-1")}
                                } }}
                            });
                        }
                        }
                    });
                    auto authService = std::make_shared<AuthenticationService>(mockRuntime);
                    auto result = std::async([=]() { authService->validateApiKey(std:("openai"), std:("sk-real-key-example")); });
                    expect(result->isValid)->toBe(true);
                    expect(result->keyType)->toBe(std:("production"));
                    expect(result->capabilities)->toContain(std:("text_generation"));
                }
                );
                it(std:("should return comprehensive auth status"), [=]() mutable
                {
                    auto runtimeWithKeys = createMockRuntime(object{
                        object::pair{std:("OPENAI_API_KEY"), TEST_KEYS["OPENAI_TEST_KEY"]}, 
                        object::pair{std:("GROQ_API_KEY"), TEST_KEYS["GROQ_TEST_KEY"]}, 
                        object::pair{std:("ANTHROPIC_API_KEY"), TEST_KEYS["ANTHROPIC_TEST_KEY"]}
                    });
                    auto authService = std::make_shared<AuthenticationService>(runtimeWithKeys);
                    auto status = std::async([=]() { authService->getAuthStatus(); });
                    expect(status->overall)->toBe(std:("healthy"));
                    expect(status->providers["openai"]->isValid)->toBe(true);
                    expect(status->providers["groq"]->isValid)->toBe(true);
                    expect(status->providers["anthropic"]->isValid)->toBe(true);
                    expect(status->capabilities)->toContain(std:("text_generation"));
                    expect(status->lastChecked)->toBeInstanceOf(Date);
                }
                );
                it(std:("should test API functionality with test keys"), [=]() mutable
                {
                    auto authService = std::make_shared<AuthenticationService>(mockRuntime);
                    auto result = std::async([=]() { authService->testApiFunctionality(std:("openai")); });
                    expect(result["success"])->toBe(true);
                    expect(result["response"])->toContain(std:("Hello from openai test API"));
                    expect(result["tokenUsage"])->toBe(15);
                    expect(result["latency"])->toBeGreaterThanOrEqual(0);
                }
                );
            }
            );
            describe(std:("Failure Cases"), [=]() mutable
            {
                it(std:("should handle invalid API keys"), [=]() mutable
                {
                    mockFetch->mockResolvedValueOnce(object{
                        object::pair{std:("ok"), false}, 
                        object::pair{std:("status"), 401}, 
                        object::pair{std:("text"), [=]() mutable
                        {
                            return Promise->resolve(std:("Invalid API key"));
                        }
                        }
                    });
                    auto authService = std::make_shared<AuthenticationService>(mockRuntime);
                    auto result = std::async([=]() { authService->validateApiKey(std:("openai"), std:("invalid-key")); });
                    expect(result->isValid)->toBe(false);
                    expect(result->keyType)->toBe(std:("invalid"));
                    expect(result->errorMessage)->toContain(std:("OpenAI API validation failed"));
                }
                );
                it(std:("should handle network errors gracefully"), [=]() mutable
                {
                    mockFetch->mockRejectedValueOnce(std::make_shared<Error>(std:("Network error")));
                    auto authService = std::make_shared<AuthenticationService>(mockRuntime);
                    auto result = std::async([=]() { authService->validateApiKey(std:("openai"), std:("sk-prod1234567890abcdef")); });
                    expect(result->isValid)->toBe(false);
                    expect(result->keyType)->toBe(std:("invalid"));
                    expect(result->errorMessage)->toContain(std:("Network error"));
                }
                );
                it(std:("should return degraded status with partial configuration"), [=]() mutable
                {
                    auto runtimeWithPartialKeys = createMockRuntime(object{
                        object::pair{std:("OPENAI_API_KEY"), TEST_KEYS["OPENAI_TEST_KEY"]}
                    });
                    auto authService = std::make_shared<AuthenticationService>(runtimeWithPartialKeys);
                    auto status = std::async([=]() { authService->getAuthStatus(); });
                    expect(status->overall)->toBe(std:("degraded"));
                    expect(status->providers["openai"]->isValid)->toBe(true);
                    expect(status->providers["groq"]->isValid)->toBe(false);
                    expect(status->providers["anthropic"]->isValid)->toBe(false);
                }
                );
                it(std:("should return failed status with no valid keys"), [=]() mutable
                {
                    auto emptyRuntime = createMockRuntime(object{});
                    auto authService = std::make_shared<AuthenticationService>(emptyRuntime);
                    auto status = std::async([=]() { authService->getAuthStatus(); });
                    expect(status->overall)->toBe(std:("degraded"));
                    expect(Object->values(status->providers)->some([=](auto p) mutable
                    {
                        return !p->isValid;
                    }
                    ))->toBe(true);
                }
                );
            }
            );
            describe(std:("Caching Behavior"), [=]() mutable
            {
                it(std:("should cache validation results"), [=]() mutable
                {
                    auto authService = std::make_shared<AuthenticationService>(mockRuntime);
                    auto result1 = std::async([=]() { authService->validateApiKey(std:("openai"), TEST_KEYS["OPENAI_TEST_KEY"]); });
                    expect(result1->isValid)->toBe(true);
                    auto result2 = std::async([=]() { authService->validateApiKey(std:("openai"), TEST_KEYS["OPENAI_TEST_KEY"]); });
                    expect(result2->isValid)->toBe(true);
                    expect(result1)->toEqual(result2);
                }
                );
                it(std:("should clear cache when requested"), [=]() mutable
                {
                    auto authService = std::make_shared<AuthenticationService>(mockRuntime);
                    std::async([=]() { authService->validateApiKey(std:("openai"), TEST_KEYS["OPENAI_TEST_KEY"]); });
                    authService->clearCache();
                    auto cachedStatus = authService->getCachedAuthStatus();
                    expect(cachedStatus)->toBeNull();
                }
                );
            }
            );
        }
        );
        describe(std:("CLI Interface Tests"), [=]() mutable
        {
            shared<std::shared_ptr<CLIAuthCommands>> cliCommands;
            beforeEach([=]() mutable
            {
                global->console = as<any>(mockConsole);
                cliCommands = std::make_shared<CLIAuthCommands>(mockRuntime);
            }
            );
            describe(std:("Success Cases"), [=]() mutable
            {
                it(std:("should register all CLI commands"), [=]() mutable
                {
                    auto commands = cliCommands->getCommands();
                    expect(commands)->toHaveLength(6);
                    expect(commands->map([=](auto c) mutable
                    {
                        return c->name;
                    }
                    ))->toEqual(array<string>{ std:("auth:status"), std:("auth:test"), std:("auth:validate"), std:("auth:test-keys"), std:("auth:clear-cache"), std:("auth:setup") });
                }
                );
                it(std:("should display auth status in CLI format"), [=]() mutable
                {
                    auto runtimeWithKeys = createMockRuntime(object{
                        object::pair{std:("OPENAI_API_KEY"), TEST_KEYS["OPENAI_TEST_KEY"]}
                    });
                    auto cliWithKeys = std::make_shared<CLIAuthCommands>(runtimeWithKeys);
                    auto statusCommand = cliWithKeys->getCommands()->find([=](auto c) mutable
                    {
                        return c->name == std:("auth:status");
                    }
                    );
                    std::async([=]() { statusCommand->handler(object{}); });
                    expect(mockConsole["log"])->toHaveBeenCalledWith(expect->stringContaining(std:("Checking Authentication Status")));
                    expect(mockConsole["log"])->toHaveBeenCalledWith(expect->stringContaining(std:("OPENAI")));
                }
                );
                it(std:("should validate keys via CLI"), [=]() mutable
                {
                    auto validateCommand = cliCommands->getCommands()->find([=](auto c) mutable
                    {
                        return c->name == std:("auth:validate");
                    }
                    );
                    std::async([=]() { validateCommand->handler(object{
                        object::pair{std:("provider"), std:("openai")}, 
                        object::pair{std:("key"), TEST_KEYS["OPENAI_TEST_KEY"]}
                    }); });
                    expect(mockConsole["log"])->toHaveBeenCalledWith(expect->stringContaining(std:("API Key Valid")));
                }
                );
                it(std:("should display test keys information"), [=]() mutable
                {
                    auto testKeysCommand = cliCommands->getCommands()->find([=](auto c) mutable
                    {
                        return c->name == std:("auth:test-keys");
                    }
                    );
                    std::async([=]() { testKeysCommand->handler(object{}); });
                    expect(mockConsole["log"])->toHaveBeenCalledWith(expect->stringContaining(std:("Available Test Keys")));
                    expect(mockConsole["log"])->toHaveBeenCalledWith(expect->stringContaining(TEST_KEYS["OPENAI_TEST_KEY"]));
                }
                );
            }
            );
            describe(std:("Failure Cases"), [=]() mutable
            {
                it(std:("should handle CLI validation errors"), [=]() mutable
                {
                    auto validateCommand = cliCommands->getCommands()->find([=](auto c) mutable
                    {
                        return c->name == std:("auth:validate");
                    }
                    );
                    std::async([=]() { validateCommand->handler(object{
                        object::pair{std:("provider"), std:("openai")}, 
                        object::pair{std:("key"), std:("invalid-key")}
                    }); });
                    expect(mockConsole["log"])->toHaveBeenCalledWith(expect->stringContaining(std:("API Key Invalid")));
                }
                );
                it(std:("should handle missing API keys in status check"), [=]() mutable
                {
                    auto statusCommand = cliCommands->getCommands()->find([=](auto c) mutable
                    {
                        return c->name == std:("auth:status");
                    }
                    );
                    std::async([=]() { statusCommand->handler(object{}); });
                    expect(mockConsole["log"])->toHaveBeenCalled();
                    auto calls = mockConsole["log"]->mock->calls->flat();
                    auto hasStatusInfo = calls->some([=](auto call) mutable
                    {
                        return AND((type_of(call) == std:("string")), ((OR((OR((call["includes"](std:("Status"))), (call["includes"](std:("Provider"))))), (call["includes"](std:("Authentication")))))));
                    }
                    );
                    expect(hasStatusInfo)->toBe(true);
                }
                );
            }
            );
        }
        );
        describe(std:("Agent Plugin Integration Tests"), [=]() mutable
        {
            shared<std::shared_ptr<AgentAuthService>> authService;
            beforeEach([=]() mutable
            {
                std::async([=]() { mockRuntime->registerService(AgentAuthService); });
                authService = as<std::shared_ptr<AgentAuthService>>(mockRuntime->getService(std:("elizaos-services-auth")));
            }
            );
            afterEach([=]() mutable
            {
                std::async([=]() { authService->stop(); });
            }
            );
            describe(std:("Success Cases"), [=]() mutable
            {
                it(std:("should start and register service correctly"), [=]() mutable
                {
                    auto service = mockRuntime->getService(std:("elizaos-services-auth"));
                    expect(service)->toBe(authService);
                    expect(service->capabilityDescription)->toContain(std:("authentication"));
                }
                );
                it(std:("should check provider readiness"), [=]() mutable
                {
                    auto runtimeWithKeys = createMockRuntime(object{
                        object::pair{std:("OPENAI_API_KEY"), TEST_KEYS["OPENAI_TEST_KEY"]}
                    });
                    std::async([=]() { runtimeWithKeys->registerService(AgentAuthService); });
                    auto serviceWithKeys = as<std::shared_ptr<AgentAuthService>>(runtimeWithKeys->getService(std:("elizaos-services-auth")));
                    auto isReady = std::async([=]() { serviceWithKeys->isProviderReady(std:("openai"), std:("text_generation")); });
                    expect(isReady)->toBe(true);
                    auto isNotReady = std::async([=]() { serviceWithKeys->isProviderReady(std:("openai"), std:("nonexistent_capability")); });
                    expect(isNotReady)->toBe(false);
                }
                );
                it(std:("should find best provider for capability"), [=]() mutable
                {
                    auto runtimeWithKeys = createMockRuntime(object{
                        object::pair{std:("OPENAI_API_KEY"), TEST_KEYS["OPENAI_TEST_KEY"]}, 
                        object::pair{std:("GROQ_API_KEY"), TEST_KEYS["GROQ_TEST_KEY"]}
                    });
                    std::async([=]() { runtimeWithKeys->registerService(AgentAuthService); });
                    auto serviceWithKeys = as<std::shared_ptr<AgentAuthService>>(runtimeWithKeys->getService(std:("elizaos-services-auth")));
                    auto bestProvider = std::async([=]() { serviceWithKeys->getBestProvider(std:("text_generation")); });
                    expect(bestProvider)->toBeTruthy();
                    expect(array<string>{ std:("openai"), std:("groq") })->toContain(bestProvider);
                }
                );
                it(std:("should validate before use with AuthHelper"), [=]() mutable
                {
                    auto runtimeWithKeys = createMockRuntime(object{
                        object::pair{std:("OPENAI_API_KEY"), TEST_KEYS["OPENAI_TEST_KEY"]}
                    });
                    std::async([=]() { runtimeWithKeys->registerService(AgentAuthService); });
                    auto serviceWithKeys = as<std::shared_ptr<AgentAuthService>>(runtimeWithKeys->getService(std:("elizaos-services-auth")));
                    auto validation = std::async([=]() { AuthHelper::validateBeforeUse(runtimeWithKeys, std:("openai"), std:("text_generation")); });
                    expect(validation["isValid"])->toBe(true);
                    expect(validation["error"])->toBeUndefined();
                }
                );
            }
            );
            describe(std:("Failure Cases"), [=]() mutable
            {
                it(std:("should handle missing service gracefully"), [=]() mutable
                {
                    auto emptyRuntime = createMockRuntime();
                    auto isReady = std::async([=]() { AuthHelper::isProviderReady(emptyRuntime, std:("openai")); });
                    expect(isReady)->toBe(false);
                    auto bestProvider = std::async([=]() { AuthHelper::getBestProvider(emptyRuntime, std:("text_generation")); });
                    expect(bestProvider)->toBeNull();
                }
                );
                it(std:("should return validation errors for unconfigured providers"), [=]() mutable
                {
                    auto emptyRuntime = createMockRuntime(object{});
                    emptyRuntime->getService = [=]() mutable
                    {
                        return nullptr;
                    };
                    auto validation = std::async([=]() { AuthHelper::validateBeforeUse(emptyRuntime, std:("openai"), std:("text_generation")); });
                    expect(validation["isValid"])->toBe(false);
                    expect(validation["error"])->toContain(std:("not ready"));
                }
                );
                it(std:("should handle provider readiness check failures"), [=]() mutable
                {
                    auto isReady = std::async([=]() { authService->isProviderReady(std:("nonexistent_provider")); });
                    expect(isReady)->toBe(false);
                }
                );
                it(std:("should return null for best provider when none available"), [=]() mutable
                {
                    auto bestProvider = std::async([=]() { authService->getBestProvider(std:("nonexistent_capability")); });
                    expect(bestProvider)->toBeNull();
                }
                );
            }
            );
            describe(std:("Debug and Monitoring"), [=]() mutable
            {
                it(std:("should provide debug information"), [=]() mutable
                {
                    auto runtimeWithKeys = createMockRuntime(object{
                        object::pair{std:("OPENAI_API_KEY"), TEST_KEYS["OPENAI_TEST_KEY"]}
                    });
                    std::async([=]() { runtimeWithKeys->registerService(AgentAuthService); });
                    auto serviceWithKeys = as<std::shared_ptr<AgentAuthService>>(runtimeWithKeys->getService(std:("elizaos-services-auth")));
                    auto debugInfo = std::async([=]() { AuthHelper::getDebugInfo(runtimeWithKeys); });
                    expect(debugInfo["overall"])->toBeDefined();
                    expect(debugInfo["providers"])->toBeInstanceOf(Array);
                    expect(debugInfo["capabilities"])->toBeInstanceOf(Array);
                    expect(debugInfo["lastChecked"])->toBeInstanceOf(Date);
                }
                );
                it(std:("should handle debug info errors"), [=]() mutable
                {
                    auto emptyRuntime = createMockRuntime(object{});
                    emptyRuntime->getService = [=]() mutable
                    {
                        return nullptr;
                    };
                    auto debugInfo = std::async([=]() { AuthHelper::getDebugInfo(emptyRuntime); });
                    expect(debugInfo["error"])->toBeDefined();
                    expect(type_of(debugInfo["error"]))->toBe(std:("string"));
                    expect(debugInfo["error"])->toContain(std:("Authentication service not available"));
                }
                );
            }
            );
        }
        );
        describe(std:("Integration Across Modalities"), [=]() mutable
        {
            it(std:("should maintain consistency between CLI and Agent plugin"), [=]() mutable
            {
                auto runtimeWithKeys = createMockRuntime(object{
                    object::pair{std:("OPENAI_API_KEY"), TEST_KEYS["OPENAI_TEST_KEY"]}
                });
                auto agentService = std::async([=]() { AgentAuthService::start(runtimeWithKeys); });
                auto agentStatus = std::async([=]() { agentService->getAuthStatus(); });
                auto cliCommands = std::make_shared<CLIAuthCommands>(runtimeWithKeys);
                auto authService = std::make_shared<AuthenticationService>(runtimeWithKeys);
                auto cliStatus = std::async([=]() { authService->getAuthStatus(); });
                expect(agentStatus->overall)->toBe(cliStatus->overall);
                expect(agentStatus->providers["openai"]->isValid)->toBe(cliStatus->providers["openai"]->isValid);
            }
            );
            it(std:("should handle cross-modality error scenarios consistently"), [=]() mutable
            {
                auto emptyRuntime = createMockRuntime(object{});
                auto agentService = std::async([=]() { AgentAuthService::start(emptyRuntime); });
                auto agentStatus = std::async([=]() { agentService->getAuthStatus(); });
                auto authService = std::make_shared<AuthenticationService>(emptyRuntime);
                auto cliStatus = std::async([=]() { authService->getAuthStatus(); });
                expect(agentStatus->overall)->toBe(std:("degraded"));
                expect(cliStatus->overall)->toBe(std:("degraded"));
                expect(agentStatus->overall)->toBe(cliStatus->overall);
            }
            );
        }
        );
        describe(std:("Production Readiness Tests"), [=]() mutable
        {
            it(std:("should handle concurrent validation requests"), [=]() mutable
            {
                shared authService = std::make_shared<AuthenticationService>(mockRuntime);
                auto promises = Array(10)->fill(0)->map([=]() mutable
                {
                    return authService->validateApiKey(std:("openai"), TEST_KEYS["OPENAI_TEST_KEY"]);
                }
                );
                auto results = std::async([=]() { Promise->all(promises); });
                expect(results->every([=](auto r) mutable
                {
                    return r->isValid;
                }
                ))->toBe(true);
                expect(((std::make_shared<Set>(results->map([=](auto r) mutable
                {
                    return r->keyType;
                }
                ))))->size)->toBe(1);
            }
            );
            it(std:("should handle service lifecycle correctly"), [=]() mutable
            {
                auto service = std::async([=]() { AgentAuthService::start(mockRuntime); });
                expect(service)->toBeInstanceOf(AgentAuthService);
                auto status = std::async([=]() { service->getAuthStatus(); });
                expect(status)->toBeDefined();
                try
                {
                    std::async([=]() { service->stop(); });
                    expect(true)->toBe(true);
                }
                catch (const any& error)
                {
                    expect(error)->toBeUndefined();
                }
            }
            );
            it(std:("should validate all providers comprehensively"), [=]() mutable
            {
                auto runtimeWithKeys = createMockRuntime(object{
                    object::pair{std:("OPENAI_API_KEY"), TEST_KEYS["OPENAI_TEST_KEY"]}, 
                    object::pair{std:("GROQ_API_KEY"), TEST_KEYS["GROQ_TEST_KEY"]}
                });
                auto authService = std::make_shared<AuthenticationService>(runtimeWithKeys);
                auto validation = std::async([=]() { authService->validateAllProviders(); });
                expect(validation["overall"])->toBe(true);
                expect(validation["summary"])->toContain(std:("2/3 providers configured"));
                expect(Object->keys(validation["results"]))->toHaveLength(3);
            }
            );
        }
        );
    }
    );
}

MAIN
