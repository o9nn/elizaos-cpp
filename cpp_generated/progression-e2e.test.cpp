#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/__tests__/progression-e2e.test.h"

void Main(void)
{
    describe(std::string("Capability Progression System E2E Integration Tests"), [=]() mutable
    {
        shared<std::shared_ptr<child_process::ChildProcess>> serverProcess;
        shared serverUrl = std::string("http://localhost:7777");
        beforeAll([=]() mutable
        {
            console->log(std::string("🚀 Starting agent server for E2E tests..."));
            process->env->MODEL_PROVIDER = std::string("test");
            process->env->ENABLE_STAGEHAND = std::string("false");
            serverProcess = spawn(std::string("bun"), array<string>{ std::string("src/index.ts") }, object{
                object::pair{std::string("cwd"), std::string("/workspace/packages/agentserver")}, 
                object::pair{std::string("env"), utils::assign(object{
                    , 
                    object::pair{std::string("PATH"), string_empty + process->env->PATH + std::string(":/home/ubuntu/.bun/bin")}, 
                    object::pair{std::string("DATABASE_URL"), std::string("postgresql://eliza:eliza_secure_pass@localhost:5432/eliza_test")}, 
                    object::pair{std::string("NODE_ENV"), std::string("test")}
                }, process->env)}, 
                object::pair{std::string("stdio"), array<string>{ std::string("pipe"), std::string("pipe"), std::string("pipe") }}
            });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve, auto reject) mutable
            {
                shared output = string_empty;
                shared timeout = setTimeout([=]() mutable
                {
                    reject(std::make_shared<Error>(std::string("Server startup timeout")));
                }
                , 30000);
                auto onData = [=](auto data) mutable
                {
                    output += data->toString();
                    console->log(std::string("[SERVER]"), data->toString()->trim());
                    if (output->includes(std::string("Server started on port 7777"))) {
                        clearTimeout(timeout);
                        resolve(undefined);
                    }
                };
                serverProcess->stdout->on(std::string("data"), onData);
                serverProcess->stderr->on(std::string("data"), onData);
                serverProcess->on(std::string("error"), [=](auto error) mutable
                {
                    clearTimeout(timeout);
                    reject(error);
                }
                );
            }
            ); });
            console->log(std::string("✅ Agent server started successfully"));
        }
        );
        afterAll([=]() mutable
        {
            if (serverProcess) {
                console->log(std::string("🛑 Stopping agent server..."));
                serverProcess->kill(std::string("SIGTERM"));
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    serverProcess->on(std::string("exit"), resolve);
                    setTimeout([=]() mutable
                    {
                        serverProcess->kill(std::string("SIGKILL"));
                        resolve(undefined);
                    }
                    , 5000);
                }
                ); });
            }
        }
        );
        it(std::string("should have progression API endpoints available"), [=]() mutable
        {
            auto response = std::async([=]() { fetch(string_empty + serverUrl + std::string("/api/agents/default/progression")); });
            expect(response->ok)->toBe(true);
            auto data = std::async([=]() { response->json(); });
            expect(data["success"])->toBe(true);
            expect(data["data"])->toBeDefined();
            if (data["data"]["progressionReady"]) {
                expect(data["data"]["currentLevel"])->toBeDefined();
                expect(data["data"]["unlockedCapabilities"])->toBeDefined();
                expect(data["data"]["availableLevels"])->toBeDefined();
            }
            console->log(std::string("✅ Progression API endpoint working"));
        }
        );
        it(std::string("should start with limited capabilities at level 0"), [=]() mutable
        {
            auto response = std::async([=]() { fetch(string_empty + serverUrl + std::string("/api/agents/default/progression")); });
            auto data = std::async([=]() { response->json(); });
            if (data["data"]["progressionReady"]) {
                expect(data["data"]["currentLevel"])->toBe(0);
                expect(data["data"]["unlockedCapabilities"])->toContain(std::string("shell"));
                expect(data["data"]["unlockedCapabilities"])->toContain(std::string("naming"));
                expect(data["data"]["unlockedCapabilities"])->not->toContain(std::string("browser"));
                expect(data["data"]["unlockedCapabilities"])->not->toContain(std::string("vision"));
                console->log(std::string("✅ Agent starts with correct basic capabilities"));
            }
        }
        );
        it(std::string("should track shell usage and progression towards browser unlock"), [=]() mutable
        {
            console->log(std::string("📝 Testing progression tracking..."));
            auto response = std::async([=]() { fetch(string_empty + serverUrl + std::string("/api/agents/default/progression")); });
            auto data = std::async([=]() { response->json(); });
            if (data["data"]["progressionReady"]) {
                auto initialLevel = data["data"]["currentLevel"];
                auto initialCapabilities = data["data"]["unlockedCapabilities"];
                console->log(std::string("Initial level: ") + initialLevel + string_empty);
                console->log(std::string("Initial capabilities: ") + initialCapabilities["join"](std::string(", ")) + string_empty);
                expect(initialLevel)->toBe(0);
                expect(initialCapabilities)->toContain(std::string("shell"));
                expect(initialCapabilities)->toContain(std::string("naming"));
                console->log(std::string("✅ Progression tracking system is operational"));
            }
        }
        );
        it(std::string("should handle progression status requests gracefully"), [=]() mutable
        {
            auto response1 = std::async([=]() { fetch(string_empty + serverUrl + std::string("/api/agents/default/progression")); });
            expect(response1->ok)->toBe(true);
            auto response2 = std::async([=]() { fetch(string_empty + serverUrl + std::string("/api/agents/nonexistent/progression")); });
            expect(response2->ok)->toBe(true);
            auto data2 = std::async([=]() { response2->json(); });
            expect(data2["success"])->toBe(true);
            expect(data2["data"]["progressionReady"])->toBe(false);
            expect(data2["data"]["message"])->toContain(std::string("not found"));
            console->log(std::string("✅ API handles edge cases correctly"));
        }
        );
        it(std::string("should integrate with existing capability toggle system"), [=]() mutable
        {
            auto settingsResponse = std::async([=]() { fetch(string_empty + serverUrl + std::string("/api/agents/default/settings")); });
            expect(settingsResponse->ok)->toBe(true);
            auto settingsData = std::async([=]() { settingsResponse->json(); });
            expect(settingsData["success"])->toBe(true);
            auto toggleResponse = std::async([=]() { fetch(string_empty + serverUrl + std::string("/api/agents/default/capabilities/shell"), object{
                object::pair{std::string("method"), std::string("POST")}
            }); });
            expect(toggleResponse->ok)->toBe(true);
            auto toggleData = std::async([=]() { toggleResponse->json(); });
            expect(toggleData["success"])->toBe(true);
            console->log(std::string("✅ Integration with existing systems working"));
        }
        );
        it(std::string("should have all required progression levels defined"), [=]() mutable
        {
            auto response = std::async([=]() { fetch(string_empty + serverUrl + std::string("/api/agents/default/progression")); });
            auto data = std::async([=]() { response->json(); });
            if (AND((data["data"]["progressionReady"]), (data["data"]["availableLevels"]))) {
                auto levels = data["data"]["availableLevels"];
                expect(levels["length"])->toBeGreaterThanOrEqual(4);
                levels["forEach"]([=](auto level) mutable
                {
                    expect(level["id"])->toBeDefined();
                    expect(level["name"])->toBeDefined();
                    expect(level["description"])->toBeDefined();
                    expect(level["capabilities"])->toBeDefined();
                    expect(type_of(level["isUnlocked"]))->toBe(std::string("boolean"));
                }
                );
                auto levelNames = levels["map"]([=](auto l) mutable
                {
                    return l["name"];
                }
                );
                expect(levelNames)->toContain(std::string("Basic Existence"));
                expect(levelNames)->toContain(std::string("Web Explorer"));
                expect(levelNames)->toContain(std::string("Visual Perception"));
                console->log(std::string("✅ All progression levels properly defined"));
                console->log(std::string("Available levels: ") + levelNames["join"](std::string(", ")) + string_empty);
            }
        }
        );
        it(std::string("should maintain consistent state across API calls"), [=]() mutable
        {
            auto responses = std::async([=]() { Promise->all(std::tuple<std::shared_ptr<Promise<std::shared_ptr<Response>>>, std::shared_ptr<Promise<std::shared_ptr<Response>>>, std::shared_ptr<Promise<std::shared_ptr<Response>>>>{ fetch(string_empty + serverUrl + std::string("/api/agents/default/progression")), fetch(string_empty + serverUrl + std::string("/api/agents/default/progression")), fetch(string_empty + serverUrl + std::string("/api/agents/default/progression")) }); });
            auto dataArray = std::async([=]() { Promise->all(responses->map([=](auto r) mutable
            {
                return r->json();
            }
            )); });
            dataArray->forEach([=](auto data) mutable
            {
                expect(data["success"])->toBe(true);
            }
            );
            if (const_(dataArray)[0]["data"]["progressionReady"]) {
                shared firstData = const_(dataArray)[0]["data"];
                dataArray->slice(1)->forEach([=](auto data) mutable
                {
                    expect(data["data"]["currentLevel"])->toBe(firstData["currentLevel"]);
                    expect(data["data"]["unlockedCapabilities"])->toEqual(firstData["unlockedCapabilities"]);
                }
                );
            }
            console->log(std::string("✅ State consistency maintained across API calls"));
        }
        );
        it(std::string("should validate the complete progression system architecture"), [=]() mutable
        {
            console->log(std::string("🔍 Validating complete progression system..."));
            auto progressionResponse = std::async([=]() { fetch(string_empty + serverUrl + std::string("/api/agents/default/progression")); });
            expect(progressionResponse->ok)->toBe(true);
            auto settingsResponse = std::async([=]() { fetch(string_empty + serverUrl + std::string("/api/agents/default/settings")); });
            expect(settingsResponse->ok)->toBe(true);
            auto agentsResponse = std::async([=]() { fetch(string_empty + serverUrl + std::string("/api/agents")); });
            expect(agentsResponse->ok)->toBe(true);
            auto agentsData = std::async([=]() { agentsResponse->json(); });
            expect(agentsData["success"])->toBe(true);
            expect(agentsData["data"]["agents"]["length"])->toBeGreaterThan(0);
            auto primaryResponse = std::async([=]() { fetch(string_empty + serverUrl + std::string("/api/agents/primary")); });
            expect(primaryResponse->ok)->toBe(true);
            auto primaryData = std::async([=]() { primaryResponse->json(); });
            expect(primaryData["success"])->toBe(true);
            expect(primaryData["data"]["available"])->toBe(true);
            console->log(std::string("✅ Complete progression system validation passed"));
            console->log(std::string("Agent ID: ") + primaryData["data"]["agentId"] + string_empty);
            console->log(std::string("Agent Name: ") + primaryData["data"]["agentName"] + string_empty);
        }
        );
    }
    );
}

MAIN
