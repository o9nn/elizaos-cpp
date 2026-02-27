#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/server/src/__tests__/integration/database-operations.test.h"

void Main(void)
{
    describe(std:("Database Operations Integration Tests"), [=]() mutable
    {
        shared<std::shared_ptr<AgentServer>> agentServer;
        shared<string> testDbPath;
        beforeAll([=]() mutable
        {
            testDbPath = path->join(__dirname, std:("test-db-ops-") + Date->now() + std:("-") + Math->random()->toString(36)->substring(7) + string_empty);
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
                    object::pair{std:("dataDir"), testDbPath}
                }); });
            }
            catch (const any& error)
            {
                console->error(std:("Failed to initialize agent server:"), error);
                if (fs->existsSync(testDbPath)) {
                    fs->rmSync(testDbPath, object{
                        object::pair{std:("recursive"), true}, 
                        object::pair{std:("force"), true}
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
                    object::pair{std:("recursive"), true}, 
                    object::pair{std:("force"), true}
                });
            }
        }
        );
        describe(std:("Transaction Handling"), [=]() mutable
        {
            it(std:("should handle concurrent message creation"), [=]() mutable
            {
                auto channelId = as<std::shared_ptr<UUID>>(std:("123e4567-e89b-12d3-a456-426614174000"));
                shared serverId = as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000000"));
                auto servers = std::async([=]() { agentServer->getServers(); });
                expect(servers->get_length())->toBeGreaterThan(0);
                expect(servers->some([=](auto s) mutable
                {
                    return s->id == serverId;
                }
                ))->toBe(true);
                std::async([=]() { agentServer->createChannel(object{
                    object::pair{std:("id"), channelId}, 
                    object::pair{std:("name"), std:("Concurrent Test Channel")}, 
                    object::pair{std:("type"), ChannelType->GROUP}, 
                    object::pair{std:("messageServerId"), serverId}, 
                    object::pair{std:("metadata"), object{}}
                }); });
                auto messagePromises = array<std::shared_ptr<Promise<std::shared_ptr<CentralRootMessage>>>>();
                for (auto i = 0; i < 10; i++)
                {
                    messagePromises->push(agentServer->createMessage(object{
                        object::pair{std:("channelId"), std:("channelId")}, 
                        object::pair{std:("authorId"), as<std::shared_ptr<UUID>>(std:("user-") + i + string_empty)}, 
                        object::pair{std:("content"), std:("Concurrent message ") + i + string_empty}, 
                        object::pair{std:("rawMessage"), std:("Concurrent message ") + i + string_empty}, 
                        object::pair{std:("sourceId"), std:("concurrent-") + i + string_empty}, 
                        object::pair{std:("sourceType"), std:("test")}, 
                        object::pair{std:("metadata"), object{}}
                    }));
                }
                auto messages = std::async([=]() { Promise->all(messagePromises); });
                expect(messages)->toHaveLength(10);
                messages->forEach([=](auto msg, auto index) mutable
                {
                    expect(msg->content)->toBe(std:("Concurrent message ") + index + string_empty);
                }
                );
                auto retrievedMessages = std::async([=]() { agentServer->getMessagesForChannel(channelId, 20); });
                expect(retrievedMessages)->toHaveLength(10);
            }
            );
            it(std:("should maintain referential integrity"), [=]() mutable
            {
                auto channelId = as<std::shared_ptr<UUID>>(std:("234e5678-e89b-12d3-a456-426614174000"));
                auto serverId = as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000000"));
                std::async([=]() { agentServer->createChannel(object{
                    object::pair{std:("id"), channelId}, 
                    object::pair{std:("name"), std:("Integrity Test Channel")}, 
                    object::pair{std:("type"), ChannelType->GROUP}, 
                    object::pair{std:("messageServerId"), serverId}, 
                    object::pair{std:("metadata"), object{}}
                }); });
                auto message1 = std::async([=]() { agentServer->createMessage(object{
                    object::pair{std:("channelId"), std:("channelId")}, 
                    object::pair{std:("authorId"), as<std::shared_ptr<UUID>>(std:("user-1"))}, 
                    object::pair{std:("content"), std:("First message")}, 
                    object::pair{std:("rawMessage"), std:("First message")}, 
                    object::pair{std:("sourceId"), std:("integrity-1")}, 
                    object::pair{std:("sourceType"), std:("test")}, 
                    object::pair{std:("metadata"), object{}}
                }); });
                std::async([=]() { agentServer->createMessage(object{
                    object::pair{std:("channelId"), std:("channelId")}, 
                    object::pair{std:("authorId"), as<std::shared_ptr<UUID>>(std:("user-2"))}, 
                    object::pair{std:("content"), std:("Reply message")}, 
                    object::pair{std:("rawMessage"), std:("Reply message")}, 
                    object::pair{std:("sourceId"), std:("integrity-2")}, 
                    object::pair{std:("sourceType"), std:("test")}, 
                    object::pair{std:("inReplyToRootMessageId"), message1->id}, 
                    object::pair{std:("metadata"), object{}}
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
        describe(std:("Complex Queries"), [=]() mutable
        {
            it(std:("should handle channel participant management"), [=]() mutable
            {
                auto serverId = as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000000"));
                auto channelId = as<std::shared_ptr<UUID>>(std:("345e6789-e89b-12d3-a456-426614174000"));
                auto participants = array<any>{ as<std::shared_ptr<UUID>>(std:("111e1111-e89b-12d3-a456-426614174000")), as<std::shared_ptr<UUID>>(std:("222e2222-e89b-12d3-a456-426614174000")), as<std::shared_ptr<UUID>>(std:("333e3333-e89b-12d3-a456-426614174000")) };
                std::async([=]() { agentServer->createChannel(object{
                    object::pair{std:("id"), channelId}, 
                    object::pair{std:("name"), std:("Participant Test Channel")}, 
                    object::pair{std:("type"), ChannelType->GROUP}, 
                    object::pair{std:("messageServerId"), serverId}, 
                    object::pair{std:("metadata"), object{}}
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
            it(std:("should handle complex message queries with filters"), [=]() mutable
            {
                auto serverId = as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000000"));
                auto channelId = as<std::shared_ptr<UUID>>(std:("456e7890-e89b-12d3-a456-426614174000"));
                std::async([=]() { agentServer->createChannel(object{
                    object::pair{std:("id"), channelId}, 
                    object::pair{std:("name"), std:("Query Test Channel")}, 
                    object::pair{std:("type"), ChannelType->GROUP}, 
                    object::pair{std:("messageServerId"), serverId}, 
                    object::pair{std:("metadata"), object{}}
                }); });
                auto baseTime = std::make_shared<Date>();
                for (auto i = 0; i < 20; i++)
                {
                    std::async([=]() { agentServer->createMessage(object{
                        object::pair{std:("channelId"), std:("channelId")}, 
                        object::pair{std:("authorId"), as<std::shared_ptr<UUID>>(std:("user-") + (i % 3) + string_empty)}, 
                        object::pair{std:("content"), std:("Message ") + i + std:(" from user ") + (i % 3) + string_empty}, 
                        object::pair{std:("rawMessage"), std:("Message ") + i + string_empty}, 
                        object::pair{std:("sourceId"), std:("query-") + i + string_empty}, 
                        object::pair{std:("sourceType"), std:("test")}, 
                        object::pair{std:("metadata"), object{
                            object::pair{std:("timestamp"), ((std::make_shared<Date>(baseTime->getTime() + i * 1000)))->toISOString()}
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
        describe(std:("Database State Consistency"), [=]() mutable
        {
            it(std:("should maintain consistent state across operations"), [=]() mutable
            {
                auto agentId = as<std::shared_ptr<UUID>>(std:("consistency-agent"));
                auto initialServers = std::async([=]() { agentServer->getServers(); });
                auto initialServerCount = initialServers->get_length();
                auto newServer = std::async([=]() { agentServer->createServer(object{
                    object::pair{std:("name"), std:("Consistency Test Server")}, 
                    object::pair{std:("sourceType"), std:("consistency-test")}, 
                    object::pair{std:("metadata"), object{}}
                }); });
                auto afterCreateServers = std::async([=]() { agentServer->getServers(); });
                expect(afterCreateServers)->toHaveLength(initialServerCount + 1);
                std::async([=]() { agentServer->addAgentToServer(newServer->id, agentId); });
                auto agentsOnServer = std::async([=]() { agentServer->getAgentsForServer(newServer->id); });
                expect(agentsOnServer)->toContain(agentId);
                shared channel = std::async([=]() { agentServer->createChannel(object{
                    object::pair{std:("name"), std:("Server Channel")}, 
                    object::pair{std:("type"), ChannelType->GROUP}, 
                    object::pair{std:("messageServerId"), newServer->id}, 
                    object::pair{std:("metadata"), object{}}
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
            it(std:("should handle database connection failures gracefully"), [=]() mutable
            {
                auto invalidId = std:("invalid-uuid-format");
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
        describe(std:("Performance and Bulk Operations"), [=]() mutable
        {
            it(std:("should handle bulk message insertion efficiently"), [=]() mutable
            {
                auto serverId = as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000000"));
                auto channelId = as<std::shared_ptr<UUID>>(std:("567e8901-e89b-12d3-a456-426614174000"));
                std::async([=]() { agentServer->createChannel(object{
                    object::pair{std:("id"), channelId}, 
                    object::pair{std:("name"), std:("Bulk Test Channel")}, 
                    object::pair{std:("type"), ChannelType->GROUP}, 
                    object::pair{std:("messageServerId"), serverId}, 
                    object::pair{std:("metadata"), object{}}
                }); });
                auto startTime = Date->now();
                auto bulkPromises = array<any>();
                for (auto i = 0; i < 100; i++)
                {
                    bulkPromises->push(as<any>(agentServer->createMessage(object{
                        object::pair{std:("channelId"), std:("channelId")}, 
                        object::pair{std:("authorId"), as<std::shared_ptr<UUID>>(std:("bulk-user-") + (i % 10) + string_empty)}, 
                        object::pair{std:("content"), std:("Bulk message ") + i + string_empty}, 
                        object::pair{std:("rawMessage"), std:("Bulk message ") + i + string_empty}, 
                        object::pair{std:("sourceId"), std:("bulk-") + i + string_empty}, 
                        object::pair{std:("sourceType"), std:("test")}, 
                        object::pair{std:("metadata"), object{
                            object::pair{std:("index"), i}
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
            it(std:("should handle large result sets"), [=]() mutable
            {
                auto serverId = as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000000"));
                auto channelPromises = array<any>();
                for (auto i = 0; i < 20; i++)
                {
                    channelPromises->push(as<any>(agentServer->createChannel(object{
                        object::pair{std:("name"), std:("Large Set Channel ") + i + string_empty}, 
                        object::pair{std:("type"), ChannelType->GROUP}, 
                        object::pair{std:("messageServerId"), serverId}, 
                        object::pair{std:("metadata"), object{
                            object::pair{std:("index"), i}
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
        describe(std:("Data Integrity Checks"), [=]() mutable
        {
            it(std:("should create DM channels properly"), [=]() mutable
            {
                auto serverId = as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000000"));
                auto user1 = as<std::shared_ptr<UUID>>(std:("777e7777-e89b-12d3-a456-426614174000"));
                auto user2 = as<std::shared_ptr<UUID>>(std:("888e8888-e89b-12d3-a456-426614174000"));
                auto dm1 = std::async([=]() { agentServer->findOrCreateCentralDmChannel(user1, user2, serverId); });
                expect(dm1)->toBeDefined();
                expect(dm1->type)->toBe(ChannelType->DM);
            }
            );
            it(std:("should enforce server existence for channels"), [=]() mutable
            {
                auto nonExistentServerId = as<std::shared_ptr<UUID>>(std:("999e9999-e89b-12d3-a456-426614174000"));
                try
                {
                    std::async([=]() { agentServer->createChannel(object{
                        object::pair{std:("name"), std:("Invalid Server Channel")}, 
                        object::pair{std:("type"), ChannelType->GROUP}, 
                        object::pair{std:("messageServerId"), nonExistentServerId}, 
                        object::pair{std:("metadata"), object{}}
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
