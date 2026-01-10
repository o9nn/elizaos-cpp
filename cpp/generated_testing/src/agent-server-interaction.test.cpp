#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/server/src/__tests__/integration/agent-server-interaction.test.h"

void Main(void)
{
    describe(std::string("Agent-Server Interaction Integration Tests"), [=]() mutable
    {
        shared<std::shared_ptr<AgentServer>> agentServer;
        shared<string> testDbPath;
        shared<std::shared_ptr<IAgentRuntime>> agent1;
        shared<std::shared_ptr<IAgentRuntime>> agent2;
        beforeAll([=]() mutable
        {
            testDbPath = path->join(__dirname, std::string("test-db-agent-") + Date->now() + string_empty);
            process->env->PGLITE_DATA_DIR = testDbPath;
            agentServer = std::make_shared<AgentServer>();
            std::async([=]() { agentServer->initialize(object{
                object::pair{std::string("dataDir"), testDbPath}
            }); });
            auto char1 = as<std::shared_ptr<Character>>(object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("char-1"))}, 
                object::pair{std::string("name"), std::string("Agent One")}, 
                object::pair{std::string("bio"), array<string>{ std::string("First test agent") }}, 
                object::pair{std::string("topics"), array<any>()}, 
                object::pair{std::string("clients"), array<any>()}, 
                object::pair{std::string("plugins"), array<any>()}, 
                object::pair{std::string("settings"), object{
                    object::pair{std::string("model"), std::string("gpt-4")}, 
                    object::pair{std::string("secrets"), object{}}
                }}, 
                object::pair{std::string("modelProvider"), std::string("openai")}
            });
            auto db1 = createDatabaseAdapter(object{
                object::pair{std::string("dataDir"), testDbPath}
            }, as<std::shared_ptr<UUID>>(std::string("agent-1")));
            std::async([=]() { db1->init(); });
            agent1 = std::make_shared<AgentRuntime>(as<any>(object{
                object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("agent-1"))}, 
                object::pair{std::string("character"), char1}, 
                object::pair{std::string("adapter"), db1}, 
                object::pair{std::string("token"), OR((process->env->OPENAI_API_KEY), (std::string("test-token")))}, 
                object::pair{std::string("serverUrl"), std::string("http://localhost:3000")}
            }));
            auto char2 = as<std::shared_ptr<Character>>(object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("char-2"))}, 
                object::pair{std::string("name"), std::string("Agent Two")}, 
                object::pair{std::string("bio"), array<string>{ std::string("Second test agent") }}, 
                object::pair{std::string("topics"), array<any>()}, 
                object::pair{std::string("clients"), array<any>()}, 
                object::pair{std::string("plugins"), array<any>()}, 
                object::pair{std::string("settings"), object{
                    object::pair{std::string("model"), std::string("gpt-3.5-turbo")}, 
                    object::pair{std::string("secrets"), object{}}
                }}, 
                object::pair{std::string("modelProvider"), std::string("openai")}
            });
            auto db2 = createDatabaseAdapter(object{
                object::pair{std::string("dataDir"), testDbPath}
            }, as<std::shared_ptr<UUID>>(std::string("agent-2")));
            std::async([=]() { db2->init(); });
            agent2 = std::make_shared<AgentRuntime>(as<any>(object{
                object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("agent-2"))}, 
                object::pair{std::string("character"), char2}, 
                object::pair{std::string("adapter"), db2}, 
                object::pair{std::string("token"), OR((process->env->OPENAI_API_KEY), (std::string("test-token")))}, 
                object::pair{std::string("serverUrl"), std::string("http://localhost:3000")}
            }));
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
        describe(std::string("Agent Registration and Management"), [=]() mutable
        {
            it(std::string("should register an agent successfully"), [=]() mutable
            {
                std::async([=]() { agentServer->registerAgent(agent1); });
                auto agents = std::async([=]() { agentServer->getAgentsForServer(as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000000"))); });
                expect(agents)->toContain(agent1->agentId);
            }
            );
            it(std::string("should register multiple agents"), [=]() mutable
            {
                std::async([=]() { agentServer->registerAgent(agent1); });
                std::async([=]() { agentServer->registerAgent(agent2); });
                auto agents = std::async([=]() { agentServer->getAgentsForServer(as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000000"))); });
                expect(agents->get_length())->toBeGreaterThanOrEqual(2);
                expect(agents)->toContain(agent1->agentId);
                expect(agents)->toContain(agent2->agentId);
            }
            );
            it(std::string("should unregister an agent"), [=]() mutable
            {
                std::async([=]() { agentServer->registerAgent(agent1); });
                auto initialAgents = std::async([=]() { agentServer->getAgentsForServer(as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000000"))); });
                auto initialCount = initialAgents->filter([=](auto id) mutable
                {
                    return id == agent1->agentId;
                }
                )->get_length();
                agentServer->unregisterAgent(agent1->agentId);
                auto finalAgents = std::async([=]() { agentServer->getAgentsForServer(as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000000"))); });
                auto finalCount = finalAgents->filter([=](auto id) mutable
                {
                    return id == agent1->agentId;
                }
                )->get_length();
                expect(finalCount)->toBeLessThan(initialCount);
            }
            );
            it(std::string("should handle invalid agent registration gracefully"), [=]() mutable
            {
                std::async([=]() { expect(agentServer->registerAgent(as<any>(nullptr)))->rejects->toThrow(std::string("Attempted to register null/undefined runtime")); });
                std::async([=]() { expect(agentServer->registerAgent(as<any>(object{})))->rejects->toThrow(std::string("Runtime missing agentId")); });
                std::async([=]() { expect(agentServer->registerAgent(as<any>(object{
                    object::pair{std::string("agentId"), std::string("test-id")}
                })))->rejects->toThrow(std::string("Runtime missing character configuration")); });
            }
            );
        }
        );
        describe(std::string("Server Management"), [=]() mutable
        {
            it(std::string("should ensure default server exists"), [=]() mutable
            {
                auto servers = std::async([=]() { agentServer->getServers(); });
                auto defaultServer = servers->find([=](auto s) mutable
                {
                    return s->id == std::string("00000000-0000-0000-0000-000000000000");
                }
                );
                expect(defaultServer)->toBeDefined();
                expect(defaultServer->name)->toBe(std::string("Default Server"));
                expect(defaultServer->sourceType)->toBe(std::string("eliza_default"));
            }
            );
            it(std::string("should create a new server"), [=]() mutable
            {
                auto newServer = std::async([=]() { agentServer->createServer(object{
                    object::pair{std::string("name"), std::string("Test Server")}, 
                    object::pair{std::string("sourceType"), std::string("test")}, 
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("test"), true}
                    }}
                }); });
                expect(newServer)->toBeDefined();
                expect(newServer->name)->toBe(std::string("Test Server"));
                expect(newServer->sourceType)->toBe(std::string("test"));
                expect(newServer->metadata)->toEqual(object{
                    object::pair{std::string("test"), true}
                });
                auto server = std::async([=]() { agentServer->getServerById(newServer->id); });
                expect(server)->toBeDefined();
                expect(server->name)->toBe(std::string("Test Server"));
            }
            );
            it(std::string("should get server by source type"), [=]() mutable
            {
                std::async([=]() { agentServer->createServer(object{
                    object::pair{std::string("name"), std::string("Discord Server")}, 
                    object::pair{std::string("sourceType"), std::string("discord")}, 
                    object::pair{std::string("metadata"), object{}}
                }); });
                auto server = std::async([=]() { agentServer->getServerBySourceType(std::string("discord")); });
                expect(server)->toBeDefined();
                expect(server->sourceType)->toBe(std::string("discord"));
            }
            );
        }
        );
        describe(std::string("Channel Management"), [=]() mutable
        {
            shared<std::shared_ptr<UUID>> serverId;
            beforeEach([=]() mutable
            {
                serverId = as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000000"));
            }
            );
            it(std::string("should create a channel"), [=]() mutable
            {
                auto channel = std::async([=]() { agentServer->createChannel(object{
                    object::pair{std::string("name"), std::string("Test Channel")}, 
                    object::pair{std::string("type"), ChannelType->GROUP}, 
                    object::pair{std::string("messageServerId"), serverId}, 
                    object::pair{std::string("metadata"), object{}}
                }); });
                expect(channel)->toBeDefined();
                expect(channel->name)->toBe(std::string("Test Channel"));
                expect(channel->type)->toBe(ChannelType->GROUP);
                expect(channel->messageServerId)->toBe(serverId);
                auto channelDetails = std::async([=]() { agentServer->getChannelDetails(channel->id); });
                expect(channelDetails)->toBeDefined();
                expect(channelDetails->name)->toBe(std::string("Test Channel"));
            }
            );
            it(std::string("should create channel with participants"), [=]() mutable
            {
                auto userId1 = as<std::shared_ptr<UUID>>(std::string("111e2222-e89b-12d3-a456-426614174000"));
                auto userId2 = as<std::shared_ptr<UUID>>(std::string("222e3333-e89b-12d3-a456-426614174000"));
                auto channel = std::async([=]() { agentServer->createChannel(object{
                    object::pair{std::string("name"), std::string("Group Chat")}, 
                    object::pair{std::string("type"), ChannelType->GROUP}, 
                    object::pair{std::string("messageServerId"), serverId}, 
                    object::pair{std::string("metadata"), object{}}
                }, array<any>{ userId1, userId2 }); });
                auto participants = std::async([=]() { agentServer->getChannelParticipants(channel->id); });
                expect(participants)->toHaveLength(2);
                expect(participants)->toContain(userId1);
                expect(participants)->toContain(userId2);
            }
            );
            it(std::string("should add participants to existing channel"), [=]() mutable
            {
                auto channel = std::async([=]() { agentServer->createChannel(object{
                    object::pair{std::string("name"), std::string("Empty Channel")}, 
                    object::pair{std::string("type"), ChannelType->GROUP}, 
                    object::pair{std::string("messageServerId"), serverId}, 
                    object::pair{std::string("metadata"), object{}}
                }); });
                auto userId = as<std::shared_ptr<UUID>>(std::string("333e4444-e89b-12d3-a456-426614174000"));
                std::async([=]() { agentServer->addParticipantsToChannel(channel->id, array<any>{ userId }); });
                auto participants = std::async([=]() { agentServer->getChannelParticipants(channel->id); });
                expect(participants)->toContain(userId);
            }
            );
            it(std::string("should update channel information"), [=]() mutable
            {
                auto channel = std::async([=]() { agentServer->createChannel(object{
                    object::pair{std::string("name"), std::string("Original Name")}, 
                    object::pair{std::string("type"), ChannelType->GROUP}, 
                    object::pair{std::string("messageServerId"), serverId}, 
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("original"), true}
                    }}
                }); });
                auto updated = std::async([=]() { agentServer->updateChannel(channel->id, object{
                    object::pair{std::string("name"), std::string("Updated Name")}, 
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("updated"), true}
                    }}
                }); });
                expect(updated->name)->toBe(std::string("Updated Name"));
                expect(updated->metadata)->toEqual(object{
                    object::pair{std::string("updated"), true}
                });
            }
            );
            it(std::string("should delete a channel"), [=]() mutable
            {
                auto channel = std::async([=]() { agentServer->createChannel(object{
                    object::pair{std::string("name"), std::string("To Be Deleted")}, 
                    object::pair{std::string("type"), ChannelType->GROUP}, 
                    object::pair{std::string("messageServerId"), serverId}, 
                    object::pair{std::string("metadata"), object{}}
                }); });
                std::async([=]() { agentServer->deleteChannel(channel->id); });
                auto channelDetails = std::async([=]() { agentServer->getChannelDetails(channel->id); });
                expect(channelDetails)->toBeNull();
            }
            );
            it(std::string("should find or create DM channel"), [=]() mutable
            {
                auto user1Id = as<std::shared_ptr<UUID>>(std::string("444e5555-e89b-12d3-a456-426614174000"));
                auto user2Id = as<std::shared_ptr<UUID>>(std::string("555e6666-e89b-12d3-a456-426614174000"));
                auto channel1 = std::async([=]() { agentServer->findOrCreateCentralDmChannel(user1Id, user2Id, serverId); });
                expect(channel1)->toBeDefined();
                expect(channel1->type)->toBe(ChannelType->DM);
                auto channel2 = std::async([=]() { agentServer->findOrCreateCentralDmChannel(user1Id, user2Id, serverId); });
                expect(channel2->id)->toBe(channel1->id);
                auto channel3 = std::async([=]() { agentServer->findOrCreateCentralDmChannel(user2Id, user1Id, serverId); });
                expect(channel3->id)->toBe(channel1->id);
            }
            );
        }
        );
        describe(std::string("Message Management"), [=]() mutable
        {
            shared<std::shared_ptr<UUID>> channelId;
            shared serverId = as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000000"));
            beforeEach([=]() mutable
            {
                auto channel = std::async([=]() { agentServer->createChannel(object{
                    object::pair{std::string("name"), std::string("Message Test Channel")}, 
                    object::pair{std::string("type"), ChannelType->GROUP}, 
                    object::pair{std::string("messageServerId"), serverId}, 
                    object::pair{std::string("metadata"), object{}}
                }); });
                channelId = channel->id;
            }
            );
            it(std::string("should create and retrieve messages"), [=]() mutable
            {
                auto message1 = std::async([=]() { agentServer->createMessage(object{
                    object::pair{std::string("channelId"), std::string("channelId")}, 
                    object::pair{std::string("authorId"), as<std::shared_ptr<UUID>>(std::string("user-1"))}, 
                    object::pair{std::string("content"), std::string("Hello, world!")}, 
                    object::pair{std::string("rawMessage"), std::string("Hello, world!")}, 
                    object::pair{std::string("sourceId"), std::string("msg-1")}, 
                    object::pair{std::string("sourceType"), std::string("test")}, 
                    object::pair{std::string("metadata"), object{}}
                }); });
                expect(message1)->toBeDefined();
                expect(message1->content)->toBe(std::string("Hello, world!"));
                expect(message1->channelId)->toBe(channelId);
                std::async([=]() { agentServer->createMessage(object{
                    object::pair{std::string("channelId"), std::string("channelId")}, 
                    object::pair{std::string("authorId"), as<std::shared_ptr<UUID>>(std::string("user-2"))}, 
                    object::pair{std::string("content"), std::string("Hi there!")}, 
                    object::pair{std::string("rawMessage"), std::string("Hi there!")}, 
                    object::pair{std::string("sourceId"), std::string("msg-2")}, 
                    object::pair{std::string("sourceType"), std::string("test")}, 
                    object::pair{std::string("metadata"), object{}}
                }); });
                auto messages = std::async([=]() { agentServer->getMessagesForChannel(channelId, 10); });
                expect(messages)->toHaveLength(2);
                expect(const_(messages)[0]->content)->toBe(std::string("Hi there!"));
                expect(const_(messages)[1]->content)->toBe(std::string("Hello, world!"));
            }
            );
            it(std::string("should handle message with reply"), [=]() mutable
            {
                auto originalMessage = std::async([=]() { agentServer->createMessage(object{
                    object::pair{std::string("channelId"), std::string("channelId")}, 
                    object::pair{std::string("authorId"), as<std::shared_ptr<UUID>>(std::string("user-1"))}, 
                    object::pair{std::string("content"), std::string("Original message")}, 
                    object::pair{std::string("rawMessage"), std::string("Original message")}, 
                    object::pair{std::string("sourceId"), std::string("original")}, 
                    object::pair{std::string("sourceType"), std::string("test")}, 
                    object::pair{std::string("metadata"), object{}}
                }); });
                auto replyMessage = std::async([=]() { agentServer->createMessage(object{
                    object::pair{std::string("channelId"), std::string("channelId")}, 
                    object::pair{std::string("authorId"), as<std::shared_ptr<UUID>>(std::string("user-2"))}, 
                    object::pair{std::string("content"), std::string("This is a reply")}, 
                    object::pair{std::string("rawMessage"), std::string("This is a reply")}, 
                    object::pair{std::string("sourceId"), std::string("reply")}, 
                    object::pair{std::string("sourceType"), std::string("test")}, 
                    object::pair{std::string("inReplyToRootMessageId"), originalMessage->id}, 
                    object::pair{std::string("metadata"), object{}}
                }); });
                expect(replyMessage->inReplyToRootMessageId)->toBe(originalMessage->id);
            }
            );
            it(std::string("should delete a message"), [=]() mutable
            {
                auto message = std::async([=]() { agentServer->createMessage(object{
                    object::pair{std::string("channelId"), std::string("channelId")}, 
                    object::pair{std::string("authorId"), as<std::shared_ptr<UUID>>(std::string("user-1"))}, 
                    object::pair{std::string("content"), std::string("To be deleted")}, 
                    object::pair{std::string("rawMessage"), std::string("To be deleted")}, 
                    object::pair{std::string("sourceId"), std::string("delete-me")}, 
                    object::pair{std::string("sourceType"), std::string("test")}, 
                    object::pair{std::string("metadata"), object{}}
                }); });
                std::async([=]() { agentServer->deleteMessage(message->id); });
                auto messages = std::async([=]() { agentServer->getMessagesForChannel(channelId); });
                expect(messages)->toHaveLength(0);
            }
            );
            it(std::string("should clear all channel messages"), [=]() mutable
            {
                for (auto i = 0; i < 5; i++)
                {
                    std::async([=]() { agentServer->createMessage(object{
                        object::pair{std::string("channelId"), std::string("channelId")}, 
                        object::pair{std::string("authorId"), as<std::shared_ptr<UUID>>(std::string("user-1"))}, 
                        object::pair{std::string("content"), std::string("Message ") + i + string_empty}, 
                        object::pair{std::string("rawMessage"), std::string("Message ") + i + string_empty}, 
                        object::pair{std::string("sourceId"), std::string("msg-") + i + string_empty}, 
                        object::pair{std::string("sourceType"), std::string("test")}, 
                        object::pair{std::string("metadata"), object{}}
                    }); });
                }
                auto messages = std::async([=]() { agentServer->getMessagesForChannel(channelId); });
                expect(messages)->toHaveLength(5);
                std::async([=]() { agentServer->clearChannelMessages(channelId); });
                messages = std::async([=]() { agentServer->getMessagesForChannel(channelId); });
                expect(messages)->toHaveLength(0);
            }
            );
            it(std::string("should retrieve messages with pagination"), [=]() mutable
            {
                auto messagePromises = array<std::shared_ptr<Promise<std::shared_ptr<CentralRootMessage>>>>();
                for (auto i = 0; i < 10; i++)
                {
                    messagePromises->push(agentServer->createMessage(object{
                        object::pair{std::string("channelId"), std::string("channelId")}, 
                        object::pair{std::string("authorId"), as<std::shared_ptr<UUID>>(std::string("user-1"))}, 
                        object::pair{std::string("content"), std::string("Message ") + i + string_empty}, 
                        object::pair{std::string("rawMessage"), std::string("Message ") + i + string_empty}, 
                        object::pair{std::string("sourceId"), std::string("msg-") + i + string_empty}, 
                        object::pair{std::string("sourceType"), std::string("test")}, 
                        object::pair{std::string("metadata"), object{}}
                    }));
                    std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                    {
                        return setTimeout(resolve, 10);
                    }
                    ); });
                }
                std::async([=]() { Promise->all(messagePromises); });
                auto firstBatch = std::async([=]() { agentServer->getMessagesForChannel(channelId, 5); });
                expect(firstBatch)->toHaveLength(5);
                auto secondBatch = std::async([=]() { agentServer->getMessagesForChannel(channelId, 5, const_(firstBatch)[firstBatch->get_length() - 1]->createdAt); });
                expect(secondBatch)->toHaveLength(5);
                auto firstIds = firstBatch->map([=](auto m) mutable
                {
                    return m->id;
                }
                );
                shared secondIds = secondBatch->map([=](auto m) mutable
                {
                    return m->id;
                }
                );
                auto overlap = firstIds->filter([=](auto id) mutable
                {
                    return secondIds->includes(id);
                }
                );
                expect(overlap)->toHaveLength(0);
            }
            );
        }
        );
        describe(std::string("Agent-Server Association"), [=]() mutable
        {
            shared<std::shared_ptr<UUID>> serverId;
            shared<std::shared_ptr<UUID>> agentId;
            beforeEach([=]() mutable
            {
                serverId = as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000000"));
                agentId = as<std::shared_ptr<UUID>>(std::string("test-agent-assoc"));
            }
            );
            it(std::string("should add agent to server"), [=]() mutable
            {
                std::async([=]() { agentServer->addAgentToServer(serverId, agentId); });
                auto agents = std::async([=]() { agentServer->getAgentsForServer(serverId); });
                expect(agents)->toContain(agentId);
            }
            );
            it(std::string("should remove agent from server"), [=]() mutable
            {
                std::async([=]() { agentServer->addAgentToServer(serverId, agentId); });
                std::async([=]() { agentServer->removeAgentFromServer(serverId, agentId); });
                auto agents = std::async([=]() { agentServer->getAgentsForServer(serverId); });
                expect(agents)->not->toContain(agentId);
            }
            );
            it(std::string("should get servers for agent"), [=]() mutable
            {
                auto newServer = std::async([=]() { agentServer->createServer(object{
                    object::pair{std::string("name"), std::string("Additional Server")}, 
                    object::pair{std::string("sourceType"), std::string("test-multi")}, 
                    object::pair{std::string("metadata"), object{}}
                }); });
                std::async([=]() { agentServer->addAgentToServer(serverId, agentId); });
                std::async([=]() { agentServer->addAgentToServer(newServer->id, agentId); });
                auto servers = std::async([=]() { agentServer->getServersForAgent(agentId); });
                expect(servers)->toHaveLength(2);
                expect(servers)->toContain(serverId);
                expect(servers)->toContain(newServer->id);
            }
            );
            it(std::string("should handle adding agent to non-existent server"), [=]() mutable
            {
                auto fakeServerId = as<std::shared_ptr<UUID>>(std::string("non-existent-server"));
                std::async([=]() { expect(agentServer->addAgentToServer(fakeServerId, agentId))->rejects->toThrow(std::string("Server non-existent-server not found")); });
            }
            );
        }
        );
    }
    );
}

MAIN
