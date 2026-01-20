#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/api-client/src/__tests__/services/memory.test.h"

any TEST_AGENT_ID = as<std::shared_ptr<UUID>>(std::string("550e8400-e29b-41d4-a716-446655440001"));
any TEST_ROOM_ID = as<std::shared_ptr<UUID>>(std::string("550e8400-e29b-41d4-a716-446655440002"));
any TEST_MEMORY_ID = as<std::shared_ptr<UUID>>(std::string("550e8400-e29b-41d4-a716-446655440003"));
any TEST_SERVER_ID = as<std::shared_ptr<UUID>>(std::string("550e8400-e29b-41d4-a716-446655440004"));

void Main(void)
{
    describe(std::string("MemoryService"), [=]() mutable
    {
        shared<std::shared_ptr<MemoryService>> memoryService;
        shared mockConfig = object{
            object::pair{std::string("baseUrl"), std::string("http://localhost:3000")}, 
            object::pair{std::string("apiKey"), std::string("test-key")}
        };
        beforeEach([=]() mutable
        {
            memoryService = std::make_shared<MemoryService>(mockConfig);
            (as<any>(memoryService))["get"] = mock([=]() mutable
            {
                return Promise->resolve(object{});
            }
            );
            (as<any>(memoryService))["post"] = mock([=]() mutable
            {
                return Promise->resolve(object{});
            }
            );
            (as<any>(memoryService))["patch"] = mock([=]() mutable
            {
                return Promise->resolve(object{});
            }
            );
            (as<any>(memoryService))["delete"] = mock([=]() mutable
            {
                return Promise->resolve(object{});
            }
            );
        }
        );
        afterEach([=]() mutable
        {
            auto getMock = (as<any>(memoryService))["get"];
            auto postMock = (as<any>(memoryService))["post"];
            auto patchMock = (as<any>(memoryService))["patch"];
            auto deleteMock = (as<any>(memoryService))["delete"];
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
                expect(memoryService)->toBeInstanceOf(MemoryService);
            }
            );
            it(std::string("should throw error when initialized with invalid configuration"), [=]() mutable
            {
                expect([=]() mutable
                {
                    return std::make_shared<MemoryService>(as<any>(nullptr));
                }
                )->toThrow();
            }
            );
        }
        );
        describe(std::string("getAgentMemories"), [=]() mutable
        {
            it(std::string("should retrieve agent memories successfully"), [=]() mutable
            {
                auto mockMemories = object{
                    object::pair{std::string("memories"), array<object>{ object{
                        object::pair{std::string("id"), std::string("mem-1")}, 
                        object::pair{std::string("content"), std::string("Memory 1")}, 
                        object::pair{std::string("timestamp"), std::string("2024-01-01T00:00:00Z")}
                    }, object{
                        object::pair{std::string("id"), std::string("mem-2")}, 
                        object::pair{std::string("content"), std::string("Memory 2")}, 
                        object::pair{std::string("timestamp"), std::string("2024-01-02T00:00:00Z")}
                    } }}
                };
                (as<any>(memoryService))["get"]["mockResolvedValue"](mockMemories);
                auto result = std::async([=]() { memoryService->getAgentMemories(TEST_AGENT_ID); });
                expect((as<any>(memoryService))["get"])->toHaveBeenCalledWith(std::string("/api/memory/") + TEST_AGENT_ID + std::string("/memories"), object{
                    object::pair{std::string("params"), undefined}
                });
                expect(result)->toEqual(mockMemories);
            }
            );
            it(std::string("should handle pagination parameters"), [=]() mutable
            {
                auto params = object{
                    object::pair{std::string("limit"), 10}, 
                    object::pair{std::string("offset"), 20}
                };
                (as<any>(memoryService))["get"]["mockResolvedValue"](object{
                    object::pair{std::string("memories"), array<any>()}
                });
                std::async([=]() { memoryService->getAgentMemories(TEST_AGENT_ID, params); });
                expect((as<any>(memoryService))["get"])->toHaveBeenCalledWith(std::string("/api/memory/") + TEST_AGENT_ID + std::string("/memories"), object{
                    object::pair{std::string("params"), std::string("params")}
                });
            }
            );
        }
        );
        describe(std::string("getRoomMemories"), [=]() mutable
        {
            it(std::string("should retrieve room memories successfully"), [=]() mutable
            {
                auto mockMemories = object{
                    object::pair{std::string("memories"), array<object>{ object{
                        object::pair{std::string("id"), std::string("mem-1")}, 
                        object::pair{std::string("content"), std::string("Room memory")}
                    } }}
                };
                (as<any>(memoryService))["get"]["mockResolvedValue"](mockMemories);
                auto result = std::async([=]() { memoryService->getRoomMemories(TEST_AGENT_ID, TEST_ROOM_ID); });
                expect((as<any>(memoryService))["get"])->toHaveBeenCalledWith(std::string("/api/memory/") + TEST_AGENT_ID + std::string("/rooms/") + TEST_ROOM_ID + std::string("/memories"), object{
                    object::pair{std::string("params"), undefined}
                });
                expect(result)->toEqual(mockMemories);
            }
            );
            it(std::string("should handle memory parameters"), [=]() mutable
            {
                auto params = object{
                    object::pair{std::string("limit"), 5}
                };
                (as<any>(memoryService))["get"]["mockResolvedValue"](object{
                    object::pair{std::string("memories"), array<any>()}
                });
                std::async([=]() { memoryService->getRoomMemories(TEST_AGENT_ID, TEST_ROOM_ID, params); });
                expect((as<any>(memoryService))["get"])->toHaveBeenCalledWith(std::string("/api/memory/") + TEST_AGENT_ID + std::string("/rooms/") + TEST_ROOM_ID + std::string("/memories"), object{
                    object::pair{std::string("params"), std::string("params")}
                });
            }
            );
        }
        );
        describe(std::string("updateMemory"), [=]() mutable
        {
            shared updateParams = object{
                object::pair{std::string("content"), std::string("Updated memory content")}
            };
            it(std::string("should update memory successfully"), [=]() mutable
            {
                auto mockUpdatedMemory = object{
                    object::pair{std::string("id"), TEST_MEMORY_ID}, 
                    object::pair{std::string("content"), std::string("Updated memory content")}
                };
                (as<any>(memoryService))["patch"]["mockResolvedValue"](mockUpdatedMemory);
                auto result = std::async([=]() { memoryService->updateMemory(TEST_AGENT_ID, TEST_MEMORY_ID, updateParams); });
                expect((as<any>(memoryService))["patch"])->toHaveBeenCalledWith(std::string("/api/memory/") + TEST_AGENT_ID + std::string("/memories/") + TEST_MEMORY_ID + string_empty, updateParams);
                expect(result)->toEqual(mockUpdatedMemory);
            }
            );
        }
        );
        describe(std::string("clearAgentMemories"), [=]() mutable
        {
            it(std::string("should clear agent memories successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("deleted"), 10}
                };
                (as<any>(memoryService))["delete"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { memoryService->clearAgentMemories(TEST_AGENT_ID); });
                expect((as<any>(memoryService))["delete"])->toHaveBeenCalledWith(std::string("/api/memory/") + TEST_AGENT_ID + std::string("/memories"));
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("clearRoomMemories"), [=]() mutable
        {
            it(std::string("should clear room memories successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("deleted"), 5}
                };
                (as<any>(memoryService))["delete"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { memoryService->clearRoomMemories(TEST_AGENT_ID, TEST_ROOM_ID); });
                expect((as<any>(memoryService))["delete"])->toHaveBeenCalledWith(std::string("/api/memory/") + TEST_AGENT_ID + std::string("/memories/all/") + TEST_ROOM_ID + string_empty);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("listAgentRooms"), [=]() mutable
        {
            it(std::string("should list agent rooms successfully"), [=]() mutable
            {
                auto mockRooms = object{
                    object::pair{std::string("rooms"), array<object>{ object{
                        object::pair{std::string("id"), std::string("room-1")}, 
                        object::pair{std::string("name"), std::string("Room 1")}
                    }, object{
                        object::pair{std::string("id"), std::string("room-2")}, 
                        object::pair{std::string("name"), std::string("Room 2")}
                    } }}
                };
                (as<any>(memoryService))["get"]["mockResolvedValue"](mockRooms);
                auto result = std::async([=]() { memoryService->listAgentRooms(TEST_AGENT_ID); });
                expect((as<any>(memoryService))["get"])->toHaveBeenCalledWith(std::string("/api/memory/") + TEST_AGENT_ID + std::string("/rooms"));
                expect(result)->toEqual(mockRooms);
            }
            );
        }
        );
        describe(std::string("getRoom"), [=]() mutable
        {
            it(std::string("should get room details successfully"), [=]() mutable
            {
                auto mockRoom = object{
                    object::pair{std::string("id"), TEST_ROOM_ID}, 
                    object::pair{std::string("name"), std::string("Test Room")}, 
                    object::pair{std::string("description"), std::string("A test room")}
                };
                (as<any>(memoryService))["get"]["mockResolvedValue"](mockRoom);
                auto result = std::async([=]() { memoryService->getRoom(TEST_AGENT_ID, TEST_ROOM_ID); });
                expect((as<any>(memoryService))["get"])->toHaveBeenCalledWith(std::string("/api/memory/") + TEST_AGENT_ID + std::string("/rooms/") + TEST_ROOM_ID + string_empty);
                expect(result)->toEqual(mockRoom);
            }
            );
        }
        );
        describe(std::string("createRoom"), [=]() mutable
        {
            shared roomParams = object{
                object::pair{std::string("name"), std::string("New Room")}, 
                object::pair{std::string("description"), std::string("A new room")}
            };
            it(std::string("should create room successfully"), [=]() mutable
            {
                auto mockCreatedRoom = utils::assign(object{
                    object::pair{std::string("id"), std::string("room-new")}
                }, roomParams);
                (as<any>(memoryService))["post"]["mockResolvedValue"](mockCreatedRoom);
                auto result = std::async([=]() { memoryService->createRoom(TEST_AGENT_ID, roomParams); });
                expect((as<any>(memoryService))["post"])->toHaveBeenCalledWith(std::string("/api/memory/") + TEST_AGENT_ID + std::string("/rooms"), roomParams);
                expect(result)->toEqual(mockCreatedRoom);
            }
            );
        }
        );
        describe(std::string("createWorldFromServer"), [=]() mutable
        {
            shared worldParams = object{
                object::pair{std::string("name"), std::string("New World")}, 
                object::pair{std::string("description"), std::string("A new world")}
            };
            it(std::string("should create world from server successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("worldId"), as<std::shared_ptr<UUID>>(std::string("world-new"))}
                };
                (as<any>(memoryService))["post"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { memoryService->createWorldFromServer(TEST_SERVER_ID, worldParams); });
                expect((as<any>(memoryService))["post"])->toHaveBeenCalledWith(std::string("/api/memory/groups/") + TEST_SERVER_ID + string_empty, worldParams);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("deleteWorld"), [=]() mutable
        {
            it(std::string("should delete world successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("success"), true}
                };
                (as<any>(memoryService))["delete"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { memoryService->deleteWorld(TEST_SERVER_ID); });
                expect((as<any>(memoryService))["delete"])->toHaveBeenCalledWith(std::string("/api/memory/groups/") + TEST_SERVER_ID + string_empty);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("clearWorldMemories"), [=]() mutable
        {
            it(std::string("should clear world memories successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("deleted"), 15}
                };
                (as<any>(memoryService))["delete"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { memoryService->clearWorldMemories(TEST_SERVER_ID); });
                expect((as<any>(memoryService))["delete"])->toHaveBeenCalledWith(std::string("/api/memory/groups/") + TEST_SERVER_ID + std::string("/memories"));
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("error handling"), [=]() mutable
        {
            it(std::string("should handle network errors"), [=]() mutable
            {
                (as<any>(memoryService))["get"]["mockRejectedValue"](std::make_shared<Error>(std::string("Network error")));
                std::async([=]() { expect(memoryService->getAgentMemories(TEST_AGENT_ID))->rejects->toThrow(std::string("Network error")); });
            }
            );
            it(std::string("should handle API errors"), [=]() mutable
            {
                (as<any>(memoryService))["post"]["mockRejectedValue"](std::make_shared<Error>(std::string("API error")));
                std::async([=]() { expect(memoryService->createRoom(TEST_AGENT_ID, object{
                    object::pair{std::string("name"), std::string("test")}
                }))->rejects->toThrow(std::string("API error")); });
            }
            );
        }
        );
    }
    );
}

MAIN
