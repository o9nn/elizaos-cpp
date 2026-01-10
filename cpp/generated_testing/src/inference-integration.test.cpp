#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/__tests__/inference-integration.test.h"

void Main(void)
{
    describe(std::string("Plugin Inference Integration Tests"), [=]() mutable
    {
        shared<std::shared_ptr<AgentServer>> server;
        shared<std::shared_ptr<IAgentRuntime>> runtime;
        shared<double> port;
        beforeAll([=]() mutable
        {
            port = 3001;
            server = std::make_shared<AgentServer>();
            std::async([=]() { server->initialize(OR((process->env->POSTGRES_URL), (std::string("postgresql://localhost/test")))); });
            runtime = std::async([=]() { startAgent(terminalCharacter); });
            std::async([=]() { server->registerAgent(runtime); });
            std::async([=]() { server->start(port); });
            console->log(std::string("Test server started on port ") + port + string_empty);
        }
        , 30000);
        afterAll([=]() mutable
        {
            if (server) {
                std::async([=]() { server->stop(); });
            }
        }
        );
        describe(std::string("Provider Management API"), [=]() mutable
        {
            it(std::string("should get provider status"), [=]() mutable
            {
                auto response = std::async([=]() { fetch(std::string("http://localhost:") + port + std::string("/api/providers")); });
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
                expect(providerNames)->toContain(std::string("ollama"));
                expect(providerNames)->toContain(std::string("openai"));
                expect(providerNames)->toContain(std::string("anthropic"));
                expect(providerNames)->toContain(std::string("elizaos"));
            }
            );
            it(std::string("should set selected provider"), [=]() mutable
            {
                auto response = std::async([=]() { fetch(std::string("http://localhost:") + port + std::string("/api/providers/selected"), object{
                    object::pair{std::string("method"), std::string("PUT")}, 
                    object::pair{std::string("headers"), object{
                        object::pair{std::string("Content-Type"), std::string("application/json")}
                    }}, 
                    object::pair{std::string("body"), JSON->stringify(object{
                        object::pair{std::string("provider"), std::string("ollama")}
                    })}
                }); });
                expect(response->ok)->toBe(true);
                auto data = std::async([=]() { response->json(); });
                expect(data["success"])->toBe(true);
            }
            );
            it(std::string("should set provider preferences"), [=]() mutable
            {
                auto response = std::async([=]() { fetch(std::string("http://localhost:") + port + std::string("/api/providers/preferences"), object{
                    object::pair{std::string("method"), std::string("PUT")}, 
                    object::pair{std::string("headers"), object{
                        object::pair{std::string("Content-Type"), std::string("application/json")}
                    }}, 
                    object::pair{std::string("body"), JSON->stringify(object{
                        object::pair{std::string("preferences"), array<string>{ std::string("ollama"), std::string("openai"), std::string("anthropic"), std::string("elizaos") }}
                    })}
                }); });
                expect(response->ok)->toBe(true);
                auto data = std::async([=]() { response->json(); });
                expect(data["success"])->toBe(true);
            }
            );
            it(std::string("should verify ollama is the default active provider"), [=]() mutable
            {
                auto response = std::async([=]() { fetch(std::string("http://localhost:") + port + std::string("/api/providers")); });
                auto data = std::async([=]() { response->json(); });
                auto ollamaProvider = data["data"]["providers"]["find"]([=](auto p) mutable
                {
                    return p["name"] == std::string("ollama");
                }
                );
                expect(ollamaProvider)->toBeDefined();
                if (ollamaProvider["status"] == std::string("available")) {
                    expect(data["data"]["active"])->toBe(std::string("ollama"));
                }
            }
            );
        }
        );
        describe(std::string("Message Routing"), [=]() mutable
        {
            it(std::string("should route messages through the inference plugin"), [=]() mutable
            {
                auto testMessage = object{
                    object::pair{std::string("channelId"), std::string("test-channel")}, 
                    object::pair{std::string("agentId"), runtime->agentId}, 
                    object::pair{std::string("content"), std::string("Hello, this is a test message")}, 
                    object::pair{std::string("author"), std::string("test-user")}, 
                    object::pair{std::string("metadata"), object{}}
                };
                auto response = std::async([=]() { runtime->generateResponse(object{
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), testMessage["content"]}
                    }}, 
                    object::pair{std::string("agentId"), runtime->agentId}, 
                    object::pair{std::string("userId"), std::string("test-user-id")}, 
                    object::pair{std::string("roomId"), std::string("test-room-id")}
                }); });
                expect(response)->toBeDefined();
                expect(response->length)->toBeGreaterThan(0);
                console->log(std::string("Response generated through inference plugin:"), const_(response)[0]->content);
            }
            );
        }
        );
    }
    );
}

MAIN
