#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/integration/world.test.h"

void Main(void)
{
    describe(std:("World Integration Tests"), [=]() mutable
    {
        shared<any> adapter;
        shared<std::function<std::shared_ptr<Promise<void>>()>> cleanup;
        shared<std::shared_ptr<UUID>> testAgentId;
        beforeAll([=]() mutable
        {
            auto setup = std::async([=]() { createIsolatedTestDatabase(std:("world-tests")); });
            adapter = setup["adapter"];
            cleanup = setup["cleanup"];
            testAgentId = setup["testAgentId"];
        }
        );
        afterAll([=]() mutable
        {
            if (cleanup) {
                std::async([=]() { cleanup(); });
            }
        }
        );
        describe(std:("World Tests"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                std::async([=]() { adapter->getDatabase()["delete"](worldTable); });
            }
            );
            it(std:("should create and retrieve a world"), [=]() mutable
            {
                auto worldId = as<std::shared_ptr<UUID>>(uuidv4());
                auto world = object{
                    object::pair{std:("id"), worldId}, 
                    object::pair{std:("agentId"), testAgentId}, 
                    object::pair{std:("name"), std:("Test World")}, 
                    object::pair{std:("metadata"), object{
                        object::pair{std:("owner"), std:("test-user")}
                    }}, 
                    object::pair{std:("serverId"), std:("server1")}
                };
                std::async([=]() { adapter->createWorld(world); });
                auto retrieved = std::async([=]() { adapter->getWorld(worldId); });
                expect(retrieved)->not->toBeNull();
                expect(retrieved["id"])->toBe(worldId);
            }
            );
            it(std:("should not create a world with a duplicate id"), [=]() mutable
            {
                auto worldId = as<std::shared_ptr<UUID>>(uuidv4());
                auto world1 = object{
                    object::pair{std:("id"), worldId}, 
                    object::pair{std:("agentId"), testAgentId}, 
                    object::pair{std:("name"), std:("Test World 1")}, 
                    object::pair{std:("serverId"), std:("server1")}
                };
                auto world2 = object{
                    object::pair{std:("id"), worldId}, 
                    object::pair{std:("agentId"), testAgentId}, 
                    object::pair{std:("name"), std:("Test World 2")}, 
                    object::pair{std:("serverId"), std:("server2")}
                };
                std::async([=]() { adapter->createWorld(world1); });
                std::async([=]() { expect(adapter->createWorld(world2))->rejects->toThrow(); });
            }
            );
            it(std:("should update an existing world"), [=]() mutable
            {
                auto worldId = as<std::shared_ptr<UUID>>(uuidv4());
                auto originalWorld = object{
                    object::pair{std:("id"), worldId}, 
                    object::pair{std:("agentId"), testAgentId}, 
                    object::pair{std:("name"), std:("Original World")}, 
                    object::pair{std:("serverId"), std:("server1")}
                };
                std::async([=]() { adapter->createWorld(originalWorld); });
                auto updatedWorld = utils::assign(object{
                    , 
                    object::pair{std:("name"), std:("Updated World Name")}
                }, originalWorld);
                std::async([=]() { adapter->updateWorld(updatedWorld); });
                auto retrieved = std::async([=]() { adapter->getWorld(worldId); });
                expect(retrieved["name"])->toBe(std:("Updated World Name"));
            }
            );
            it(std:("should only update the specified world"), [=]() mutable
            {
                auto world1 = object{
                    object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                    object::pair{std:("agentId"), testAgentId}, 
                    object::pair{std:("name"), std:("World One")}, 
                    object::pair{std:("serverId"), std:("server1")}
                };
                auto world2 = object{
                    object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                    object::pair{std:("agentId"), testAgentId}, 
                    object::pair{std:("name"), std:("World Two")}, 
                    object::pair{std:("serverId"), std:("server2")}
                };
                std::async([=]() { adapter->createWorld(world1); });
                std::async([=]() { adapter->createWorld(world2); });
                auto updatedWorld1 = utils::assign(object{
                    , 
                    object::pair{std:("name"), std:("Updated World One")}
                }, world1);
                std::async([=]() { adapter->updateWorld(updatedWorld1); });
                auto retrieved1 = std::async([=]() { adapter->getWorld(world1->id); });
                auto retrieved2 = std::async([=]() { adapter->getWorld(world2->id); });
                expect(retrieved1["name"])->toBe(std:("Updated World One"));
                expect(retrieved2["name"])->toBe(std:("World Two"));
            }
            );
            it(std:("should delete a world"), [=]() mutable
            {
                auto worldId = as<std::shared_ptr<UUID>>(uuidv4());
                auto world = object{
                    object::pair{std:("id"), worldId}, 
                    object::pair{std:("agentId"), testAgentId}, 
                    object::pair{std:("name"), std:("To Be Deleted")}, 
                    object::pair{std:("serverId"), std:("server1")}
                };
                std::async([=]() { adapter->createWorld(world); });
                auto retrieved = std::async([=]() { adapter->getWorld(worldId); });
                expect(retrieved)->not->toBeNull();
                std::async([=]() { adapter->removeWorld(worldId); });
                retrieved = std::async([=]() { adapter->getWorld(worldId); });
                expect(retrieved)->toBeNull();
            }
            );
            it(std:("should return null when retrieving a non-existent world"), [=]() mutable
            {
                auto world = std::async([=]() { adapter->getWorld(as<std::shared_ptr<UUID>>(uuidv4())); });
                expect(world)->toBeNull();
            }
            );
            it(std:("should retrieve all worlds for an agent"), [=]() mutable
            {
                auto world1 = object{
                    object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                    object::pair{std:("agentId"), testAgentId}, 
                    object::pair{std:("name"), std:("World 0")}, 
                    object::pair{std:("serverId"), std:("server0")}
                };
                auto world2 = object{
                    object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                    object::pair{std:("agentId"), testAgentId}, 
                    object::pair{std:("name"), std:("World 1")}, 
                    object::pair{std:("serverId"), std:("server1")}
                };
                std::async([=]() { adapter->createWorld(world1); });
                std::async([=]() { adapter->createWorld(world2); });
                auto worlds = std::async([=]() { adapter->getAllWorlds(); });
                expect(worlds->get_length())->toBe(2);
            }
            );
            it(std:("should return an empty array if no worlds exist"), [=]() mutable
            {
                auto worlds = std::async([=]() { adapter->getAllWorlds(); });
                expect(worlds)->toEqual(array<any>());
            }
            );
        }
        );
    }
    );
}

MAIN
