#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/__tests__/inference-integration.test.h"

void Main(void)
{
    describe(std:("Plugin Inference Integration Tests"), [=]() mutable
    {
        shared<std::shared_ptr<AgentServer>> server;
        shared<std::shared_ptr<IAgentRuntime>> runtime;
        shared<double> port;
        beforeAll([=]() mutable
        {
            port = 3001;
            server = std::make_shared<AgentServer>();
            std::async([=]() { server->initialize(OR((process->env->POSTGRES_URL), (std:("postgresql://localhost/test")))); });
            runtime = std::async([=]() { startAgent(terminalCharacter); });
            std::async([=]() { server->registerAgent(runtime); });
            std::async([=]() { server->start(port); });
            console->log(std:("Test server started on port ") + port + string_empty);
        }
        , 30000);
        afterAll([=]() mutable
        {
            if (server) {
                std::async([=]() { server->stop(); });
            }
        }
        );
        describe(std:("Provider Management API"), [=]() mutable
        {
            it(std:("should get provider status"), [=]() mutable
            {
                auto response = std::async([=]() { fetch(std:("http://localhost:") + port + std:("/api/providers")); });
                expect(response->ok)->toBe(true);
                auto data = std::async([=]() { response->json(); });
                expect(data["success"])->toBe(true);
                expect(data["data"]["providers"])->toBeInstanceOf(Array);
                expect(data["data"]["providers"]["length"])->toBeGreaterThan(0);
                auto providerNames = data["data"]["providers"]["map"]([=](auto p) mutable
                {
                    return p["name"];
                }
                );
                expect(providerNames)->toContain(std:("ollama"));
                expect(providerNames)->toContain(std:("openai"));
                expect(providerNames)->toContain(std:("anthropic"));
                expect(providerNames)->toContain(std:("elizaos"));
            }
            );
            it(std:("should set selected provider"), [=]() mutable
            {
                auto response = std::async([=]() { fetch(std:("http://localhost:") + port + std:("/api/providers/selected"), object{
                    object::pair{std:("method"), std:("PUT")}, 
                    object::pair{std:("headers"), object{
                        object::pair{std:("Content-Type"), std:("application/json")}
                    }}, 
                    object::pair{std:("body"), JSON->stringify(object{
                        object::pair{std:("provider"), std:("ollama")}
                    })}
                }); });
                expect(response->ok)->toBe(true);
                auto data = std::async([=]() { response->json(); });
                expect(data["success"])->toBe(true);
            }
            );
            it(std:("should set provider preferences"), [=]() mutable
            {
                auto response = std::async([=]() { fetch(std:("http://localhost:") + port + std:("/api/providers/preferences"), object{
                    object::pair{std:("method"), std:("PUT")}, 
                    object::pair{std:("headers"), object{
                        object::pair{std:("Content-Type"), std:("application/json")}
                    }}, 
                    object::pair{std:("body"), JSON->stringify(object{
                        object::pair{std:("preferences"), array<string>{ std:("ollama"), std:("openai"), std:("anthropic"), std:("elizaos") }}
                    })}
                }); });
                expect(response->ok)->toBe(true);
                auto data = std::async([=]() { response->json(); });
                expect(data["success"])->toBe(true);
            }
            );
            it(std:("should verify ollama is the default active provider"), [=]() mutable
            {
                auto response = std::async([=]() { fetch(std:("http://localhost:") + port + std:("/api/providers")); });
                auto data = std::async([=]() { response->json(); });
                auto ollamaProvider = data["data"]["providers"]["find"]([=](auto p) mutable
                {
                    return p["name"] == std:("ollama");
                }
                );
                expect(ollamaProvider)->toBeDefined();
                if (ollamaProvider["status"] == std:("available")) {
                    expect(data["data"]["active"])->toBe(std:("ollama"));
                }
            }
            );
        }
        );
        describe(std:("Message Routing"), [=]() mutable
        {
            it(std:("should route messages through the inference plugin"), [=]() mutable
            {
                auto testMessage = object{
                    object::pair{std:("channelId"), std:("test-channel")}, 
                    object::pair{std:("agentId"), runtime->agentId}, 
                    object::pair{std:("content"), std:("Hello, this is a test message")}, 
                    object::pair{std:("author"), std:("test-user")}, 
                    object::pair{std:("metadata"), object{}}
                };
                auto response = std::async([=]() { runtime->generateResponse(object{
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), testMessage["content"]}
                    }}, 
                    object::pair{std:("agentId"), runtime->agentId}, 
                    object::pair{std:("userId"), std:("test-user-id")}, 
                    object::pair{std:("roomId"), std:("test-room-id")}
                }); });
                expect(response)->toBeDefined();
                expect(response->length)->toBeGreaterThan(0);
                console->log(std:("Response generated through inference plugin:"), const_(response)[0]->content);
            }
            );
        }
        );
    }
    );
}

MAIN
