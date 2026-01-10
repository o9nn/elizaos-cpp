#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/integration/world.test.h"

void Main(void)
{
    describe(std::string("World Integration Tests"), [=]() mutable
    {
        shared<any> adapter;
        shared<std::function<std::shared_ptr<Promise<void>>()>> cleanup;
        shared<std::shared_ptr<UUID>> testAgentId;
        beforeAll([=]() mutable
        {
            auto setup = std::async([=]() { createIsolatedTestDatabase(std::string("world-tests")); });
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
        describe(std::string("World Tests"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                std::async([=]() { adapter->getDatabase()["delete"](worldTable); });
            }
            );
            it(std::string("should create and retrieve a world"), [=]() mutable
            {
                auto worldId = as<std::shared_ptr<UUID>>(uuidv4());
                auto world = object{
                    object::pair{std::string("id"), worldId}, 
                    object::pair{std::string("agentId"), testAgentId}, 
                    object::pair{std::string("name"), std::string("Test World")}, 
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("owner"), std::string("test-user")}
                    }}, 
                    object::pair{std::string("serverId"), std::string("server1")}
                };
                std::async([=]() { adapter->createWorld(world); });
                auto retrieved = std::async([=]() { adapter->getWorld(worldId); });
                expect(retrieved)->not->toBeNull();
                expect(retrieved["id"])->toBe(worldId);
            }
            );
            it(std::string("should not create a world with a duplicate id"), [=]() mutable
            {
                auto worldId = as<std::shared_ptr<UUID>>(uuidv4());
                auto world1 = object{
                    object::pair{std::string("id"), worldId}, 
                    object::pair{std::string("agentId"), testAgentId}, 
                    object::pair{std::string("name"), std::string("Test World 1")}, 
                    object::pair{std::string("serverId"), std::string("server1")}
                };
                auto world2 = object{
                    object::pair{std::string("id"), worldId}, 
                    object::pair{std::string("agentId"), testAgentId}, 
                    object::pair{std::string("name"), std::string("Test World 2")}, 
                    object::pair{std::string("serverId"), std::string("server2")}
                };
                std::async([=]() { adapter->createWorld(world1); });
                std::async([=]() { expect(adapter->createWorld(world2))->rejects->toThrow(); });
            }
            );
            it(std::string("should update an existing world"), [=]() mutable
            {
                auto worldId = as<std::shared_ptr<UUID>>(uuidv4());
                auto originalWorld = object{
                    object::pair{std::string("id"), worldId}, 
                    object::pair{std::string("agentId"), testAgentId}, 
                    object::pair{std::string("name"), std::string("Original World")}, 
                    object::pair{std::string("serverId"), std::string("server1")}
                };
                std::async([=]() { adapter->createWorld(originalWorld); });
                auto updatedWorld = utils::assign(object{
                    , 
                    object::pair{std::string("name"), std::string("Updated World Name")}
                }, originalWorld);
                std::async([=]() { adapter->updateWorld(updatedWorld); });
                auto retrieved = std::async([=]() { adapter->getWorld(worldId); });
                expect(retrieved["name"])->toBe(std::string("Updated World Name"));
            }
            );
            it(std::string("should only update the specified world"), [=]() mutable
            {
                auto world1 = object{
                    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                    object::pair{std::string("agentId"), testAgentId}, 
                    object::pair{std::string("name"), std::string("World One")}, 
                    object::pair{std::string("serverId"), std::string("server1")}
                };
                auto world2 = object{
                    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                    object::pair{std::string("agentId"), testAgentId}, 
                    object::pair{std::string("name"), std::string("World Two")}, 
                    object::pair{std::string("serverId"), std::string("server2")}
                };
                std::async([=]() { adapter->createWorld(world1); });
                std::async([=]() { adapter->createWorld(world2); });
                auto updatedWorld1 = utils::assign(object{
                    , 
                    object::pair{std::string("name"), std::string("Updated World One")}
                }, world1);
                std::async([=]() { adapter->updateWorld(updatedWorld1); });
                auto retrieved1 = std::async([=]() { adapter->getWorld(world1->id); });
                auto retrieved2 = std::async([=]() { adapter->getWorld(world2->id); });
                expect(retrieved1["name"])->toBe(std::string("Updated World One"));
                expect(retrieved2["name"])->toBe(std::string("World Two"));
            }
            );
            it(std::string("should delete a world"), [=]() mutable
            {
                auto worldId = as<std::shared_ptr<UUID>>(uuidv4());
                auto world = object{
                    object::pair{std::string("id"), worldId}, 
                    object::pair{std::string("agentId"), testAgentId}, 
                    object::pair{std::string("name"), std::string("To Be Deleted")}, 
                    object::pair{std::string("serverId"), std::string("server1")}
                };
                std::async([=]() { adapter->createWorld(world); });
                auto retrieved = std::async([=]() { adapter->getWorld(worldId); });
                expect(retrieved)->not->toBeNull();
                std::async([=]() { adapter->removeWorld(worldId); });
                retrieved = std::async([=]() { adapter->getWorld(worldId); });
                expect(retrieved)->toBeNull();
            }
            );
            it(std::string("should return null when retrieving a non-existent world"), [=]() mutable
            {
                auto world = std::async([=]() { adapter->getWorld(as<std::shared_ptr<UUID>>(uuidv4())); });
                expect(world)->toBeNull();
            }
            );
            it(std::string("should retrieve all worlds for an agent"), [=]() mutable
            {
                auto world1 = object{
                    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                    object::pair{std::string("agentId"), testAgentId}, 
                    object::pair{std::string("name"), std::string("World 0")}, 
                    object::pair{std::string("serverId"), std::string("server0")}
                };
                auto world2 = object{
                    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                    object::pair{std::string("agentId"), testAgentId}, 
                    object::pair{std::string("name"), std::string("World 1")}, 
                    object::pair{std::string("serverId"), std::string("server1")}
                };
                std::async([=]() { adapter->createWorld(world1); });
                std::async([=]() { adapter->createWorld(world2); });
                auto worlds = std::async([=]() { adapter->getAllWorlds(); });
                expect(worlds->get_length())->toBe(2);
            }
            );
            it(std::string("should return an empty array if no worlds exist"), [=]() mutable
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
