#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/server/src/__tests__/integration/database-operations.test.h"

void Main(void)
{
    describe(std::string("Database Operations Integration Tests"), [=]() mutable
    {
        shared<std::shared_ptr<AgentServer>> agentServer;
        shared<string> testDbPath;
        beforeAll([=]() mutable
        {
            testDbPath = path->join(__dirname, std::string("test-db-ops-") + Date->now() + std::string("-") + Math->random()->toString(36)->substring(7) + string_empty);
            process->env->PGLITE_DATA_DIR = testDbPath;
            process->env.Delete("POSTGRES_URL");
            process->env.Delete("POSTGRES_PASSWORD");
            process->env.Delete("POSTGRES_USER");
            process->env.Delete("POSTGRES_HOST");
            process->env.Delete("POSTGRES_PORT");
            process->env.Delete("POSTGRES_DATABASE");
            agentServer = std::make_shared<AgentServer>();
            try
            {
                std::async([=]() { agentServer->initialize(object{
                    object::pair{std::string("dataDir"), testDbPath}
                }); });
            }
            catch (const any& error)
            {
                console->error(std::string("Failed to initialize agent server:"), error);
                if (fs->existsSync(testDbPath)) {
                    fs->rmSync(testDbPath, object{
                        object::pair{std::string("recursive"), true}, 
                        object::pair{std::string("force"), true}
                    });
                }
                throw any(error);
            }
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 1000);
            }
            ); });
        }
        );
        afterAll([=]() mutable
        {
            std::async([=]() { agentServer->stop(); });
            if (fs->existsSync(testDbPath)) {
                fs->rmSync(testDbPath, object{
                    object::pair{std::string("recursive"), true}, 
                    object::pair{std::string("force"), true}
                });
            }
        }
        );
        describe(std::string("Transaction Handling"), [=]() mutable
        {
            it(std::string("should handle concurrent message creation"), [=]() mutable
            {
                auto channelId = as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174000"));
                shared serverId = as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000000"));
                auto servers = std::async([=]() { agentServer->getServers(); });
                expect(servers->get_length())->toBeGreaterThan(0);
                expect(servers->some([=](auto s) mutable
                {
                    return s->id == serverId;
                }
                ))->toBe(true);
                std::async([=]() { agentServer->createChannel(object{
                    object::pair{std::string("id"), channelId}, 
                    object::pair{std::string("name"), std::string("Concurrent Test Channel")}, 
                    object::pair{std::string("type"), ChannelType->GROUP}, 
                    object::pair{std::string("messageServerId"), serverId}, 
                    object::pair{std::string("metadata"), object{}}
                }); });
                auto messagePromises = array<std::shared_ptr<Promise<std::shared_ptr<CentralRootMessage>>>>();
                for (auto i = 0; i < 10; i++)
                {
                    messagePromises->push(agentServer->createMessage(object{
                        object::pair{std::string("channelId"), std::string("channelId")}, 
                        object::pair{std::string("authorId"), as<std::shared_ptr<UUID>>(std::string("user-") + i + string_empty)}, 
                        object::pair{std::string("content"), std::string("Concurrent message ") + i + string_empty}, 
                        object::pair{std::string("rawMessage"), std::string("Concurrent message ") + i + string_empty}, 
                        object::pair{std::string("sourceId"), std::string("concurrent-") + i + string_empty}, 
                        object::pair{std::string("sourceType"), std::string("test")}, 
                        object::pair{std::string("metadata"), object{}}
                    }));
                }
                auto messages = std::async([=]() { Promise->all(messagePromises); });
                expect(messages)->toHaveLength(10);
                messages->forEach([=](auto msg, auto index) mutable
                {
                    expect(msg->content)->toBe(std::string("Concurrent message ") + index + string_empty);
                }
                );
                auto retrievedMessages = std::async([=]() { agentServer->getMessagesForChannel(channelId, 20); });
                expect(retrievedMessages)->toHaveLength(10);
            }
            );
            it(std::string("should maintain referential integrity"), [=]() mutable
            {
                auto channelId = as<std::shared_ptr<UUID>>(std::string("234e5678-e89b-12d3-a456-426614174000"));
                auto serverId = as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000000"));
                std::async([=]() { agentServer->createChannel(object{
                    object::pair{std::string("id"), channelId}, 
                    object::pair{std::string("name"), std::string("Integrity Test Channel")}, 
                    object::pair{std::string("type"), ChannelType->GROUP}, 
                    object::pair{std::string("messageServerId"), serverId}, 
                    object::pair{std::string("metadata"), object{}}
                }); });
                auto message1 = std::async([=]() { agentServer->createMessage(object{
                    object::pair{std::string("channelId"), std::string("channelId")}, 
                    object::pair{std::string("authorId"), as<std::shared_ptr<UUID>>(std::string("user-1"))}, 
                    object::pair{std::string("content"), std::string("First message")}, 
                    object::pair{std::string("rawMessage"), std::string("First message")}, 
                    object::pair{std::string("sourceId"), std::string("integrity-1")}, 
                    object::pair{std::string("sourceType"), std::string("test")}, 
                    object::pair{std::string("metadata"), object{}}
                }); });
                std::async([=]() { agentServer->createMessage(object{
                    object::pair{std::string("channelId"), std::string("channelId")}, 
                    object::pair{std::string("authorId"), as<std::shared_ptr<UUID>>(std::string("user-2"))}, 
                    object::pair{std::string("content"), std::string("Reply message")}, 
                    object::pair{std::string("rawMessage"), std::string("Reply message")}, 
                    object::pair{std::string("sourceId"), std::string("integrity-2")}, 
                    object::pair{std::string("sourceType"), std::string("test")}, 
                    object::pair{std::string("inReplyToRootMessageId"), message1->id}, 
                    object::pair{std::string("metadata"), object{}}
                }); });
                std::async([=]() { agentServer->deleteChannel(channelId); });
                auto deletedChannel = std::async([=]() { agentServer->getChannelDetails(channelId); });
                expect(deletedChannel)->toBeNull();
                auto messages = std::async([=]() { agentServer->getMessagesForChannel(channelId); });
                expect(messages)->toHaveLength(0);
            }
            );
        }
        );
        describe(std::string("Complex Queries"), [=]() mutable
        {
            it(std::string("should handle channel participant management"), [=]() mutable
            {
                auto serverId = as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000000"));
                auto channelId = as<std::shared_ptr<UUID>>(std::string("345e6789-e89b-12d3-a456-426614174000"));
                auto participants = array<any>{ as<std::shared_ptr<UUID>>(std::string("111e1111-e89b-12d3-a456-426614174000")), as<std::shared_ptr<UUID>>(std::string("222e2222-e89b-12d3-a456-426614174000")), as<std::shared_ptr<UUID>>(std::string("333e3333-e89b-12d3-a456-426614174000")) };
                std::async([=]() { agentServer->createChannel(object{
                    object::pair{std::string("id"), channelId}, 
                    object::pair{std::string("name"), std::string("Participant Test Channel")}, 
                    object::pair{std::string("type"), ChannelType->GROUP}, 
                    object::pair{std::string("messageServerId"), serverId}, 
                    object::pair{std::string("metadata"), object{}}
                }, participants->slice(0, 2)); });
                shared currentParticipants = std::async([=]() { agentServer->getChannelParticipants(channelId); });
                expect(currentParticipants)->toHaveLength(2);
                std::async([=]() { agentServer->addParticipantsToChannel(channelId, array<any>{ const_(participants)[2] }); });
                currentParticipants = std::async([=]() { agentServer->getChannelParticipants(channelId); });
                expect(currentParticipants)->toHaveLength(3);
                participants->forEach([=](auto p) mutable
                {
                    expect(currentParticipants)->toContain(p);
                }
                );
            }
            );
            it(std::string("should handle complex message queries with filters"), [=]() mutable
            {
                auto serverId = as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000000"));
                auto channelId = as<std::shared_ptr<UUID>>(std::string("456e7890-e89b-12d3-a456-426614174000"));
                std::async([=]() { agentServer->createChannel(object{
                    object::pair{std::string("id"), channelId}, 
                    object::pair{std::string("name"), std::string("Query Test Channel")}, 
                    object::pair{std::string("type"), ChannelType->GROUP}, 
                    object::pair{std::string("messageServerId"), serverId}, 
                    object::pair{std::string("metadata"), object{}}
                }); });
                auto baseTime = std::make_shared<Date>();
                for (auto i = 0; i < 20; i++)
                {
                    std::async([=]() { agentServer->createMessage(object{
                        object::pair{std::string("channelId"), std::string("channelId")}, 
                        object::pair{std::string("authorId"), as<std::shared_ptr<UUID>>(std::string("user-") + (i % 3) + string_empty)}, 
                        object::pair{std::string("content"), std::string("Message ") + i + std::string(" from user ") + (i % 3) + string_empty}, 
                        object::pair{std::string("rawMessage"), std::string("Message ") + i + string_empty}, 
                        object::pair{std::string("sourceId"), std::string("query-") + i + string_empty}, 
                        object::pair{std::string("sourceType"), std::string("test")}, 
                        object::pair{std::string("metadata"), object{
                            object::pair{std::string("timestamp"), ((std::make_shared<Date>(baseTime->getTime() + i * 1000)))->toISOString()}
                        }}
                    }); });
                }
                auto page1 = std::async([=]() { agentServer->getMessagesForChannel(channelId, 5); });
                expect(page1)->toHaveLength(5);
                auto page2 = std::async([=]() { agentServer->getMessagesForChannel(channelId, 5, const_(page1)[page1->get_length() - 1]->createdAt); });
                expect(page2)->toHaveLength(5);
                auto page1Ids = page1->map([=](auto m) mutable
                {
                    return m->id;
                }
                );
                shared page2Ids = page2->map([=](auto m) mutable
                {
                    return m->id;
                }
                );
                auto overlap = page1Ids->filter([=](auto id) mutable
                {
                    return page2Ids->includes(id);
                }
                );
                expect(overlap)->toHaveLength(0);
            }
            );
        }
        );
        describe(std::string("Database State Consistency"), [=]() mutable
        {
            it(std::string("should maintain consistent state across operations"), [=]() mutable
            {
                auto agentId = as<std::shared_ptr<UUID>>(std::string("consistency-agent"));
                auto initialServers = std::async([=]() { agentServer->getServers(); });
                auto initialServerCount = initialServers->get_length();
                auto newServer = std::async([=]() { agentServer->createServer(object{
                    object::pair{std::string("name"), std::string("Consistency Test Server")}, 
                    object::pair{std::string("sourceType"), std::string("consistency-test")}, 
                    object::pair{std::string("metadata"), object{}}
                }); });
                auto afterCreateServers = std::async([=]() { agentServer->getServers(); });
                expect(afterCreateServers)->toHaveLength(initialServerCount + 1);
                std::async([=]() { agentServer->addAgentToServer(newServer->id, agentId); });
                auto agentsOnServer = std::async([=]() { agentServer->getAgentsForServer(newServer->id); });
                expect(agentsOnServer)->toContain(agentId);
                shared channel = std::async([=]() { agentServer->createChannel(object{
                    object::pair{std::string("name"), std::string("Server Channel")}, 
                    object::pair{std::string("type"), ChannelType->GROUP}, 
                    object::pair{std::string("messageServerId"), newServer->id}, 
                    object::pair{std::string("metadata"), object{}}
                }); });
                auto serverChannels = std::async([=]() { agentServer->getChannelsForServer(newServer->id); });
                expect(serverChannels->some([=](auto c) mutable
                {
                    return c->id == channel->id;
                }
                ))->toBe(true);
                std::async([=]() { agentServer->removeAgentFromServer(newServer->id, agentId); });
                auto agentsAfterRemoval = std::async([=]() { agentServer->getAgentsForServer(newServer->id); });
                expect(agentsAfterRemoval)->not->toContain(agentId);
                auto channelStillExists = std::async([=]() { agentServer->getChannelDetails(channel->id); });
                expect(channelStillExists)->toBeDefined();
            }
            );
            it(std::string("should handle database connection failures gracefully"), [=]() mutable
            {
                auto invalidId = std::string("invalid-uuid-format");
                try
                {
                    std::async([=]() { agentServer->getChannelDetails(as<std::shared_ptr<UUID>>(invalidId)); });
                }
                catch (const any& error)
                {
                    expect(error)->toBeDefined();
                }
            }
            );
        }
        );
        describe(std::string("Performance and Bulk Operations"), [=]() mutable
        {
            it(std::string("should handle bulk message insertion efficiently"), [=]() mutable
            {
                auto serverId = as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000000"));
                auto channelId = as<std::shared_ptr<UUID>>(std::string("567e8901-e89b-12d3-a456-426614174000"));
                std::async([=]() { agentServer->createChannel(object{
                    object::pair{std::string("id"), channelId}, 
                    object::pair{std::string("name"), std::string("Bulk Test Channel")}, 
                    object::pair{std::string("type"), ChannelType->GROUP}, 
                    object::pair{std::string("messageServerId"), serverId}, 
                    object::pair{std::string("metadata"), object{}}
                }); });
                auto startTime = Date->now();
                auto bulkPromises = array<any>();
                for (auto i = 0; i < 100; i++)
                {
                    bulkPromises->push(as<any>(agentServer->createMessage(object{
                        object::pair{std::string("channelId"), std::string("channelId")}, 
                        object::pair{std::string("authorId"), as<std::shared_ptr<UUID>>(std::string("bulk-user-") + (i % 10) + string_empty)}, 
                        object::pair{std::string("content"), std::string("Bulk message ") + i + string_empty}, 
                        object::pair{std::string("rawMessage"), std::string("Bulk message ") + i + string_empty}, 
                        object::pair{std::string("sourceId"), std::string("bulk-") + i + string_empty}, 
                        object::pair{std::string("sourceType"), std::string("test")}, 
                        object::pair{std::string("metadata"), object{
                            object::pair{std::string("index"), i}
                        }}
                    })));
                }
                std::async([=]() { Promise->all(bulkPromises); });
                auto endTime = Date->now();
                expect(endTime - startTime)->toBeLessThan(5000);
                auto messages = std::async([=]() { agentServer->getMessagesForChannel(channelId, 150); });
                expect(messages)->toHaveLength(100);
            }
            );
            it(std::string("should handle large result sets"), [=]() mutable
            {
                auto serverId = as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000000"));
                auto channelPromises = array<any>();
                for (auto i = 0; i < 20; i++)
                {
                    channelPromises->push(as<any>(agentServer->createChannel(object{
                        object::pair{std::string("name"), std::string("Large Set Channel ") + i + string_empty}, 
                        object::pair{std::string("type"), ChannelType->GROUP}, 
                        object::pair{std::string("messageServerId"), serverId}, 
                        object::pair{std::string("metadata"), object{
                            object::pair{std::string("index"), i}
                        }}
                    })));
                }
                std::async([=]() { Promise->all(channelPromises); });
                auto channels = std::async([=]() { agentServer->getChannelsForServer(serverId); });
                expect(channels->get_length())->toBeGreaterThanOrEqual(20);
            }
            );
        }
        );
        describe(std::string("Data Integrity Checks"), [=]() mutable
        {
            it(std::string("should create DM channels properly"), [=]() mutable
            {
                auto serverId = as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000000"));
                auto user1 = as<std::shared_ptr<UUID>>(std::string("777e7777-e89b-12d3-a456-426614174000"));
                auto user2 = as<std::shared_ptr<UUID>>(std::string("888e8888-e89b-12d3-a456-426614174000"));
                auto dm1 = std::async([=]() { agentServer->findOrCreateCentralDmChannel(user1, user2, serverId); });
                expect(dm1)->toBeDefined();
                expect(dm1->type)->toBe(ChannelType->DM);
            }
            );
            it(std::string("should enforce server existence for channels"), [=]() mutable
            {
                auto nonExistentServerId = as<std::shared_ptr<UUID>>(std::string("999e9999-e89b-12d3-a456-426614174000"));
                try
                {
                    std::async([=]() { agentServer->createChannel(object{
                        object::pair{std::string("name"), std::string("Invalid Server Channel")}, 
                        object::pair{std::string("type"), ChannelType->GROUP}, 
                        object::pair{std::string("messageServerId"), nonExistentServerId}, 
                        object::pair{std::string("metadata"), object{}}
                    }); });
                }
                catch (const any& error)
                {
                    expect(error)->toBeDefined();
                }
            }
            );
        }
        );
    }
    );
}

MAIN
