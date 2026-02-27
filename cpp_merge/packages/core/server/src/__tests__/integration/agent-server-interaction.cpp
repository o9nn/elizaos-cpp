#include "agent-server-interaction.test.h"

void Main(void)
{
    describe(std:("Agent-Server Interaction Integration Tests"), [=]() mutable
    {
        shared<std::shared_ptr<AgentServer>> agentServer;
        shared<string> testDbPath;
        shared<std::shared_ptr<IAgentRuntime>> agent1;
        shared<std::shared_ptr<IAgentRuntime>> agent2;
        beforeAll([=]() mutable
        {
            testDbPath = path->join(__dirname, std:("test-db-agent-") + Date->now() + string_empty);
            process->env->PGLITE_DATA_DIR = testDbPath;
            agentServer = std::make_shared<AgentServer>();
            std::async([=]() { agentServer->initialize(object{
                object::pair{std:("dataDir"), testDbPath}
            }); });
            auto char1 = as<std::shared_ptr<Character>>(object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("char-1"))}, 
                object::pair{std:("name"), std:("Agent One")}, 
                object::pair{std:("bio"), array<string>{ std:("First test agent") }}, 
                object::pair{std:("topics"), array<any>()}, 
                object::pair{std:("clients"), array<any>()}, 
                object::pair{std:("plugins"), array<any>()}, 
                object::pair{std:("settings"), object{
                    object::pair{std:("model"), std:("gpt-4")}, 
                    object::pair{std:("secrets"), object{}}
                }}, 
                object::pair{std:("modelProvider"), std:("openai")}
            });
            auto db1 = createDatabaseAdapter(object{
                object::pair{std:("dataDir"), testDbPath}
            }, as<std::shared_ptr<UUID>>(std:("agent-1")));
            std::async([=]() { db1->init(); });
            agent1 = std::make_shared<AgentRuntime>(as<any>(object{
                object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("agent-1"))}, 
                object::pair{std:("character"), char1}, 
                object::pair{std:("adapter"), db1}, 
                object::pair{std:("token"), OR((process->env->OPENAI_API_KEY), (std:("test-token")))}, 
                object::pair{std:("serverUrl"), std:("http://localhost:3000")}
            }));
            auto char2 = as<std::shared_ptr<Character>>(object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("char-2"))}, 
                object::pair{std:("name"), std:("Agent Two")}, 
                object::pair{std:("bio"), array<string>{ std:("Second test agent") }}, 
                object::pair{std:("topics"), array<any>()}, 
                object::pair{std:("clients"), array<any>()}, 
                object::pair{std:("plugins"), array<any>()}, 
                object::pair{std:("settings"), object{
                    object::pair{std:("model"), std:("gpt-3.5-turbo")}, 
                    object::pair{std:("secrets"), object{}}
                }}, 
                object::pair{std:("modelProvider"), std:("openai")}
            });
            auto db2 = createDatabaseAdapter(object{
                object::pair{std:("dataDir"), testDbPath}
            }, as<std::shared_ptr<UUID>>(std:("agent-2")));
            std::async([=]() { db2->init(); });
            agent2 = std::make_shared<AgentRuntime>(as<any>(object{
                object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("agent-2"))}, 
                object::pair{std:("character"), char2}, 
                object::pair{std:("adapter"), db2}, 
                object::pair{std:("token"), OR((process->env->OPENAI_API_KEY), (std:("test-token")))}, 
                object::pair{std:("serverUrl"), std:("http://localhost:3000")}
            }));
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
        describe(std:("Agent Registration and Management"), [=]() mutable
        {
            it(std:("should register an agent successfully"), [=]() mutable
            {
                std::async([=]() { agentServer->registerAgent(agent1); });
                auto agents = std::async([=]() { agentServer->getAgentsForServer(as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000000"))); });
                expect(agents)->toContain(agent1->agentId);
            }
            );
            it(std:("should register multiple agents"), [=]() mutable
            {
                std::async([=]() { agentServer->registerAgent(agent1); });
                std::async([=]() { agentServer->registerAgent(agent2); });
                auto agents = std::async([=]() { agentServer->getAgentsForServer(as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000000"))); });
                expect(agents->get_length())->toBeGreaterThanOrEqual(2);
                expect(agents)->toContain(agent1->agentId);
                expect(agents)->toContain(agent2->agentId);
            }
            );
            it(std:("should unregister an agent"), [=]() mutable
            {
                std::async([=]() { agentServer->registerAgent(agent1); });
                auto initialAgents = std::async([=]() { agentServer->getAgentsForServer(as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000000"))); });
                auto initialCount = initialAgents->filter([=](auto id) mutable
                {
                    return id == agent1->agentId;
                }
                )->get_length();
                agentServer->unregisterAgent(agent1->agentId);
                auto finalAgents = std::async([=]() { agentServer->getAgentsForServer(as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000000"))); });
                auto finalCount = finalAgents->filter([=](auto id) mutable
                {
                    return id == agent1->agentId;
                }
                )->get_length();
                expect(finalCount)->toBeLessThan(initialCount);
            }
            );
            it(std:("should handle invalid agent registration gracefully"), [=]() mutable
            {
                std::async([=]() { expect(agentServer->registerAgent(as<any>(nullptr)))->rejects->toThrow(std:("Attempted to register null/undefined runtime")); });
                std::async([=]() { expect(agentServer->registerAgent(as<any>(object{})))->rejects->toThrow(std:("Runtime missing agentId")); });
                std::async([=]() { expect(agentServer->registerAgent(as<any>(object{
                    object::pair{std:("agentId"), std:("test-id")}
                })))->rejects->toThrow(std:("Runtime missing character configuration")); });
            }
            );
        }
        );
        describe(std:("Server Management"), [=]() mutable
        {
            it(std:("should ensure default server exists"), [=]() mutable
            {
                auto servers = std::async([=]() { agentServer->getServers(); });
                auto defaultServer = servers->find([=](auto s) mutable
                {
                    return s->id == std:("00000000-0000-0000-0000-000000000000");
                }
                );
                expect(defaultServer)->toBeDefined();
                expect(defaultServer->name)->toBe(std:("Default Server"));
                expect(defaultServer->sourceType)->toBe(std:("eliza_default"));
            }
            );
            it(std:("should create a new server"), [=]() mutable
            {
                auto newServer = std::async([=]() { agentServer->createServer(object{
                    object::pair{std:("name"), std:("Test Server")}, 
                    object::pair{std:("sourceType"), std:("test")}, 
                    object::pair{std:("metadata"), object{
                        object::pair{std:("test"), true}
                    }}
                }); });
                expect(newServer)->toBeDefined();
                expect(newServer->name)->toBe(std:("Test Server"));
                expect(newServer->sourceType)->toBe(std:("test"));
                expect(newServer->metadata)->toEqual(object{
                    object::pair{std:("test"), true}
                });
                auto server = std::async([=]() { agentServer->getServerById(newServer->id); });
                expect(server)->toBeDefined();
                expect(server->name)->toBe(std:("Test Server"));
            }
            );
            it(std:("should get server by source type"), [=]() mutable
            {
                std::async([=]() { agentServer->createServer(object{
                    object::pair{std:("name"), std:("Discord Server")}, 
                    object::pair{std:("sourceType"), std:("discord")}, 
                    object::pair{std:("metadata"), object{}}
                }); });
                auto server = std::async([=]() { agentServer->getServerBySourceType(std:("discord")); });
                expect(server)->toBeDefined();
                expect(server->sourceType)->toBe(std:("discord"));
            }
            );
        }
        );
        describe(std:("Channel Management"), [=]() mutable
        {
            shared<std::shared_ptr<UUID>> serverId;
            beforeEach([=]() mutable
            {
                serverId = as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000000"));
            }
            );
            it(std:("should create a channel"), [=]() mutable
            {
                auto channel = std::async([=]() { agentServer->createChannel(object{
                    object::pair{std:("name"), std:("Test Channel")}, 
                    object::pair{std:("type"), ChannelType->GROUP}, 
                    object::pair{std:("messageServerId"), serverId}, 
                    object::pair{std:("metadata"), object{}}
                }); });
                expect(channel)->toBeDefined();
                expect(channel->name)->toBe(std:("Test Channel"));
                expect(channel->type)->toBe(ChannelType->GROUP);
                expect(channel->messageServerId)->toBe(serverId);
                auto channelDetails = std::async([=]() { agentServer->getChannelDetails(channel->id); });
                expect(channelDetails)->toBeDefined();
                expect(channelDetails->name)->toBe(std:("Test Channel"));
            }
            );
            it(std:("should create channel with participants"), [=]() mutable
            {
                auto userId1 = as<std::shared_ptr<UUID>>(std:("111e2222-e89b-12d3-a456-426614174000"));
                auto userId2 = as<std::shared_ptr<UUID>>(std:("222e3333-e89b-12d3-a456-426614174000"));
                auto channel = std::async([=]() { agentServer->createChannel(object{
                    object::pair{std:("name"), std:("Group Chat")}, 
                    object::pair{std:("type"), ChannelType->GROUP}, 
                    object::pair{std:("messageServerId"), serverId}, 
                    object::pair{std:("metadata"), object{}}
                }, array<any>{ userId1, userId2 }); });
                auto participants = std::async([=]() { agentServer->getChannelParticipants(channel->id); });
                expect(participants)->toHaveLength(2);
                expect(participants)->toContain(userId1);
                expect(participants)->toContain(userId2);
            }
            );
            it(std:("should add participants to existing channel"), [=]() mutable
            {
                auto channel = std::async([=]() { agentServer->createChannel(object{
                    object::pair{std:("name"), std:("Empty Channel")}, 
                    object::pair{std:("type"), ChannelType->GROUP}, 
                    object::pair{std:("messageServerId"), serverId}, 
                    object::pair{std:("metadata"), object{}}
                }); });
                auto userId = as<std::shared_ptr<UUID>>(std:("333e4444-e89b-12d3-a456-426614174000"));
                std::async([=]() { agentServer->addParticipantsToChannel(channel->id, array<any>{ userId }); });
                auto participants = std::async([=]() { agentServer->getChannelParticipants(channel->id); });
                expect(participants)->toContain(userId);
            }
            );
            it(std:("should update channel information"), [=]() mutable
            {
                auto channel = std::async([=]() { agentServer->createChannel(object{
                    object::pair{std:("name"), std:("Original Name")}, 
                    object::pair{std:("type"), ChannelType->GROUP}, 
                    object::pair{std:("messageServerId"), serverId}, 
                    object::pair{std:("metadata"), object{
                        object::pair{std:("original"), true}
                    }}
                }); });
                auto updated = std::async([=]() { agentServer->updateChannel(channel->id, object{
                    object::pair{std:("name"), std:("Updated Name")}, 
                    object::pair{std:("metadata"), object{
                        object::pair{std:("updated"), true}
                    }}
                }); });
                expect(updated->name)->toBe(std:("Updated Name"));
                expect(updated->metadata)->toEqual(object{
                    object::pair{std:("updated"), true}
                });
            }
            );
            it(std:("should delete a channel"), [=]() mutable
            {
                auto channel = std::async([=]() { agentServer->createChannel(object{
                    object::pair{std:("name"), std:("To Be Deleted")}, 
                    object::pair{std:("type"), ChannelType->GROUP}, 
                    object::pair{std:("messageServerId"), serverId}, 
                    object::pair{std:("metadata"), object{}}
                }); });
                std::async([=]() { agentServer->deleteChannel(channel->id); });
                auto channelDetails = std::async([=]() { agentServer->getChannelDetails(channel->id); });
                expect(channelDetails)->toBeNull();
            }
            );
            it(std:("should find or create DM channel"), [=]() mutable
            {
                auto user1Id = as<std::shared_ptr<UUID>>(std:("444e5555-e89b-12d3-a456-426614174000"));
                auto user2Id = as<std::shared_ptr<UUID>>(std:("555e6666-e89b-12d3-a456-426614174000"));
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
        describe(std:("Message Management"), [=]() mutable
        {
            shared<std::shared_ptr<UUID>> channelId;
            shared serverId = as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000000"));
            beforeEach([=]() mutable
            {
                auto channel = std::async([=]() { agentServer->createChannel(object{
                    object::pair{std:("name"), std:("Message Test Channel")}, 
                    object::pair{std:("type"), ChannelType->GROUP}, 
                    object::pair{std:("messageServerId"), serverId}, 
                    object::pair{std:("metadata"), object{}}
                }); });
                channelId = channel->id;
            }
            );
            it(std:("should create and retrieve messages"), [=]() mutable
            {
                auto message1 = std::async([=]() { agentServer->createMessage(object{
                    object::pair{std:("channelId"), std:("channelId")}, 
                    object::pair{std:("authorId"), as<std::shared_ptr<UUID>>(std:("user-1"))}, 
                    object::pair{std:("content"), std:("Hello, world!")}, 
                    object::pair{std:("rawMessage"), std:("Hello, world!")}, 
                    object::pair{std:("sourceId"), std:("msg-1")}, 
                    object::pair{std:("sourceType"), std:("test")}, 
                    object::pair{std:("metadata"), object{}}
                }); });
                expect(message1)->toBeDefined();
                expect(message1->content)->toBe(std:("Hello, world!"));
                expect(message1->channelId)->toBe(channelId);
                std::async([=]() { agentServer->createMessage(object{
                    object::pair{std:("channelId"), std:("channelId")}, 
                    object::pair{std:("authorId"), as<std::shared_ptr<UUID>>(std:("user-2"))}, 
                    object::pair{std:("content"), std:("Hi there!")}, 
                    object::pair{std:("rawMessage"), std:("Hi there!")}, 
                    object::pair{std:("sourceId"), std:("msg-2")}, 
                    object::pair{std:("sourceType"), std:("test")}, 
                    object::pair{std:("metadata"), object{}}
                }); });
                auto messages = std::async([=]() { agentServer->getMessagesForChannel(channelId, 10); });
                expect(messages)->toHaveLength(2);
                expect(const_(messages)[0]->content)->toBe(std:("Hi there!"));
                expect(const_(messages)[1]->content)->toBe(std:("Hello, world!"));
            }
            );
            it(std:("should handle message with reply"), [=]() mutable
            {
                auto originalMessage = std::async([=]() { agentServer->createMessage(object{
                    object::pair{std:("channelId"), std:("channelId")}, 
                    object::pair{std:("authorId"), as<std::shared_ptr<UUID>>(std:("user-1"))}, 
                    object::pair{std:("content"), std:("Original message")}, 
                    object::pair{std:("rawMessage"), std:("Original message")}, 
                    object::pair{std:("sourceId"), std:("original")}, 
                    object::pair{std:("sourceType"), std:("test")}, 
                    object::pair{std:("metadata"), object{}}
                }); });
                auto replyMessage = std::async([=]() { agentServer->createMessage(object{
                    object::pair{std:("channelId"), std:("channelId")}, 
                    object::pair{std:("authorId"), as<std::shared_ptr<UUID>>(std:("user-2"))}, 
                    object::pair{std:("content"), std:("This is a reply")}, 
                    object::pair{std:("rawMessage"), std:("This is a reply")}, 
                    object::pair{std:("sourceId"), std:("reply")}, 
                    object::pair{std:("sourceType"), std:("test")}, 
                    object::pair{std:("inReplyToRootMessageId"), originalMessage->id}, 
                    object::pair{std:("metadata"), object{}}
                }); });
                expect(replyMessage->inReplyToRootMessageId)->toBe(originalMessage->id);
            }
            );
            it(std:("should delete a message"), [=]() mutable
            {
                auto message = std::async([=]() { agentServer->createMessage(object{
                    object::pair{std:("channelId"), std:("channelId")}, 
                    object::pair{std:("authorId"), as<std::shared_ptr<UUID>>(std:("user-1"))}, 
                    object::pair{std:("content"), std:("To be deleted")}, 
                    object::pair{std:("rawMessage"), std:("To be deleted")}, 
                    object::pair{std:("sourceId"), std:("delete-me")}, 
                    object::pair{std:("sourceType"), std:("test")}, 
                    object::pair{std:("metadata"), object{}}
                }); });
                std::async([=]() { agentServer->deleteMessage(message->id); });
                auto messages = std::async([=]() { agentServer->getMessagesForChannel(channelId); });
                expect(messages)->toHaveLength(0);
            }
            );
            it(std:("should clear all channel messages"), [=]() mutable
            {
                for (auto i = 0; i < 5; i++)
                {
                    std::async([=]() { agentServer->createMessage(object{
                        object::pair{std:("channelId"), std:("channelId")}, 
                        object::pair{std:("authorId"), as<std::shared_ptr<UUID>>(std:("user-1"))}, 
                        object::pair{std:("content"), std:("Message ") + i + string_empty}, 
                        object::pair{std:("rawMessage"), std:("Message ") + i + string_empty}, 
                        object::pair{std:("sourceId"), std:("msg-") + i + string_empty}, 
                        object::pair{std:("sourceType"), std:("test")}, 
                        object::pair{std:("metadata"), object{}}
                    }); });
                }
                auto messages = std::async([=]() { agentServer->getMessagesForChannel(channelId); });
                expect(messages)->toHaveLength(5);
                std::async([=]() { agentServer->clearChannelMessages(channelId); });
                messages = std::async([=]() { agentServer->getMessagesForChannel(channelId); });
                expect(messages)->toHaveLength(0);
            }
            );
            it(std:("should retrieve messages with pagination"), [=]() mutable
            {
                auto messagePromises = array<std::shared_ptr<Promise<std::shared_ptr<CentralRootMessage>>>>();
                for (auto i = 0; i < 10; i++)
                {
                    messagePromises->push(agentServer->createMessage(object{
                        object::pair{std:("channelId"), std:("channelId")}, 
                        object::pair{std:("authorId"), as<std::shared_ptr<UUID>>(std:("user-1"))}, 
                        object::pair{std:("content"), std:("Message ") + i + string_empty}, 
                        object::pair{std:("rawMessage"), std:("Message ") + i + string_empty}, 
                        object::pair{std:("sourceId"), std:("msg-") + i + string_empty}, 
                        object::pair{std:("sourceType"), std:("test")}, 
                        object::pair{std:("metadata"), object{}}
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
        describe(std:("Agent-Server Association"), [=]() mutable
        {
            shared<std::shared_ptr<UUID>> serverId;
            shared<std::shared_ptr<UUID>> agentId;
            beforeEach([=]() mutable
            {
                serverId = as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000000"));
                agentId = as<std::shared_ptr<UUID>>(std:("test-agent-assoc"));
            }
            );
            it(std:("should add agent to server"), [=]() mutable
            {
                std::async([=]() { agentServer->addAgentToServer(serverId, agentId); });
                auto agents = std::async([=]() { agentServer->getAgentsForServer(serverId); });
                expect(agents)->toContain(agentId);
            }
            );
            it(std:("should remove agent from server"), [=]() mutable
            {
                std::async([=]() { agentServer->addAgentToServer(serverId, agentId); });
                std::async([=]() { agentServer->removeAgentFromServer(serverId, agentId); });
                auto agents = std::async([=]() { agentServer->getAgentsForServer(serverId); });
                expect(agents)->not->toContain(agentId);
            }
            );
            it(std:("should get servers for agent"), [=]() mutable
            {
                auto newServer = std::async([=]() { agentServer->createServer(object{
                    object::pair{std:("name"), std:("Additional Server")}, 
                    object::pair{std:("sourceType"), std:("test-multi")}, 
                    object::pair{std:("metadata"), object{}}
                }); });
                std::async([=]() { agentServer->addAgentToServer(serverId, agentId); });
                std::async([=]() { agentServer->addAgentToServer(newServer->id, agentId); });
                auto servers = std::async([=]() { agentServer->getServersForAgent(agentId); });
                expect(servers)->toHaveLength(2);
                expect(servers)->toContain(serverId);
                expect(servers)->toContain(newServer->id);
            }
            );
            it(std:("should handle adding agent to non-existent server"), [=]() mutable
            {
                auto fakeServerId = as<std::shared_ptr<UUID>>(std:("non-existent-server"));
                std::async([=]() { expect(agentServer->addAgentToServer(fakeServerId, agentId))->rejects->toThrow(std:("Server non-existent-server not found")); });
            }
            );
        }
        );
    }
    );
}

MAIN
