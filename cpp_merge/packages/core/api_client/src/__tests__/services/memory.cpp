#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/api-client/src/__tests__/services/memory.test.h"

any TEST_AGENT_ID = as<std::shared_ptr<UUID>>(std:("550e8400-e29b-41d4-a716-446655440001"));
any TEST_ROOM_ID = as<std::shared_ptr<UUID>>(std:("550e8400-e29b-41d4-a716-446655440002"));
any TEST_MEMORY_ID = as<std::shared_ptr<UUID>>(std:("550e8400-e29b-41d4-a716-446655440003"));
any TEST_SERVER_ID = as<std::shared_ptr<UUID>>(std:("550e8400-e29b-41d4-a716-446655440004"));

void Main(void)
{
    describe(std:("MemoryService"), [=]() mutable
    {
        shared<std::shared_ptr<MemoryService>> memoryService;
        shared mockConfig = object{
            object::pair{std:("baseUrl"), std:("http://localhost:3000")}, 
            object::pair{std:("apiKey"), std:("test-key")}
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
        describe(std:("constructor"), [=]() mutable
        {
            it(std:("should create an instance with valid configuration"), [=]() mutable
            {
                expect(memoryService)->toBeInstanceOf(MemoryService);
            }
            );
            it(std:("should throw error when initialized with invalid configuration"), [=]() mutable
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
        describe(std:("getAgentMemories"), [=]() mutable
        {
            it(std:("should retrieve agent memories successfully"), [=]() mutable
            {
                auto mockMemories = object{
                    object::pair{std:("memories"), array<object>{ object{
                        object::pair{std:("id"), std:("mem-1")}, 
                        object::pair{std:("content"), std:("Memory 1")}, 
                        object::pair{std:("timestamp"), std:("2024-01-01T00:00:00Z")}
                    }, object{
                        object::pair{std:("id"), std:("mem-2")}, 
                        object::pair{std:("content"), std:("Memory 2")}, 
                        object::pair{std:("timestamp"), std:("2024-01-02T00:00:00Z")}
                    } }}
                };
                (as<any>(memoryService))["get"]["mockResolvedValue"](mockMemories);
                auto result = std::async([=]() { memoryService->getAgentMemories(TEST_AGENT_ID); });
                expect((as<any>(memoryService))["get"])->toHaveBeenCalledWith(std:("/api/memory/") + TEST_AGENT_ID + std:("/memories"), object{
                    object::pair{std:("params"), undefined}
                });
                expect(result)->toEqual(mockMemories);
            }
            );
            it(std:("should handle pagination parameters"), [=]() mutable
            {
                auto params = object{
                    object::pair{std:("limit"), 10}, 
                    object::pair{std:("offset"), 20}
                };
                (as<any>(memoryService))["get"]["mockResolvedValue"](object{
                    object::pair{std:("memories"), array<any>()}
                });
                std::async([=]() { memoryService->getAgentMemories(TEST_AGENT_ID, params); });
                expect((as<any>(memoryService))["get"])->toHaveBeenCalledWith(std:("/api/memory/") + TEST_AGENT_ID + std:("/memories"), object{
                    object::pair{std:("params"), std:("params")}
                });
            }
            );
        }
        );
        describe(std:("getRoomMemories"), [=]() mutable
        {
            it(std:("should retrieve room memories successfully"), [=]() mutable
            {
                auto mockMemories = object{
                    object::pair{std:("memories"), array<object>{ object{
                        object::pair{std:("id"), std:("mem-1")}, 
                        object::pair{std:("content"), std:("Room memory")}
                    } }}
                };
                (as<any>(memoryService))["get"]["mockResolvedValue"](mockMemories);
                auto result = std::async([=]() { memoryService->getRoomMemories(TEST_AGENT_ID, TEST_ROOM_ID); });
                expect((as<any>(memoryService))["get"])->toHaveBeenCalledWith(std:("/api/memory/") + TEST_AGENT_ID + std:("/rooms/") + TEST_ROOM_ID + std:("/memories"), object{
                    object::pair{std:("params"), undefined}
                });
                expect(result)->toEqual(mockMemories);
            }
            );
            it(std:("should handle memory parameters"), [=]() mutable
            {
                auto params = object{
                    object::pair{std:("limit"), 5}
                };
                (as<any>(memoryService))["get"]["mockResolvedValue"](object{
                    object::pair{std:("memories"), array<any>()}
                });
                std::async([=]() { memoryService->getRoomMemories(TEST_AGENT_ID, TEST_ROOM_ID, params); });
                expect((as<any>(memoryService))["get"])->toHaveBeenCalledWith(std:("/api/memory/") + TEST_AGENT_ID + std:("/rooms/") + TEST_ROOM_ID + std:("/memories"), object{
                    object::pair{std:("params"), std:("params")}
                });
            }
            );
        }
        );
        describe(std:("updateMemory"), [=]() mutable
        {
            shared updateParams = object{
                object::pair{std:("content"), std:("Updated memory content")}
            };
            it(std:("should update memory successfully"), [=]() mutable
            {
                auto mockUpdatedMemory = object{
                    object::pair{std:("id"), TEST_MEMORY_ID}, 
                    object::pair{std:("content"), std:("Updated memory content")}
                };
                (as<any>(memoryService))["patch"]["mockResolvedValue"](mockUpdatedMemory);
                auto result = std::async([=]() { memoryService->updateMemory(TEST_AGENT_ID, TEST_MEMORY_ID, updateParams); });
                expect((as<any>(memoryService))["patch"])->toHaveBeenCalledWith(std:("/api/memory/") + TEST_AGENT_ID + std:("/memories/") + TEST_MEMORY_ID + string_empty, updateParams);
                expect(result)->toEqual(mockUpdatedMemory);
            }
            );
        }
        );
        describe(std:("clearAgentMemories"), [=]() mutable
        {
            it(std:("should clear agent memories successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("deleted"), 10}
                };
                (as<any>(memoryService))["delete"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { memoryService->clearAgentMemories(TEST_AGENT_ID); });
                expect((as<any>(memoryService))["delete"])->toHaveBeenCalledWith(std:("/api/memory/") + TEST_AGENT_ID + std:("/memories"));
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("clearRoomMemories"), [=]() mutable
        {
            it(std:("should clear room memories successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("deleted"), 5}
                };
                (as<any>(memoryService))["delete"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { memoryService->clearRoomMemories(TEST_AGENT_ID, TEST_ROOM_ID); });
                expect((as<any>(memoryService))["delete"])->toHaveBeenCalledWith(std:("/api/memory/") + TEST_AGENT_ID + std:("/memories/all/") + TEST_ROOM_ID + string_empty);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("listAgentRooms"), [=]() mutable
        {
            it(std:("should list agent rooms successfully"), [=]() mutable
            {
                auto mockRooms = object{
                    object::pair{std:("rooms"), array<object>{ object{
                        object::pair{std:("id"), std:("room-1")}, 
                        object::pair{std:("name"), std:("Room 1")}
                    }, object{
                        object::pair{std:("id"), std:("room-2")}, 
                        object::pair{std:("name"), std:("Room 2")}
                    } }}
                };
                (as<any>(memoryService))["get"]["mockResolvedValue"](mockRooms);
                auto result = std::async([=]() { memoryService->listAgentRooms(TEST_AGENT_ID); });
                expect((as<any>(memoryService))["get"])->toHaveBeenCalledWith(std:("/api/memory/") + TEST_AGENT_ID + std:("/rooms"));
                expect(result)->toEqual(mockRooms);
            }
            );
        }
        );
        describe(std:("getRoom"), [=]() mutable
        {
            it(std:("should get room details successfully"), [=]() mutable
            {
                auto mockRoom = object{
                    object::pair{std:("id"), TEST_ROOM_ID}, 
                    object::pair{std:("name"), std:("Test Room")}, 
                    object::pair{std:("description"), std:("A test room")}
                };
                (as<any>(memoryService))["get"]["mockResolvedValue"](mockRoom);
                auto result = std::async([=]() { memoryService->getRoom(TEST_AGENT_ID, TEST_ROOM_ID); });
                expect((as<any>(memoryService))["get"])->toHaveBeenCalledWith(std:("/api/memory/") + TEST_AGENT_ID + std:("/rooms/") + TEST_ROOM_ID + string_empty);
                expect(result)->toEqual(mockRoom);
            }
            );
        }
        );
        describe(std:("createRoom"), [=]() mutable
        {
            shared roomParams = object{
                object::pair{std:("name"), std:("New Room")}, 
                object::pair{std:("description"), std:("A new room")}
            };
            it(std:("should create room successfully"), [=]() mutable
            {
                auto mockCreatedRoom = utils::assign(object{
                    object::pair{std:("id"), std:("room-new")}
                }, roomParams);
                (as<any>(memoryService))["post"]["mockResolvedValue"](mockCreatedRoom);
                auto result = std::async([=]() { memoryService->createRoom(TEST_AGENT_ID, roomParams); });
                expect((as<any>(memoryService))["post"])->toHaveBeenCalledWith(std:("/api/memory/") + TEST_AGENT_ID + std:("/rooms"), roomParams);
                expect(result)->toEqual(mockCreatedRoom);
            }
            );
        }
        );
        describe(std:("createWorldFromServer"), [=]() mutable
        {
            shared worldParams = object{
                object::pair{std:("name"), std:("New World")}, 
                object::pair{std:("description"), std:("A new world")}
            };
            it(std:("should create world from server successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("worldId"), as<std::shared_ptr<UUID>>(std:("world-new"))}
                };
                (as<any>(memoryService))["post"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { memoryService->createWorldFromServer(TEST_SERVER_ID, worldParams); });
                expect((as<any>(memoryService))["post"])->toHaveBeenCalledWith(std:("/api/memory/groups/") + TEST_SERVER_ID + string_empty, worldParams);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("deleteWorld"), [=]() mutable
        {
            it(std:("should delete world successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("success"), true}
                };
                (as<any>(memoryService))["delete"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { memoryService->deleteWorld(TEST_SERVER_ID); });
                expect((as<any>(memoryService))["delete"])->toHaveBeenCalledWith(std:("/api/memory/groups/") + TEST_SERVER_ID + string_empty);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("clearWorldMemories"), [=]() mutable
        {
            it(std:("should clear world memories successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("deleted"), 15}
                };
                (as<any>(memoryService))["delete"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { memoryService->clearWorldMemories(TEST_SERVER_ID); });
                expect((as<any>(memoryService))["delete"])->toHaveBeenCalledWith(std:("/api/memory/groups/") + TEST_SERVER_ID + std:("/memories"));
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("error handling"), [=]() mutable
        {
            it(std:("should handle network errors"), [=]() mutable
            {
                (as<any>(memoryService))["get"]["mockRejectedValue"](std::make_shared<Error>(std:("Network error")));
                std::async([=]() { expect(memoryService->getAgentMemories(TEST_AGENT_ID))->rejects->toThrow(std:("Network error")); });
            }
            );
            it(std:("should handle API errors"), [=]() mutable
            {
                (as<any>(memoryService))["post"]["mockRejectedValue"](std::make_shared<Error>(std:("API error")));
                std::async([=]() { expect(memoryService->createRoom(TEST_AGENT_ID, object{
                    object::pair{std:("name"), std:("test")}
                }))->rejects->toThrow(std:("API error")); });
            }
            );
        }
        );
    }
    );
}

MAIN
