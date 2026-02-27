#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/server/src/__tests__/integration/socketio-message-flow.test.h"

void Main(void)
{
    describe(std:("Socket.IO End-to-End Message Flow"), [=]() mutable
    {
        shared<std::shared_ptr<AgentServer>> agentServer;
        shared<double> port;
        shared<std::shared_ptr<ClientSocket>> client1;
        shared<std::shared_ptr<ClientSocket>> client2;
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<string> testDbPath;
        beforeAll([=]() mutable
        {
            testDbPath = path->join(__dirname, std:("test-db-") + Date->now() + string_empty);
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
            auto testCharacter = as<std::shared_ptr<Character>>(object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("test-char"))}, 
                object::pair{std:("name"), std:("Test Agent")}, 
                object::pair{std:("bio"), array<string>{ std:("Test bio") }}, 
                object::pair{std:("topics"), array<any>()}, 
                object::pair{std:("clients"), array<any>()}, 
                object::pair{std:("plugins"), array<any>()}, 
                object::pair{std:("settings"), object{
                    object::pair{std:("model"), std:("gpt-4")}, 
                    object::pair{std:("secrets"), object{}}
                }}, 
                object::pair{std:("modelProvider"), std:("openai")}
            });
            auto db = createDatabaseAdapter(object{
                object::pair{std:("dataDir"), testDbPath}
            }, as<std::shared_ptr<UUID>>(std:("test-agent-123")));
            std::async([=]() { db->init(); });
            mockRuntime = std::make_shared<AgentRuntime>(as<any>(object{
                object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("test-agent-123"))}, 
                object::pair{std:("character"), testCharacter}, 
                object::pair{std:("adapter"), db}, 
                object::pair{std:("token"), OR((process->env->OPENAI_API_KEY), (std:("test-token")))}, 
                object::pair{std:("serverUrl"), std:("http://localhost:3000")}
            }));
            std::async([=]() { agentServer->registerAgent(mockRuntime); });
            port = 3100;
            agentServer->start(port);
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 1000);
            }
            ); });
        }
        );
        afterAll([=]() mutable
        {
            if (client1) client1->close();
            if (client2) client2->close();
            std::async([=]() { agentServer->stop(); });
            if (fs->existsSync(testDbPath)) {
                fs->rmSync(testDbPath, object{
                    object::pair{std:("recursive"), true}, 
                    object::pair{std:("force"), true}
                });
            }
        }
        );
        beforeEach([=]() mutable
        {
            client1 = ioClient(std:("http://localhost:") + port + string_empty, object{
                object::pair{std:("autoConnect"), false}, 
                object::pair{std:("transports"), array<string>{ std:("websocket") }}
            });
            client2 = ioClient(std:("http://localhost:") + port + string_empty, object{
                object::pair{std:("autoConnect"), false}, 
                object::pair{std:("transports"), array<string>{ std:("websocket") }}
            });
        }
        );
        afterEach([=]() mutable
        {
            if (client1->connected) client1->disconnect();
            if (client2->connected) client2->disconnect();
        }
        );
        describe(std:("Connection and Channel Joining"), [=]() mutable
        {
            it(std:("should establish connection and join channel"), [=]() mutable
            {
                auto connectionPromise = std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std:("connection_established"), [=](auto data) mutable
                    {
                        expect(data)->toHaveProperty(std:("socketId"));
                        expect(data["message"])->toContain(std:("Connected to Eliza Socket.IO server"));
                        resolve(data);
                    }
                    );
                }
                );
                client1->connect();
                std::async([=]() { connectionPromise; });
                auto joinPromise = std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std:("channel_joined"), [=](auto data) mutable
                    {
                        expect(data)->toHaveProperty(std:("channelId"));
                        expect(data["message"])->toContain(std:("successfully joined"));
                        resolve(data);
                    }
                    );
                }
                );
                auto channelId = std:("123e4567-e89b-12d3-a456-426614174000");
                client1->emit(String(SOCKET_MESSAGE_TYPE->ROOM_JOINING), object{
                    object::pair{std:("channelId"), std:("channelId")}, 
                    object::pair{std:("entityId"), std:("user-123")}, 
                    object::pair{std:("serverId"), std:("00000000-0000-0000-0000-000000000000")}
                });
                std::async([=]() { joinPromise; });
            }
            );
            it(std:("should allow multiple clients to join same channel"), [=]() mutable
            {
                std::async([=]() { Promise->all(std::tuple<std::shared_ptr<Promise<any>>, std::shared_ptr<Promise<any>>>{ std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std:("connection_established"), resolve);
                    client1->connect();
                }
                ), std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client2->on(std:("connection_established"), resolve);
                    client2->connect();
                }
                ) }); });
                shared channelId = std:("123e4567-e89b-12d3-a456-426614174000");
                auto joinPromises = Promise->all(std::tuple<std::shared_ptr<Promise<any>>, std::shared_ptr<Promise<any>>>{ std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std:("channel_joined"), resolve);
                    client1->emit(String(SOCKET_MESSAGE_TYPE->ROOM_JOINING), object{
                        object::pair{std:("channelId"), std:("channelId")}, 
                        object::pair{std:("entityId"), std:("user-1")}, 
                        object::pair{std:("serverId"), std:("00000000-0000-0000-0000-000000000000")}
                    });
                }
                ), std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client2->on(std:("channel_joined"), resolve);
                    client2->emit(String(SOCKET_MESSAGE_TYPE->ROOM_JOINING), object{
                        object::pair{std:("channelId"), std:("channelId")}, 
                        object::pair{std:("entityId"), std:("user-2")}, 
                        object::pair{std:("serverId"), std:("00000000-0000-0000-0000-000000000000")}
                    });
                }
                ) });
                std::async([=]() { joinPromises; });
            }
            );
        }
        );
        describe(std:("Message Sending and Broadcasting"), [=]() mutable
        {
            it(std:("should send message and broadcast to other clients"), [=]() mutable
            {
                shared channelId = std:("123e4567-e89b-12d3-a456-426614174000");
                shared serverId = std:("00000000-0000-0000-0000-000000000000");
                std::async([=]() { Promise->all(std::tuple<std::shared_ptr<Promise<any>>, std::shared_ptr<Promise<any>>>{ std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std:("connection_established"), [=]() mutable
                    {
                        client1->emit(String(SOCKET_MESSAGE_TYPE->ROOM_JOINING), object{
                            object::pair{std:("channelId"), std:("channelId")}, 
                            object::pair{std:("entityId"), std:("user-1")}, 
                            object::pair{std:("serverId"), std:("serverId")}
                        });
                    }
                    );
                    client1->on(std:("channel_joined"), resolve);
                    client1->connect();
                }
                ), std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client2->on(std:("connection_established"), [=]() mutable
                    {
                        client2->emit(String(SOCKET_MESSAGE_TYPE->ROOM_JOINING), object{
                            object::pair{std:("channelId"), std:("channelId")}, 
                            object::pair{std:("entityId"), std:("user-2")}, 
                            object::pair{std:("serverId"), std:("serverId")}
                        });
                    }
                    );
                    client2->on(std:("channel_joined"), resolve);
                    client2->connect();
                }
                ) }); });
                auto messageReceived = std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client2->on(std:("messageBroadcast"), [=](auto message) mutable
                    {
                        expect(message)->toHaveProperty(std:("id"));
                        expect(message["text"])->toBe(std:("Hello from client1"));
                        expect(message["senderId"])->toBe(std:("user-1-id"));
                        expect(message["channelId"])->toBe(channelId);
                        resolve(message);
                    }
                    );
                }
                );
                auto ackReceived = std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std:("messageAck"), [=](auto ack) mutable
                    {
                        expect(ack["status"])->toBe(std:("received_by_server_and_processing"));
                        resolve(ack);
                    }
                    );
                }
                );
                client1->emit(String(SOCKET_MESSAGE_TYPE->SEND_MESSAGE), object{
                    object::pair{std:("channelId"), std:("channelId")}, 
                    object::pair{std:("senderId"), std:("user-1-id")}, 
                    object::pair{std:("senderName"), std:("User 1")}, 
                    object::pair{std:("message"), std:("Hello from client1")}, 
                    object::pair{std:("serverId"), std:("serverId")}, 
                    object::pair{std:("messageId"), std:("client-msg-1")}
                });
                std::async([=]() { Promise->all(std::tuple<std::shared_ptr<Promise<any>>, std::shared_ptr<Promise<any>>>{ messageReceived, ackReceived }); });
            }
            );
            it(std:("should handle message with attachments"), [=]() mutable
            {
                shared channelId = std:("456e7890-e89b-12d3-a456-426614174000");
                shared serverId = std:("00000000-0000-0000-0000-000000000000");
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std:("connection_established"), [=]() mutable
                    {
                        client1->emit(String(SOCKET_MESSAGE_TYPE->ROOM_JOINING), object{
                            object::pair{std:("channelId"), std:("channelId")}, 
                            object::pair{std:("entityId"), std:("user-1")}, 
                            object::pair{std:("serverId"), std:("serverId")}
                        });
                    }
                    );
                    client1->on(std:("channel_joined"), resolve);
                    client1->connect();
                }
                ); });
                auto ackReceived = std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std:("messageAck"), resolve);
                }
                );
                auto messageBroadcast = std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std:("messageBroadcast"), [=](auto message) mutable
                    {
                        expect(message["attachments"])->toHaveLength(1);
                        expect(const_(message["attachments"])[0])->toEqual(object{
                            object::pair{std:("url"), std:("https://example.com/image.jpg")}, 
                            object::pair{std:("type"), std:("image")}
                        });
                        resolve(message);
                    }
                    );
                }
                );
                client1->emit(String(SOCKET_MESSAGE_TYPE->SEND_MESSAGE), object{
                    object::pair{std:("channelId"), std:("channelId")}, 
                    object::pair{std:("senderId"), std:("user-1-id")}, 
                    object::pair{std:("senderName"), std:("User 1")}, 
                    object::pair{std:("message"), std:("Check out this image")}, 
                    object::pair{std:("serverId"), std:("serverId")}, 
                    object::pair{std:("attachments"), array<object>{ object{
                        object::pair{std:("url"), std:("https://example.com/image.jpg")}, 
                        object::pair{std:("type"), std:("image")}
                    } }}
                });
                std::async([=]() { Promise->all(std::tuple<std::shared_ptr<Promise<any>>, std::shared_ptr<Promise<any>>>{ ackReceived, messageBroadcast }); });
            }
            );
        }
        );
        describe(std:("DM Channel Creation and Messaging"), [=]() mutable
        {
            it(std:("should auto-create DM channel and send message"), [=]() mutable
            {
                auto channelId = std:("789e1234-e89b-12d3-a456-426614174000");
                auto serverId = std:("00000000-0000-0000-0000-000000000000");
                auto user1Id = std:("111e2222-e89b-12d3-a456-426614174000");
                auto user2Id = std:("222e3333-e89b-12d3-a456-426614174000");
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std:("connection_established"), resolve);
                    client1->connect();
                }
                ); });
                auto ackReceived = std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std:("messageAck"), [=](auto ack) mutable
                    {
                        expect(ack["status"])->toBe(std:("received_by_server_and_processing"));
                        resolve(ack);
                    }
                    );
                }
                );
                client1->emit(String(SOCKET_MESSAGE_TYPE->SEND_MESSAGE), object{
                    object::pair{std:("channelId"), std:("channelId")}, 
                    object::pair{std:("senderId"), user1Id}, 
                    object::pair{std:("senderName"), std:("User 1")}, 
                    object::pair{std:("message"), std:("Hello, this is a DM")}, 
                    object::pair{std:("serverId"), std:("serverId")}, 
                    object::pair{std:("targetUserId"), user2Id}, 
                    object::pair{std:("metadata"), object{
                        object::pair{std:("isDm"), true}, 
                        object::pair{std:("channelType"), ChannelType->DM}
                    }}
                });
                std::async([=]() { ackReceived; });
                auto channel = std::async([=]() { agentServer->getChannelDetails(as<std::shared_ptr<UUID>>(channelId)); });
                expect(channel)->toBeTruthy();
                expect(channel->type)->toBe(ChannelType->DM);
            }
            );
        }
        );
        describe(std:("Error Handling and Edge Cases"), [=]() mutable
        {
            it(std:("should handle invalid channel ID gracefully"), [=]() mutable
            {
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std:("connection_established"), resolve);
                    client1->connect();
                }
                ); });
                auto errorReceived = std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std:("messageError"), [=](auto error) mutable
                    {
                        expect(error["error"])->toContain(std:("channelId is required"));
                        resolve(error);
                    }
                    );
                }
                );
                client1->emit(String(SOCKET_MESSAGE_TYPE->ROOM_JOINING), object{
                    object::pair{std:("entityId"), std:("user-1")}, 
                    object::pair{std:("serverId"), std:("00000000-0000-0000-0000-000000000000")}
                });
                std::async([=]() { errorReceived; });
            }
            );
            it(std:("should handle message without required fields"), [=]() mutable
            {
                auto channelId = std:("999e4567-e89b-12d3-a456-426614174000");
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std:("connection_established"), resolve);
                    client1->connect();
                }
                ); });
                auto errorReceived = std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std:("messageError"), [=](auto error) mutable
                    {
                        expect(error["error"])->toContain(std:("required"));
                        resolve(error);
                    }
                    );
                }
                );
                client1->emit(String(SOCKET_MESSAGE_TYPE->SEND_MESSAGE), object{
                    object::pair{std:("channelId"), std:("channelId")}
                });
                std::async([=]() { errorReceived; });
            }
            );
            it(std:("should handle disconnection and cleanup"), [=]() mutable
            {
                shared channelId = std:("888e4567-e89b-12d3-a456-426614174000");
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std:("connection_established"), [=]() mutable
                    {
                        client1->emit(String(SOCKET_MESSAGE_TYPE->ROOM_JOINING), object{
                            object::pair{std:("channelId"), std:("channelId")}, 
                            object::pair{std:("entityId"), std:("user-1")}, 
                            object::pair{std:("serverId"), std:("00000000-0000-0000-0000-000000000000")}
                        });
                    }
                    );
                    client1->on(std:("channel_joined"), resolve);
                    client1->connect();
                }
                ); });
                auto disconnectPromise = std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std:("disconnect"), resolve);
                    client1->disconnect();
                }
                );
                std::async([=]() { disconnectPromise; });
                expect(client1->connected)->toBe(false);
            }
            );
        }
        );
        describe(std:("Log Streaming"), [=]() mutable
        {
            it(std:("should subscribe to log stream and receive filtered logs"), [=]() mutable
            {
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std:("connection_established"), resolve);
                    client1->connect();
                }
                ); });
                auto subscriptionConfirmed = std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std:("log_subscription_confirmed"), [=](auto data) mutable
                    {
                        expect(data["subscribed"])->toBe(true);
                        resolve(data);
                    }
                    );
                }
                );
                client1->emit(std:("subscribe_logs"));
                std::async([=]() { subscriptionConfirmed; });
                auto filtersUpdated = std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std:("log_filters_updated"), [=](auto data) mutable
                    {
                        expect(data["success"])->toBe(true);
                        expect(data["filters"])->toMatchObject(object{
                            object::pair{std:("agentName"), std:("Test Agent")}, 
                            object::pair{std:("level"), std:("info")}
                        });
                        resolve(data);
                    }
                    );
                }
                );
                client1->emit(std:("update_log_filters"), object{
                    object::pair{std:("agentName"), std:("Test Agent")}, 
                    object::pair{std:("level"), std:("info")}
                });
                std::async([=]() { filtersUpdated; });
            }
            );
            it(std:("should unsubscribe from log stream"), [=]() mutable
            {
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std:("connection_established"), resolve);
                    client1->connect();
                }
                ); });
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std:("log_subscription_confirmed"), resolve);
                    client1->emit(std:("subscribe_logs"));
                }
                ); });
                auto unsubscribeConfirmed = std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std:("log_subscription_confirmed"), [=](auto data) mutable
                    {
                        expect(data["subscribed"])->toBe(false);
                        resolve(data);
                    }
                    );
                }
                );
                client1->emit(std:("unsubscribe_logs"));
                std::async([=]() { unsubscribeConfirmed; });
            }
            );
        }
        );
    }
    );
}

MAIN
