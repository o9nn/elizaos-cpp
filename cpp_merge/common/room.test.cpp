#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/integration/room.test.h"

void Main(void)
{
    describe(std:("Room Integration Tests"), [=]() mutable
    {
        shared<any> adapter;
        shared<std::shared_ptr<AgentRuntime>> runtime;
        shared<std::function<std::shared_ptr<Promise<void>>()>> cleanup;
        shared<std::shared_ptr<UUID>> testAgentId;
        shared<std::shared_ptr<UUID>> testWorldId;
        beforeAll([=]() mutable
        {
            auto setup = std::async([=]() { createIsolatedTestDatabase(std:("room-tests")); });
            adapter = setup["adapter"];
            runtime = setup["runtime"];
            cleanup = setup["cleanup"];
            testAgentId = setup["testAgentId"];
            testWorldId = as<std::shared_ptr<UUID>>(uuidv4());
            std::async([=]() { adapter->createWorld(object{
                object::pair{std:("id"), testWorldId}, 
                object::pair{std:("agentId"), testAgentId}, 
                object::pair{std:("name"), std:("Test World")}, 
                object::pair{std:("serverId"), std:("test-server")}
            }); });
        }
        );
        afterAll([=]() mutable
        {
            if (cleanup) {
                std::async([=]() { cleanup(); });
            }
        }
        );
        describe(std:("Room Tests"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                std::async([=]() { adapter->getDatabase()["delete"](roomTable); });
            }
            );
            it(std:("should create and retrieve a room"), [=]() mutable
            {
                auto roomId = as<std::shared_ptr<UUID>>(uuidv4());
                auto room = object{
                    object::pair{std:("id"), roomId}, 
                    object::pair{std:("agentId"), testAgentId}, 
                    object::pair{std:("worldId"), testWorldId}, 
                    object::pair{std:("source"), std:("test")}, 
                    object::pair{std:("type"), ChannelType->GROUP}, 
                    object::pair{std:("name"), std:("Test Room")}
                };
                std::async([=]() { adapter->createRooms(array<any>{ room }); });
                auto retrieved = std::async([=]() { adapter->getRoomsByIds(array<any>{ roomId }); });
                expect(retrieved)->not->toBeNull();
                expect(const_(retrieved)[0]->id)->toBe(roomId);
            }
            );
            it(std:("should get all rooms for a world"), [=]() mutable
            {
                auto room1 = object{
                    object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                    object::pair{std:("agentId"), testAgentId}, 
                    object::pair{std:("worldId"), testWorldId}, 
                    object::pair{std:("source"), std:("test")}, 
                    object::pair{std:("type"), ChannelType->GROUP}, 
                    object::pair{std:("name"), std:("Room 1")}
                };
                auto room2 = object{
                    object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                    object::pair{std:("agentId"), testAgentId}, 
                    object::pair{std:("worldId"), testWorldId}, 
                    object::pair{std:("source"), std:("test")}, 
                    object::pair{std:("type"), ChannelType->GROUP}, 
                    object::pair{std:("name"), std:("Room 2")}
                };
                std::async([=]() { adapter->createRooms(array<any>{ room1, room2 }); });
                auto rooms = std::async([=]() { adapter->getRoomsByWorld(testWorldId); });
                expect(rooms)->toHaveLength(2);
            }
            );
            it(std:("should update a room"), [=]() mutable
            {
                auto roomId = as<std::shared_ptr<UUID>>(uuidv4());
                auto room = object{
                    object::pair{std:("id"), roomId}, 
                    object::pair{std:("agentId"), testAgentId}, 
                    object::pair{std:("worldId"), testWorldId}, 
                    object::pair{std:("source"), std:("test")}, 
                    object::pair{std:("type"), ChannelType->GROUP}, 
                    object::pair{std:("name"), std:("Original Room Name")}
                };
                std::async([=]() { adapter->createRooms(array<any>{ as<std::shared_ptr<Room>>(room) }); });
                auto updatedRoom = utils::assign(object{
                    , 
                    object::pair{std:("name"), std:("Updated Room Name")}
                }, room);
                std::async([=]() { adapter->updateRoom(updatedRoom); });
                auto retrieved = std::async([=]() { adapter->getRoomsByIds(array<any>{ roomId }); });
                expect(retrieved)->not->toBeNull();
                expect(const_(retrieved)[0]->name)->toBe(std:("Updated Room Name"));
            }
            );
            it(std:("should delete a room"), [=]() mutable
            {
                auto roomId = as<std::shared_ptr<UUID>>(uuidv4());
                auto room = object{
                    object::pair{std:("id"), roomId}, 
                    object::pair{std:("agentId"), testAgentId}, 
                    object::pair{std:("worldId"), testWorldId}, 
                    object::pair{std:("source"), std:("test")}, 
                    object::pair{std:("type"), ChannelType->GROUP}, 
                    object::pair{std:("name"), std:("To Be Deleted")}
                };
                std::async([=]() { adapter->createRooms(array<any>{ as<std::shared_ptr<Room>>(room) }); });
                auto retrieved = std::async([=]() { adapter->getRoomsByIds(array<any>{ room["id"] }); });
                expect(retrieved)->toHaveLength(1);
                std::async([=]() { adapter->deleteRoom(room["id"]); });
                retrieved = std::async([=]() { adapter->getRoomsByIds(array<any>{ room["id"] }); });
                expect(retrieved)->toEqual(array<any>());
            }
            );
        }
        );
    }
    );
}

MAIN
