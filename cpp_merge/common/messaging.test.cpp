#include "messaging.test.h"

void Main(void)
{
    describe(std:("MessagingService"), [=]() mutable
    {
        shared<std::shared_ptr<MessagingService>> messagingService;
        shared mockConfig = object{
            object::pair{std:("baseUrl"), std:("http://localhost:3000")}, 
            object::pair{std:("apiKey"), std:("test-key")}
        };
        beforeEach([=]() mutable
        {
            messagingService = std::make_shared<MessagingService>(mockConfig);
            (as<any>(messagingService))["get"] = mock([=]() mutable
            {
                return Promise->resolve(object{});
            }
            );
            (as<any>(messagingService))["post"] = mock([=]() mutable
            {
                return Promise->resolve(object{});
            }
            );
            (as<any>(messagingService))["patch"] = mock([=]() mutable
            {
                return Promise->resolve(object{});
            }
            );
            (as<any>(messagingService))["delete"] = mock([=]() mutable
            {
                return Promise->resolve(object{});
            }
            );
        }
        );
        afterEach([=]() mutable
        {
            auto getMock = (as<any>(messagingService))["get"];
            auto postMock = (as<any>(messagingService))["post"];
            auto patchMock = (as<any>(messagingService))["patch"];
            auto deleteMock = (as<any>(messagingService))["delete"];
            if (getMock["mockClear"]) getMock["mockClear"]();
            if (postMock["mockClear"]) postMock["mockClear"]();
            if (patchMock["mockClear"]) patchMock["mockClear"]();
            if (deleteMock["mockClear"]) deleteMock["mockClear"]();
        }
        );
        describe(std:("constructor"), [=]() mutable
        {
            it(std:("should create an instance with valid configuration"), [=]() mutable
            {
                expect(messagingService)->toBeInstanceOf(MessagingService);
            }
            );
            it(std:("should throw error when initialized with invalid configuration"), [=]() mutable
            {
                expect([=]() mutable
                {
                    return std::make_shared<MessagingService>(as<any>(nullptr));
                }
                )->toThrow();
            }
            );
        }
        );
        describe(std:("submitMessage"), [=]() mutable
        {
            shared mockParams = object{
                object::pair{std:("agentId"), as<any>(std:("agent-123"))}, 
                object::pair{std:("channelId"), as<any>(std:("channel-456"))}, 
                object::pair{std:("content"), std:("Test message")}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("source"), std:("test")}
                }}
            };
            it(std:("should submit message successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("id"), std:("msg-789")}, 
                    object::pair{std:("content"), std:("Test message")}
                };
                (as<any>(messagingService))["post"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->submitMessage(mockParams); });
                expect((as<any>(messagingService))["post"])->toHaveBeenCalledWith(std:("/api/messaging/submit"), mockParams);
                expect(result)->toEqual(mockResponse);
            }
            );
            it(std:("should handle submission errors"), [=]() mutable
            {
                (as<any>(messagingService))["post"]["mockRejectedValue"](std::make_shared<Error>(std:("Submission failed")));
                std::async([=]() { expect(messagingService->submitMessage(mockParams))->rejects->toThrow(std:("Submission failed")); });
            }
            );
        }
        );
        describe(std:("completeMessage"), [=]() mutable
        {
            shared mockParams = object{
                object::pair{std:("messageId"), as<any>(std:("msg-123"))}, 
                object::pair{std:("status"), as<any>(std:("completed"))}
            };
            it(std:("should complete message successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("success"), true}
                };
                (as<any>(messagingService))["post"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->completeMessage(mockParams); });
                expect((as<any>(messagingService))["post"])->toHaveBeenCalledWith(std:("/api/messaging/complete"), mockParams);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("ingestExternalMessages"), [=]() mutable
        {
            shared mockParams = object{
                object::pair{std:("platform"), std:("discord")}, 
                object::pair{std:("channelId"), std:("external-channel-123")}, 
                object::pair{std:("messages"), array<object>{ object{
                    object::pair{std:("id"), std:("ext-msg-1")}, 
                    object::pair{std:("authorId"), std:("ext-user-1")}, 
                    object::pair{std:("content"), std:("External message")}, 
                    object::pair{std:("timestamp"), Date->now()}, 
                    object::pair{std:("metadata"), object{
                        object::pair{std:("platform"), std:("discord")}
                    }}
                } }}
            };
            it(std:("should ingest external messages successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("processed"), 1}
                };
                (as<any>(messagingService))["post"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->ingestExternalMessages(mockParams); });
                expect((as<any>(messagingService))["post"])->toHaveBeenCalledWith(std:("/api/messaging/ingest-external"), mockParams);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("createChannel"), [=]() mutable
        {
            shared mockParams = object{
                object::pair{std:("name"), std:("New Channel")}, 
                object::pair{std:("type"), as<any>(std:("public"))}, 
                object::pair{std:("serverId"), as<any>(std:("server-123"))}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("description"), std:("A new channel")}
                }}
            };
            it(std:("should create channel successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("id"), std:("channel-new")}, 
                    object::pair{std:("name"), std:("New Channel")}
                };
                (as<any>(messagingService))["post"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->createChannel(mockParams); });
                expect((as<any>(messagingService))["post"])->toHaveBeenCalledWith(std:("/api/messaging/central-channels"), mockParams);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("createGroupChannel"), [=]() mutable
        {
            shared mockParams = object{
                object::pair{std:("name"), std:("Group Channel")}, 
                object::pair{std:("participantIds"), as<array<any>>(array<any>{ std:("user-1"), std:("user-2") })}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("type"), std:("group")}
                }}
            };
            it(std:("should create group channel successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("id"), std:("channel-group")}, 
                    object::pair{std:("name"), std:("Group Channel")}
                };
                (as<any>(messagingService))["post"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->createGroupChannel(mockParams); });
                expect((as<any>(messagingService))["post"])->toHaveBeenCalledWith(std:("/api/messaging/central-channels"), mockParams);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("getOrCreateDmChannel"), [=]() mutable
        {
            shared mockParams = object{
                object::pair{std:("participantIds"), as<std::tuple<any, any>>(std::tuple<string, string>{ std:("user-1"), std:("user-2") })}
            };
            it(std:("should get or create DM channel successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("id"), std:("channel-dm")}, 
                    object::pair{std:("name"), std:("DM Channel")}
                };
                (as<any>(messagingService))["get"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->getOrCreateDmChannel(mockParams); });
                expect((as<any>(messagingService))["get"])->toHaveBeenCalledWith(std:("/api/messaging/dm-channel"), object{
                    object::pair{std:("params"), mockParams}
                });
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("getChannelDetails"), [=]() mutable
        {
            shared channelId = as<any>(std:("channel-123"));
            it(std:("should get channel details successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("id"), channelId}, 
                    object::pair{std:("name"), std:("Test Channel")}, 
                    object::pair{std:("type"), std:("public")}
                };
                (as<any>(messagingService))["get"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->getChannelDetails(channelId); });
                expect((as<any>(messagingService))["get"])->toHaveBeenCalledWith(std:("/api/messaging/central-channels/") + channelId + std:("/details"));
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("getChannelParticipants"), [=]() mutable
        {
            shared channelId = as<any>(std:("channel-123"));
            it(std:("should get channel participants successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("participants"), array<object>{ object{
                        object::pair{std:("id"), std:("user-1")}, 
                        object::pair{std:("role"), std:("member")}
                    } }}
                };
                (as<any>(messagingService))["get"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->getChannelParticipants(channelId); });
                expect((as<any>(messagingService))["get"])->toHaveBeenCalledWith(std:("/api/messaging/central-channels/") + channelId + std:("/participants"));
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("addAgentToChannel"), [=]() mutable
        {
            shared channelId = as<any>(std:("channel-123"));
            shared agentId = as<any>(std:("agent-456"));
            it(std:("should add agent to channel successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("success"), true}
                };
                (as<any>(messagingService))["post"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->addAgentToChannel(channelId, agentId); });
                expect((as<any>(messagingService))["post"])->toHaveBeenCalledWith(std:("/api/messaging/central-channels/") + channelId + std:("/agents"), object{
                    object::pair{std:("agentId"), std:("agentId")}
                });
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("removeAgentFromChannel"), [=]() mutable
        {
            shared channelId = as<any>(std:("channel-123"));
            shared agentId = as<any>(std:("agent-456"));
            it(std:("should remove agent from channel successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("success"), true}
                };
                (as<any>(messagingService))["delete"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->removeAgentFromChannel(channelId, agentId); });
                expect((as<any>(messagingService))["delete"])->toHaveBeenCalledWith(std:("/api/messaging/central-channels/") + channelId + std:("/agents/") + agentId + string_empty);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("deleteChannel"), [=]() mutable
        {
            shared channelId = as<any>(std:("channel-123"));
            it(std:("should delete channel successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("success"), true}
                };
                (as<any>(messagingService))["delete"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->deleteChannel(channelId); });
                expect((as<any>(messagingService))["delete"])->toHaveBeenCalledWith(std:("/api/messaging/central-channels/") + channelId + string_empty);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("clearChannelHistory"), [=]() mutable
        {
            shared channelId = as<any>(std:("channel-123"));
            it(std:("should clear channel history successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("deleted"), 10}
                };
                (as<any>(messagingService))["delete"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->clearChannelHistory(channelId); });
                expect((as<any>(messagingService))["delete"])->toHaveBeenCalledWith(std:("/api/messaging/central-channels/") + channelId + std:("/messages"));
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("postMessage"), [=]() mutable
        {
            shared channelId = as<any>(std:("channel-123"));
            shared content = std:("Hello world");
            shared metadata = object{
                object::pair{std:("source"), std:("test")}
            };
            it(std:("should post message successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("id"), std:("msg-new")}, 
                    object::pair{std:("content"), std:("content")}, 
                    object::pair{std:("channelId"), std:("channelId")}
                };
                (as<any>(messagingService))["post"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->postMessage(channelId, content, metadata); });
                expect((as<any>(messagingService))["post"])->toHaveBeenCalledWith(std:("/api/messaging/central-channels/") + channelId + std:("/messages"), object{
                    object::pair{std:("content"), std:("content")}, 
                    object::pair{std:("metadata"), std:("metadata")}
                });
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("getChannelMessages"), [=]() mutable
        {
            shared channelId = as<any>(std:("channel-123"));
            it(std:("should get channel messages successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("messages"), array<object>{ object{
                        object::pair{std:("id"), std:("msg-1")}, 
                        object::pair{std:("content"), std:("Hello")}
                    } }}
                };
                (as<any>(messagingService))["get"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->getChannelMessages(channelId); });
                expect((as<any>(messagingService))["get"])->toHaveBeenCalledWith(std:("/api/messaging/central-channels/") + channelId + std:("/messages"), object{
                    object::pair{std:("params"), undefined}
                });
                expect(result)->toEqual(mockResponse);
            }
            );
            it(std:("should handle pagination parameters"), [=]() mutable
            {
                auto params = object{
                    object::pair{std:("limit"), 10}, 
                    object::pair{std:("offset"), 20}
                };
                (as<any>(messagingService))["get"]["mockResolvedValue"](object{
                    object::pair{std:("messages"), array<any>()}
                });
                std::async([=]() { messagingService->getChannelMessages(channelId, params); });
                expect((as<any>(messagingService))["get"])->toHaveBeenCalledWith(std:("/api/messaging/central-channels/") + channelId + std:("/messages"), object{
                    object::pair{std:("params"), std:("params")}
                });
            }
            );
        }
        );
        describe(std:("getMessage"), [=]() mutable
        {
            shared messageId = as<any>(std:("msg-123"));
            it(std:("should get message successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("id"), messageId}, 
                    object::pair{std:("content"), std:("Test message")}
                };
                (as<any>(messagingService))["get"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->getMessage(messageId); });
                expect((as<any>(messagingService))["get"])->toHaveBeenCalledWith(std:("/api/messaging/messages/") + messageId + string_empty);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("deleteMessage"), [=]() mutable
        {
            shared channelId = as<any>(std:("channel-123"));
            shared messageId = as<any>(std:("msg-123"));
            it(std:("should delete message successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("success"), true}
                };
                (as<any>(messagingService))["delete"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->deleteMessage(channelId, messageId); });
                expect((as<any>(messagingService))["delete"])->toHaveBeenCalledWith(std:("/api/messaging/central-channels/") + channelId + std:("/messages/") + messageId + string_empty);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("updateMessage"), [=]() mutable
        {
            shared messageId = as<any>(std:("msg-123"));
            shared content = std:("Updated content");
            it(std:("should update message successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("id"), messageId}, 
                    object::pair{std:("content"), std:("content")}
                };
                (as<any>(messagingService))["patch"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->updateMessage(messageId, content); });
                expect((as<any>(messagingService))["patch"])->toHaveBeenCalledWith(std:("/api/messaging/messages/") + messageId + string_empty, object{
                    object::pair{std:("content"), std:("content")}
                });
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("searchMessages"), [=]() mutable
        {
            shared mockParams = object{
                object::pair{std:("query"), std:("search term")}, 
                object::pair{std:("channelId"), as<any>(std:("channel-123"))}, 
                object::pair{std:("limit"), 10}
            };
            it(std:("should search messages successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("messages"), array<object>{ object{
                        object::pair{std:("id"), std:("msg-1")}, 
                        object::pair{std:("content"), std:("Found message")}
                    } }}
                };
                (as<any>(messagingService))["post"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->searchMessages(mockParams); });
                expect((as<any>(messagingService))["post"])->toHaveBeenCalledWith(std:("/api/messaging/messages/search"), mockParams);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("listServers"), [=]() mutable
        {
            it(std:("should list servers successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("servers"), array<object>{ object{
                        object::pair{std:("id"), std:("server-1")}, 
                        object::pair{std:("name"), std:("Test Server")}
                    } }}
                };
                (as<any>(messagingService))["get"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->listServers(); });
                expect((as<any>(messagingService))["get"])->toHaveBeenCalledWith(std:("/api/messaging/central-servers"));
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("getServerChannels"), [=]() mutable
        {
            shared serverId = as<any>(std:("server-123"));
            it(std:("should get server channels successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("channels"), array<object>{ object{
                        object::pair{std:("id"), std:("channel-1")}, 
                        object::pair{std:("name"), std:("General")}
                    } }}
                };
                (as<any>(messagingService))["get"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->getServerChannels(serverId); });
                expect((as<any>(messagingService))["get"])->toHaveBeenCalledWith(std:("/api/messaging/central-servers/") + serverId + std:("/channels"));
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("createServer"), [=]() mutable
        {
            shared mockParams = object{
                object::pair{std:("name"), std:("New Server")}, 
                object::pair{std:("sourceType"), std:("discord")}, 
                object::pair{std:("sourceId"), std:("discord-server-123")}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("description"), std:("A new server")}
                }}
            };
            it(std:("should create server successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("id"), std:("server-new")}, 
                    object::pair{std:("name"), std:("New Server")}
                };
                (as<any>(messagingService))["post"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->createServer(mockParams); });
                expect((as<any>(messagingService))["post"])->toHaveBeenCalledWith(std:("/api/messaging/servers"), mockParams);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("syncServerChannels"), [=]() mutable
        {
            shared serverId = as<any>(std:("server-123"));
            shared mockParams = object{
                object::pair{std:("channels"), array<object>{ object{
                    object::pair{std:("name"), std:("general")}, 
                    object::pair{std:("type"), as<any>(std:("public"))}, 
                    object::pair{std:("sourceId"), std:("discord-channel-1")}
                }, object{
                    object::pair{std:("name"), std:("private")}, 
                    object::pair{std:("type"), as<any>(std:("private"))}, 
                    object::pair{std:("sourceId"), std:("discord-channel-2")}
                } }}
            };
            it(std:("should sync server channels successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("synced"), 2}
                };
                (as<any>(messagingService))["post"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->syncServerChannels(serverId, mockParams); });
                expect((as<any>(messagingService))["post"])->toHaveBeenCalledWith(std:("/api/messaging/servers/") + serverId + std:("/sync-channels"), mockParams);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("deleteServer"), [=]() mutable
        {
            shared serverId = as<any>(std:("server-123"));
            it(std:("should delete server successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("success"), true}
                };
                (as<any>(messagingService))["delete"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->deleteServer(serverId); });
                expect((as<any>(messagingService))["delete"])->toHaveBeenCalledWith(std:("/api/messaging/servers/") + serverId + string_empty);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("error handling"), [=]() mutable
        {
            it(std:("should handle network errors"), [=]() mutable
            {
                (as<any>(messagingService))["get"]["mockRejectedValue"](std::make_shared<Error>(std:("Network error")));
                std::async([=]() { expect(messagingService->listServers())->rejects->toThrow(std:("Network error")); });
            }
            );
            it(std:("should handle API errors"), [=]() mutable
            {
                (as<any>(messagingService))["post"]["mockRejectedValue"](std::make_shared<Error>(std:("API error")));
                auto params = object{
                    object::pair{std:("agentId"), as<any>(std:("agent-123"))}, 
                    object::pair{std:("channelId"), as<any>(std:("channel-456"))}, 
                    object::pair{std:("content"), std:("Test message")}
                };
                std::async([=]() { expect(messagingService->submitMessage(params))->rejects->toThrow(std:("API error")); });
            }
            );
        }
        );
    }
    );
}

MAIN
