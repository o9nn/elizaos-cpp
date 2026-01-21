#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/api-client/src/__tests__/services/messaging.test.h"

void Main(void)
{
    describe(std::string("MessagingService"), [=]() mutable
    {
        shared<std::shared_ptr<MessagingService>> messagingService;
        shared mockConfig = object{
            object::pair{std::string("baseUrl"), std::string("http://localhost:3000")}, 
            object::pair{std::string("apiKey"), std::string("test-key")}
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
        describe(std::string("constructor"), [=]() mutable
        {
            it(std::string("should create an instance with valid configuration"), [=]() mutable
            {
                expect(messagingService)->toBeInstanceOf(MessagingService);
            }
            );
            it(std::string("should throw error when initialized with invalid configuration"), [=]() mutable
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
        describe(std::string("submitMessage"), [=]() mutable
        {
            shared mockParams = object{
                object::pair{std::string("agentId"), as<any>(std::string("agent-123"))}, 
                object::pair{std::string("channelId"), as<any>(std::string("channel-456"))}, 
                object::pair{std::string("content"), std::string("Test message")}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("source"), std::string("test")}
                }}
            };
            it(std::string("should submit message successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("id"), std::string("msg-789")}, 
                    object::pair{std::string("content"), std::string("Test message")}
                };
                (as<any>(messagingService))["post"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->submitMessage(mockParams); });
                expect((as<any>(messagingService))["post"])->toHaveBeenCalledWith(std::string("/api/messaging/submit"), mockParams);
                expect(result)->toEqual(mockResponse);
            }
            );
            it(std::string("should handle submission errors"), [=]() mutable
            {
                (as<any>(messagingService))["post"]["mockRejectedValue"](std::make_shared<Error>(std::string("Submission failed")));
                std::async([=]() { expect(messagingService->submitMessage(mockParams))->rejects->toThrow(std::string("Submission failed")); });
            }
            );
        }
        );
        describe(std::string("completeMessage"), [=]() mutable
        {
            shared mockParams = object{
                object::pair{std::string("messageId"), as<any>(std::string("msg-123"))}, 
                object::pair{std::string("status"), as<any>(std::string("completed"))}
            };
            it(std::string("should complete message successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("success"), true}
                };
                (as<any>(messagingService))["post"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->completeMessage(mockParams); });
                expect((as<any>(messagingService))["post"])->toHaveBeenCalledWith(std::string("/api/messaging/complete"), mockParams);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("ingestExternalMessages"), [=]() mutable
        {
            shared mockParams = object{
                object::pair{std::string("platform"), std::string("discord")}, 
                object::pair{std::string("channelId"), std::string("external-channel-123")}, 
                object::pair{std::string("messages"), array<object>{ object{
                    object::pair{std::string("id"), std::string("ext-msg-1")}, 
                    object::pair{std::string("authorId"), std::string("ext-user-1")}, 
                    object::pair{std::string("content"), std::string("External message")}, 
                    object::pair{std::string("timestamp"), Date->now()}, 
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("platform"), std::string("discord")}
                    }}
                } }}
            };
            it(std::string("should ingest external messages successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("processed"), 1}
                };
                (as<any>(messagingService))["post"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->ingestExternalMessages(mockParams); });
                expect((as<any>(messagingService))["post"])->toHaveBeenCalledWith(std::string("/api/messaging/ingest-external"), mockParams);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("createChannel"), [=]() mutable
        {
            shared mockParams = object{
                object::pair{std::string("name"), std::string("New Channel")}, 
                object::pair{std::string("type"), as<any>(std::string("public"))}, 
                object::pair{std::string("serverId"), as<any>(std::string("server-123"))}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("description"), std::string("A new channel")}
                }}
            };
            it(std::string("should create channel successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("id"), std::string("channel-new")}, 
                    object::pair{std::string("name"), std::string("New Channel")}
                };
                (as<any>(messagingService))["post"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->createChannel(mockParams); });
                expect((as<any>(messagingService))["post"])->toHaveBeenCalledWith(std::string("/api/messaging/central-channels"), mockParams);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("createGroupChannel"), [=]() mutable
        {
            shared mockParams = object{
                object::pair{std::string("name"), std::string("Group Channel")}, 
                object::pair{std::string("participantIds"), as<array<any>>(array<any>{ std::string("user-1"), std::string("user-2") })}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("type"), std::string("group")}
                }}
            };
            it(std::string("should create group channel successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("id"), std::string("channel-group")}, 
                    object::pair{std::string("name"), std::string("Group Channel")}
                };
                (as<any>(messagingService))["post"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->createGroupChannel(mockParams); });
                expect((as<any>(messagingService))["post"])->toHaveBeenCalledWith(std::string("/api/messaging/central-channels"), mockParams);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("getOrCreateDmChannel"), [=]() mutable
        {
            shared mockParams = object{
                object::pair{std::string("participantIds"), as<std::tuple<any, any>>(std::tuple<string, string>{ std::string("user-1"), std::string("user-2") })}
            };
            it(std::string("should get or create DM channel successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("id"), std::string("channel-dm")}, 
                    object::pair{std::string("name"), std::string("DM Channel")}
                };
                (as<any>(messagingService))["get"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->getOrCreateDmChannel(mockParams); });
                expect((as<any>(messagingService))["get"])->toHaveBeenCalledWith(std::string("/api/messaging/dm-channel"), object{
                    object::pair{std::string("params"), mockParams}
                });
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("getChannelDetails"), [=]() mutable
        {
            shared channelId = as<any>(std::string("channel-123"));
            it(std::string("should get channel details successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("id"), channelId}, 
                    object::pair{std::string("name"), std::string("Test Channel")}, 
                    object::pair{std::string("type"), std::string("public")}
                };
                (as<any>(messagingService))["get"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->getChannelDetails(channelId); });
                expect((as<any>(messagingService))["get"])->toHaveBeenCalledWith(std::string("/api/messaging/central-channels/") + channelId + std::string("/details"));
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("getChannelParticipants"), [=]() mutable
        {
            shared channelId = as<any>(std::string("channel-123"));
            it(std::string("should get channel participants successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("participants"), array<object>{ object{
                        object::pair{std::string("id"), std::string("user-1")}, 
                        object::pair{std::string("role"), std::string("member")}
                    } }}
                };
                (as<any>(messagingService))["get"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->getChannelParticipants(channelId); });
                expect((as<any>(messagingService))["get"])->toHaveBeenCalledWith(std::string("/api/messaging/central-channels/") + channelId + std::string("/participants"));
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("addAgentToChannel"), [=]() mutable
        {
            shared channelId = as<any>(std::string("channel-123"));
            shared agentId = as<any>(std::string("agent-456"));
            it(std::string("should add agent to channel successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("success"), true}
                };
                (as<any>(messagingService))["post"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->addAgentToChannel(channelId, agentId); });
                expect((as<any>(messagingService))["post"])->toHaveBeenCalledWith(std::string("/api/messaging/central-channels/") + channelId + std::string("/agents"), object{
                    object::pair{std::string("agentId"), std::string("agentId")}
                });
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("removeAgentFromChannel"), [=]() mutable
        {
            shared channelId = as<any>(std::string("channel-123"));
            shared agentId = as<any>(std::string("agent-456"));
            it(std::string("should remove agent from channel successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("success"), true}
                };
                (as<any>(messagingService))["delete"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->removeAgentFromChannel(channelId, agentId); });
                expect((as<any>(messagingService))["delete"])->toHaveBeenCalledWith(std::string("/api/messaging/central-channels/") + channelId + std::string("/agents/") + agentId + string_empty);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("deleteChannel"), [=]() mutable
        {
            shared channelId = as<any>(std::string("channel-123"));
            it(std::string("should delete channel successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("success"), true}
                };
                (as<any>(messagingService))["delete"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->deleteChannel(channelId); });
                expect((as<any>(messagingService))["delete"])->toHaveBeenCalledWith(std::string("/api/messaging/central-channels/") + channelId + string_empty);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("clearChannelHistory"), [=]() mutable
        {
            shared channelId = as<any>(std::string("channel-123"));
            it(std::string("should clear channel history successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("deleted"), 10}
                };
                (as<any>(messagingService))["delete"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->clearChannelHistory(channelId); });
                expect((as<any>(messagingService))["delete"])->toHaveBeenCalledWith(std::string("/api/messaging/central-channels/") + channelId + std::string("/messages"));
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("postMessage"), [=]() mutable
        {
            shared channelId = as<any>(std::string("channel-123"));
            shared content = std::string("Hello world");
            shared metadata = object{
                object::pair{std::string("source"), std::string("test")}
            };
            it(std::string("should post message successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("id"), std::string("msg-new")}, 
                    object::pair{std::string("content"), std::string("content")}, 
                    object::pair{std::string("channelId"), std::string("channelId")}
                };
                (as<any>(messagingService))["post"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->postMessage(channelId, content, metadata); });
                expect((as<any>(messagingService))["post"])->toHaveBeenCalledWith(std::string("/api/messaging/central-channels/") + channelId + std::string("/messages"), object{
                    object::pair{std::string("content"), std::string("content")}, 
                    object::pair{std::string("metadata"), std::string("metadata")}
                });
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("getChannelMessages"), [=]() mutable
        {
            shared channelId = as<any>(std::string("channel-123"));
            it(std::string("should get channel messages successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("messages"), array<object>{ object{
                        object::pair{std::string("id"), std::string("msg-1")}, 
                        object::pair{std::string("content"), std::string("Hello")}
                    } }}
                };
                (as<any>(messagingService))["get"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->getChannelMessages(channelId); });
                expect((as<any>(messagingService))["get"])->toHaveBeenCalledWith(std::string("/api/messaging/central-channels/") + channelId + std::string("/messages"), object{
                    object::pair{std::string("params"), undefined}
                });
                expect(result)->toEqual(mockResponse);
            }
            );
            it(std::string("should handle pagination parameters"), [=]() mutable
            {
                auto params = object{
                    object::pair{std::string("limit"), 10}, 
                    object::pair{std::string("offset"), 20}
                };
                (as<any>(messagingService))["get"]["mockResolvedValue"](object{
                    object::pair{std::string("messages"), array<any>()}
                });
                std::async([=]() { messagingService->getChannelMessages(channelId, params); });
                expect((as<any>(messagingService))["get"])->toHaveBeenCalledWith(std::string("/api/messaging/central-channels/") + channelId + std::string("/messages"), object{
                    object::pair{std::string("params"), std::string("params")}
                });
            }
            );
        }
        );
        describe(std::string("getMessage"), [=]() mutable
        {
            shared messageId = as<any>(std::string("msg-123"));
            it(std::string("should get message successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("id"), messageId}, 
                    object::pair{std::string("content"), std::string("Test message")}
                };
                (as<any>(messagingService))["get"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->getMessage(messageId); });
                expect((as<any>(messagingService))["get"])->toHaveBeenCalledWith(std::string("/api/messaging/messages/") + messageId + string_empty);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("deleteMessage"), [=]() mutable
        {
            shared channelId = as<any>(std::string("channel-123"));
            shared messageId = as<any>(std::string("msg-123"));
            it(std::string("should delete message successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("success"), true}
                };
                (as<any>(messagingService))["delete"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->deleteMessage(channelId, messageId); });
                expect((as<any>(messagingService))["delete"])->toHaveBeenCalledWith(std::string("/api/messaging/central-channels/") + channelId + std::string("/messages/") + messageId + string_empty);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("updateMessage"), [=]() mutable
        {
            shared messageId = as<any>(std::string("msg-123"));
            shared content = std::string("Updated content");
            it(std::string("should update message successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("id"), messageId}, 
                    object::pair{std::string("content"), std::string("content")}
                };
                (as<any>(messagingService))["patch"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->updateMessage(messageId, content); });
                expect((as<any>(messagingService))["patch"])->toHaveBeenCalledWith(std::string("/api/messaging/messages/") + messageId + string_empty, object{
                    object::pair{std::string("content"), std::string("content")}
                });
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("searchMessages"), [=]() mutable
        {
            shared mockParams = object{
                object::pair{std::string("query"), std::string("search term")}, 
                object::pair{std::string("channelId"), as<any>(std::string("channel-123"))}, 
                object::pair{std::string("limit"), 10}
            };
            it(std::string("should search messages successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("messages"), array<object>{ object{
                        object::pair{std::string("id"), std::string("msg-1")}, 
                        object::pair{std::string("content"), std::string("Found message")}
                    } }}
                };
                (as<any>(messagingService))["post"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->searchMessages(mockParams); });
                expect((as<any>(messagingService))["post"])->toHaveBeenCalledWith(std::string("/api/messaging/messages/search"), mockParams);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("listServers"), [=]() mutable
        {
            it(std::string("should list servers successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("servers"), array<object>{ object{
                        object::pair{std::string("id"), std::string("server-1")}, 
                        object::pair{std::string("name"), std::string("Test Server")}
                    } }}
                };
                (as<any>(messagingService))["get"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->listServers(); });
                expect((as<any>(messagingService))["get"])->toHaveBeenCalledWith(std::string("/api/messaging/central-servers"));
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("getServerChannels"), [=]() mutable
        {
            shared serverId = as<any>(std::string("server-123"));
            it(std::string("should get server channels successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("channels"), array<object>{ object{
                        object::pair{std::string("id"), std::string("channel-1")}, 
                        object::pair{std::string("name"), std::string("General")}
                    } }}
                };
                (as<any>(messagingService))["get"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->getServerChannels(serverId); });
                expect((as<any>(messagingService))["get"])->toHaveBeenCalledWith(std::string("/api/messaging/central-servers/") + serverId + std::string("/channels"));
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("createServer"), [=]() mutable
        {
            shared mockParams = object{
                object::pair{std::string("name"), std::string("New Server")}, 
                object::pair{std::string("sourceType"), std::string("discord")}, 
                object::pair{std::string("sourceId"), std::string("discord-server-123")}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("description"), std::string("A new server")}
                }}
            };
            it(std::string("should create server successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("id"), std::string("server-new")}, 
                    object::pair{std::string("name"), std::string("New Server")}
                };
                (as<any>(messagingService))["post"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->createServer(mockParams); });
                expect((as<any>(messagingService))["post"])->toHaveBeenCalledWith(std::string("/api/messaging/servers"), mockParams);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("syncServerChannels"), [=]() mutable
        {
            shared serverId = as<any>(std::string("server-123"));
            shared mockParams = object{
                object::pair{std::string("channels"), array<object>{ object{
                    object::pair{std::string("name"), std::string("general")}, 
                    object::pair{std::string("type"), as<any>(std::string("public"))}, 
                    object::pair{std::string("sourceId"), std::string("discord-channel-1")}
                }, object{
                    object::pair{std::string("name"), std::string("private")}, 
                    object::pair{std::string("type"), as<any>(std::string("private"))}, 
                    object::pair{std::string("sourceId"), std::string("discord-channel-2")}
                } }}
            };
            it(std::string("should sync server channels successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("synced"), 2}
                };
                (as<any>(messagingService))["post"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->syncServerChannels(serverId, mockParams); });
                expect((as<any>(messagingService))["post"])->toHaveBeenCalledWith(std::string("/api/messaging/servers/") + serverId + std::string("/sync-channels"), mockParams);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("deleteServer"), [=]() mutable
        {
            shared serverId = as<any>(std::string("server-123"));
            it(std::string("should delete server successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("success"), true}
                };
                (as<any>(messagingService))["delete"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { messagingService->deleteServer(serverId); });
                expect((as<any>(messagingService))["delete"])->toHaveBeenCalledWith(std::string("/api/messaging/servers/") + serverId + string_empty);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("error handling"), [=]() mutable
        {
            it(std::string("should handle network errors"), [=]() mutable
            {
                (as<any>(messagingService))["get"]["mockRejectedValue"](std::make_shared<Error>(std::string("Network error")));
                std::async([=]() { expect(messagingService->listServers())->rejects->toThrow(std::string("Network error")); });
            }
            );
            it(std::string("should handle API errors"), [=]() mutable
            {
                (as<any>(messagingService))["post"]["mockRejectedValue"](std::make_shared<Error>(std::string("API error")));
                auto params = object{
                    object::pair{std::string("agentId"), as<any>(std::string("agent-123"))}, 
                    object::pair{std::string("channelId"), as<any>(std::string("channel-456"))}, 
                    object::pair{std::string("content"), std::string("Test message")}
                };
                std::async([=]() { expect(messagingService->submitMessage(params))->rejects->toThrow(std::string("API error")); });
            }
            );
        }
        );
    }
    );
}

MAIN
