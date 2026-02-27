#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/server/src/__tests__/integration/socketio-message-flow.test.h"

void Main(void)
{
    describe(std::string("Socket.IO End-to-End Message Flow"), [=]() mutable
    {
        shared<std::shared_ptr<AgentServer>> agentServer;
        shared<double> port;
        shared<std::shared_ptr<ClientSocket>> client1;
        shared<std::shared_ptr<ClientSocket>> client2;
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<string> testDbPath;
        beforeAll([=]() mutable
        {
            testDbPath = path->join(__dirname, std::string("test-db-") + Date->now() + string_empty);
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
            auto testCharacter = as<std::shared_ptr<Character>>(object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("test-char"))}, 
                object::pair{std::string("name"), std::string("Test Agent")}, 
                object::pair{std::string("bio"), array<string>{ std::string("Test bio") }}, 
                object::pair{std::string("topics"), array<any>()}, 
                object::pair{std::string("clients"), array<any>()}, 
                object::pair{std::string("plugins"), array<any>()}, 
                object::pair{std::string("settings"), object{
                    object::pair{std::string("model"), std::string("gpt-4")}, 
                    object::pair{std::string("secrets"), object{}}
                }}, 
                object::pair{std::string("modelProvider"), std::string("openai")}
            });
            auto db = createDatabaseAdapter(object{
                object::pair{std::string("dataDir"), testDbPath}
            }, as<std::shared_ptr<UUID>>(std::string("test-agent-123")));
            std::async([=]() { db->init(); });
            mockRuntime = std::make_shared<AgentRuntime>(as<any>(object{
                object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("test-agent-123"))}, 
                object::pair{std::string("character"), testCharacter}, 
                object::pair{std::string("adapter"), db}, 
                object::pair{std::string("token"), OR((process->env->OPENAI_API_KEY), (std::string("test-token")))}, 
                object::pair{std::string("serverUrl"), std::string("http://localhost:3000")}
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
                    object::pair{std::string("recursive"), true}, 
                    object::pair{std::string("force"), true}
                });
            }
        }
        );
        beforeEach([=]() mutable
        {
            client1 = ioClient(std::string("http://localhost:") + port + string_empty, object{
                object::pair{std::string("autoConnect"), false}, 
                object::pair{std::string("transports"), array<string>{ std::string("websocket") }}
            });
            client2 = ioClient(std::string("http://localhost:") + port + string_empty, object{
                object::pair{std::string("autoConnect"), false}, 
                object::pair{std::string("transports"), array<string>{ std::string("websocket") }}
            });
        }
        );
        afterEach([=]() mutable
        {
            if (client1->connected) client1->disconnect();
            if (client2->connected) client2->disconnect();
        }
        );
        describe(std::string("Connection and Channel Joining"), [=]() mutable
        {
            it(std::string("should establish connection and join channel"), [=]() mutable
            {
                auto connectionPromise = std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std::string("connection_established"), [=](auto data) mutable
                    {
                        expect(data)->toHaveProperty(std::string("socketId"));
                        expect(data["message"])->toContain(std::string("Connected to Eliza Socket.IO server"));
                        resolve(data);
                    }
                    );
                }
                );
                client1->connect();
                std::async([=]() { connectionPromise; });
                auto joinPromise = std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std::string("channel_joined"), [=](auto data) mutable
                    {
                        expect(data)->toHaveProperty(std::string("channelId"));
                        expect(data["message"])->toContain(std::string("successfully joined"));
                        resolve(data);
                    }
                    );
                }
                );
                auto channelId = std::string("123e4567-e89b-12d3-a456-426614174000");
                client1->emit(String(SOCKET_MESSAGE_TYPE->ROOM_JOINING), object{
                    object::pair{std::string("channelId"), std::string("channelId")}, 
                    object::pair{std::string("entityId"), std::string("user-123")}, 
                    object::pair{std::string("serverId"), std::string("00000000-0000-0000-0000-000000000000")}
                });
                std::async([=]() { joinPromise; });
            }
            );
            it(std::string("should allow multiple clients to join same channel"), [=]() mutable
            {
                std::async([=]() { Promise->all(std::tuple<std::shared_ptr<Promise<any>>, std::shared_ptr<Promise<any>>>{ std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std::string("connection_established"), resolve);
                    client1->connect();
                }
                ), std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client2->on(std::string("connection_established"), resolve);
                    client2->connect();
                }
                ) }); });
                shared channelId = std::string("123e4567-e89b-12d3-a456-426614174000");
                auto joinPromises = Promise->all(std::tuple<std::shared_ptr<Promise<any>>, std::shared_ptr<Promise<any>>>{ std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std::string("channel_joined"), resolve);
                    client1->emit(String(SOCKET_MESSAGE_TYPE->ROOM_JOINING), object{
                        object::pair{std::string("channelId"), std::string("channelId")}, 
                        object::pair{std::string("entityId"), std::string("user-1")}, 
                        object::pair{std::string("serverId"), std::string("00000000-0000-0000-0000-000000000000")}
                    });
                }
                ), std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client2->on(std::string("channel_joined"), resolve);
                    client2->emit(String(SOCKET_MESSAGE_TYPE->ROOM_JOINING), object{
                        object::pair{std::string("channelId"), std::string("channelId")}, 
                        object::pair{std::string("entityId"), std::string("user-2")}, 
                        object::pair{std::string("serverId"), std::string("00000000-0000-0000-0000-000000000000")}
                    });
                }
                ) });
                std::async([=]() { joinPromises; });
            }
            );
        }
        );
        describe(std::string("Message Sending and Broadcasting"), [=]() mutable
        {
            it(std::string("should send message and broadcast to other clients"), [=]() mutable
            {
                shared channelId = std::string("123e4567-e89b-12d3-a456-426614174000");
                shared serverId = std::string("00000000-0000-0000-0000-000000000000");
                std::async([=]() { Promise->all(std::tuple<std::shared_ptr<Promise<any>>, std::shared_ptr<Promise<any>>>{ std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std::string("connection_established"), [=]() mutable
                    {
                        client1->emit(String(SOCKET_MESSAGE_TYPE->ROOM_JOINING), object{
                            object::pair{std::string("channelId"), std::string("channelId")}, 
                            object::pair{std::string("entityId"), std::string("user-1")}, 
                            object::pair{std::string("serverId"), std::string("serverId")}
                        });
                    }
                    );
                    client1->on(std::string("channel_joined"), resolve);
                    client1->connect();
                }
                ), std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client2->on(std::string("connection_established"), [=]() mutable
                    {
                        client2->emit(String(SOCKET_MESSAGE_TYPE->ROOM_JOINING), object{
                            object::pair{std::string("channelId"), std::string("channelId")}, 
                            object::pair{std::string("entityId"), std::string("user-2")}, 
                            object::pair{std::string("serverId"), std::string("serverId")}
                        });
                    }
                    );
                    client2->on(std::string("channel_joined"), resolve);
                    client2->connect();
                }
                ) }); });
                auto messageReceived = std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client2->on(std::string("messageBroadcast"), [=](auto message) mutable
                    {
                        expect(message)->toHaveProperty(std::string("id"));
                        expect(message["text"])->toBe(std::string("Hello from client1"));
                        expect(message["senderId"])->toBe(std::string("user-1-id"));
                        expect(message["channelId"])->toBe(channelId);
                        resolve(message);
                    }
                    );
                }
                );
                auto ackReceived = std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std::string("messageAck"), [=](auto ack) mutable
                    {
                        expect(ack["status"])->toBe(std::string("received_by_server_and_processing"));
                        resolve(ack);
                    }
                    );
                }
                );
                client1->emit(String(SOCKET_MESSAGE_TYPE->SEND_MESSAGE), object{
                    object::pair{std::string("channelId"), std::string("channelId")}, 
                    object::pair{std::string("senderId"), std::string("user-1-id")}, 
                    object::pair{std::string("senderName"), std::string("User 1")}, 
                    object::pair{std::string("message"), std::string("Hello from client1")}, 
                    object::pair{std::string("serverId"), std::string("serverId")}, 
                    object::pair{std::string("messageId"), std::string("client-msg-1")}
                });
                std::async([=]() { Promise->all(std::tuple<std::shared_ptr<Promise<any>>, std::shared_ptr<Promise<any>>>{ messageReceived, ackReceived }); });
            }
            );
            it(std::string("should handle message with attachments"), [=]() mutable
            {
                shared channelId = std::string("456e7890-e89b-12d3-a456-426614174000");
                shared serverId = std::string("00000000-0000-0000-0000-000000000000");
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std::string("connection_established"), [=]() mutable
                    {
                        client1->emit(String(SOCKET_MESSAGE_TYPE->ROOM_JOINING), object{
                            object::pair{std::string("channelId"), std::string("channelId")}, 
                            object::pair{std::string("entityId"), std::string("user-1")}, 
                            object::pair{std::string("serverId"), std::string("serverId")}
                        });
                    }
                    );
                    client1->on(std::string("channel_joined"), resolve);
                    client1->connect();
                }
                ); });
                auto ackReceived = std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std::string("messageAck"), resolve);
                }
                );
                auto messageBroadcast = std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std::string("messageBroadcast"), [=](auto message) mutable
                    {
                        expect(message["attachments"])->toHaveLength(1);
                        expect(const_(message["attachments"])[0])->toEqual(object{
                            object::pair{std::string("url"), std::string("https://example.com/image.jpg")}, 
                            object::pair{std::string("type"), std::string("image")}
                        });
                        resolve(message);
                    }
                    );
                }
                );
                client1->emit(String(SOCKET_MESSAGE_TYPE->SEND_MESSAGE), object{
                    object::pair{std::string("channelId"), std::string("channelId")}, 
                    object::pair{std::string("senderId"), std::string("user-1-id")}, 
                    object::pair{std::string("senderName"), std::string("User 1")}, 
                    object::pair{std::string("message"), std::string("Check out this image")}, 
                    object::pair{std::string("serverId"), std::string("serverId")}, 
                    object::pair{std::string("attachments"), array<object>{ object{
                        object::pair{std::string("url"), std::string("https://example.com/image.jpg")}, 
                        object::pair{std::string("type"), std::string("image")}
                    } }}
                });
                std::async([=]() { Promise->all(std::tuple<std::shared_ptr<Promise<any>>, std::shared_ptr<Promise<any>>>{ ackReceived, messageBroadcast }); });
            }
            );
        }
        );
        describe(std::string("DM Channel Creation and Messaging"), [=]() mutable
        {
            it(std::string("should auto-create DM channel and send message"), [=]() mutable
            {
                auto channelId = std::string("789e1234-e89b-12d3-a456-426614174000");
                auto serverId = std::string("00000000-0000-0000-0000-000000000000");
                auto user1Id = std::string("111e2222-e89b-12d3-a456-426614174000");
                auto user2Id = std::string("222e3333-e89b-12d3-a456-426614174000");
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std::string("connection_established"), resolve);
                    client1->connect();
                }
                ); });
                auto ackReceived = std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std::string("messageAck"), [=](auto ack) mutable
                    {
                        expect(ack["status"])->toBe(std::string("received_by_server_and_processing"));
                        resolve(ack);
                    }
                    );
                }
                );
                client1->emit(String(SOCKET_MESSAGE_TYPE->SEND_MESSAGE), object{
                    object::pair{std::string("channelId"), std::string("channelId")}, 
                    object::pair{std::string("senderId"), user1Id}, 
                    object::pair{std::string("senderName"), std::string("User 1")}, 
                    object::pair{std::string("message"), std::string("Hello, this is a DM")}, 
                    object::pair{std::string("serverId"), std::string("serverId")}, 
                    object::pair{std::string("targetUserId"), user2Id}, 
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("isDm"), true}, 
                        object::pair{std::string("channelType"), ChannelType->DM}
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
        describe(std::string("Error Handling and Edge Cases"), [=]() mutable
        {
            it(std::string("should handle invalid channel ID gracefully"), [=]() mutable
            {
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std::string("connection_established"), resolve);
                    client1->connect();
                }
                ); });
                auto errorReceived = std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std::string("messageError"), [=](auto error) mutable
                    {
                        expect(error["error"])->toContain(std::string("channelId is required"));
                        resolve(error);
                    }
                    );
                }
                );
                client1->emit(String(SOCKET_MESSAGE_TYPE->ROOM_JOINING), object{
                    object::pair{std::string("entityId"), std::string("user-1")}, 
                    object::pair{std::string("serverId"), std::string("00000000-0000-0000-0000-000000000000")}
                });
                std::async([=]() { errorReceived; });
            }
            );
            it(std::string("should handle message without required fields"), [=]() mutable
            {
                auto channelId = std::string("999e4567-e89b-12d3-a456-426614174000");
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std::string("connection_established"), resolve);
                    client1->connect();
                }
                ); });
                auto errorReceived = std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std::string("messageError"), [=](auto error) mutable
                    {
                        expect(error["error"])->toContain(std::string("required"));
                        resolve(error);
                    }
                    );
                }
                );
                client1->emit(String(SOCKET_MESSAGE_TYPE->SEND_MESSAGE), object{
                    object::pair{std::string("channelId"), std::string("channelId")}
                });
                std::async([=]() { errorReceived; });
            }
            );
            it(std::string("should handle disconnection and cleanup"), [=]() mutable
            {
                shared channelId = std::string("888e4567-e89b-12d3-a456-426614174000");
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std::string("connection_established"), [=]() mutable
                    {
                        client1->emit(String(SOCKET_MESSAGE_TYPE->ROOM_JOINING), object{
                            object::pair{std::string("channelId"), std::string("channelId")}, 
                            object::pair{std::string("entityId"), std::string("user-1")}, 
                            object::pair{std::string("serverId"), std::string("00000000-0000-0000-0000-000000000000")}
                        });
                    }
                    );
                    client1->on(std::string("channel_joined"), resolve);
                    client1->connect();
                }
                ); });
                auto disconnectPromise = std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std::string("disconnect"), resolve);
                    client1->disconnect();
                }
                );
                std::async([=]() { disconnectPromise; });
                expect(client1->connected)->toBe(false);
            }
            );
        }
        );
        describe(std::string("Log Streaming"), [=]() mutable
        {
            it(std::string("should subscribe to log stream and receive filtered logs"), [=]() mutable
            {
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std::string("connection_established"), resolve);
                    client1->connect();
                }
                ); });
                auto subscriptionConfirmed = std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std::string("log_subscription_confirmed"), [=](auto data) mutable
                    {
                        expect(data["subscribed"])->toBe(true);
                        resolve(data);
                    }
                    );
                }
                );
                client1->emit(std::string("subscribe_logs"));
                std::async([=]() { subscriptionConfirmed; });
                auto filtersUpdated = std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std::string("log_filters_updated"), [=](auto data) mutable
                    {
                        expect(data["success"])->toBe(true);
                        expect(data["filters"])->toMatchObject(object{
                            object::pair{std::string("agentName"), std::string("Test Agent")}, 
                            object::pair{std::string("level"), std::string("info")}
                        });
                        resolve(data);
                    }
                    );
                }
                );
                client1->emit(std::string("update_log_filters"), object{
                    object::pair{std::string("agentName"), std::string("Test Agent")}, 
                    object::pair{std::string("level"), std::string("info")}
                });
                std::async([=]() { filtersUpdated; });
            }
            );
            it(std::string("should unsubscribe from log stream"), [=]() mutable
            {
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std::string("connection_established"), resolve);
                    client1->connect();
                }
                ); });
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std::string("log_subscription_confirmed"), resolve);
                    client1->emit(std::string("subscribe_logs"));
                }
                ); });
                auto unsubscribeConfirmed = std::make_shared<Promise>([=](auto resolve) mutable
                {
                    client1->on(std::string("log_subscription_confirmed"), [=](auto data) mutable
                    {
                        expect(data["subscribed"])->toBe(false);
                        resolve(data);
                    }
                    );
                }
                );
                client1->emit(std::string("unsubscribe_logs"));
                std::async([=]() { unsubscribeConfirmed; });
            }
            );
        }
        );
    }
    );
}

MAIN
