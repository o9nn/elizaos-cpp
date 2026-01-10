#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/api-client/src/__tests__/client.test.h"

void Main(void)
{
    describe(std::string("ElizaClient"), [=]() mutable
    {
        shared config = object{
            object::pair{std::string("baseUrl"), std::string("http://localhost:3000")}, 
            object::pair{std::string("apiKey"), std::string("test-key")}
        };
        it(std::string("should create client with all services"), [=]() mutable
        {
            auto client = std::make_shared<ElizaClient>(config);
            expect(client->agents)->toBeInstanceOf(AgentsService);
            expect(client->messaging)->toBeInstanceOf(MessagingService);
            expect(client->memory)->toBeInstanceOf(MemoryService);
            expect(client->audio)->toBeInstanceOf(AudioService);
            expect(client->media)->toBeInstanceOf(MediaService);
            expect(client->server)->toBeInstanceOf(ServerService);
            expect(client->system)->toBeInstanceOf(SystemService);
        }
        );
        it(std::string("should create client using static create method"), [=]() mutable
        {
            auto client = ElizaClient::create(config);
            expect(client)->toBeInstanceOf(ElizaClient);
            expect(client->agents)->toBeInstanceOf(AgentsService);
        }
        );
        it(std::string("should pass config to all services"), [=]() mutable
        {
            auto client = std::make_shared<ElizaClient>(config);
            expect(client->agents)->toBeDefined();
            expect(client->messaging)->toBeDefined();
            expect(client->memory)->toBeDefined();
            expect(client->audio)->toBeDefined();
            expect(client->media)->toBeDefined();
            expect(client->server)->toBeDefined();
            expect(client->system)->toBeDefined();
        }
        );
    }
    );
}

MAIN
