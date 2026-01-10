#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/unit/pglite/manager.test.h"

void Main(void)
{
    describe(std::string("PGliteClientManager"), [=]() mutable
    {
        describe(std::string("constructor"), [=]() mutable
        {
            it(std::string("should create a PGLite client with the provided options"), [=]() mutable
            {
                auto manager = std::make_shared<PGliteClientManager>(object{
                    object::pair{std::string("dataDir"), std::string("memory://")}
                });
                expect(manager)->toBeDefined();
                expect(manager->getConnection())->toBeDefined();
            }
            );
            it(std::string("should initialize shuttingDown to false"), [=]() mutable
            {
                auto manager = std::make_shared<PGliteClientManager>(object{
                    object::pair{std::string("dataDir"), std::string("memory://")}
                });
                expect(manager->isShuttingDown())->toBe(false);
            }
            );
        }
        );
        describe(std::string("getConnection"), [=]() mutable
        {
            it(std::string("should return the PGLite client"), [=]() mutable
            {
                auto manager = std::make_shared<PGliteClientManager>(object{
                    object::pair{std::string("dataDir"), std::string("memory://")}
                });
                auto client = manager->getConnection();
                expect(client)->toBeDefined();
                expect(client->query)->toBeDefined();
                expect(client->close)->toBeDefined();
            }
            );
        }
        );
        describe(std::string("isShuttingDown"), [=]() mutable
        {
            it(std::string("should return false initially"), [=]() mutable
            {
                auto manager = std::make_shared<PGliteClientManager>(object{
                    object::pair{std::string("dataDir"), std::string("memory://")}
                });
                expect(manager->isShuttingDown())->toBe(false);
            }
            );
            it(std::string("should return true after close is called"), [=]() mutable
            {
                auto manager = std::make_shared<PGliteClientManager>(object{
                    object::pair{std::string("dataDir"), std::string("memory://")}
                });
                std::async([=]() { manager->close(); });
                expect(manager->isShuttingDown())->toBe(true);
            }
            );
        }
        );
        describe(std::string("close"), [=]() mutable
        {
            it(std::string("should set shuttingDown to true immediately"), [=]() mutable
            {
                auto manager = std::make_shared<PGliteClientManager>(object{
                    object::pair{std::string("dataDir"), std::string("memory://")}
                });
                expect(manager->isShuttingDown())->toBe(false);
                std::async([=]() { manager->close(); });
                expect(manager->isShuttingDown())->toBe(true);
            }
            );
            it(std::string("should return a promise"), [=]() mutable
            {
                auto manager = std::make_shared<PGliteClientManager>(object{
                    object::pair{std::string("dataDir"), std::string("memory://")}
                });
                auto result = manager->close();
                expect(result)->toBeInstanceOf(Promise);
            }
            );
            it(std::string("should handle multiple close calls"), [=]() mutable
            {
                auto manager = std::make_shared<PGliteClientManager>(object{
                    object::pair{std::string("dataDir"), std::string("memory://")}
                });
                std::async([=]() { manager->close(); });
                std::async([=]() { manager->close(); });
                std::async([=]() { manager->close(); });
                expect(manager->isShuttingDown())->toBe(true);
            }
            );
        }
        );
        describe(std::string("edge cases"), [=]() mutable
        {
            it(std::string("should handle empty data directory"), [=]() mutable
            {
                auto manager = std::make_shared<PGliteClientManager>(object{
                    object::pair{std::string("dataDir"), string_empty}
                });
                expect(manager)->toBeDefined();
                expect(manager->getConnection())->toBeDefined();
            }
            );
            it(std::string("should maintain state consistency during concurrent close calls"), [=]() mutable
            {
                auto manager = std::make_shared<PGliteClientManager>(object{
                    object::pair{std::string("dataDir"), std::string("memory://")}
                });
                auto close1 = manager->close();
                auto close2 = manager->close();
                auto close3 = manager->close();
                expect(close1)->toBeInstanceOf(Promise);
                expect(close2)->toBeInstanceOf(Promise);
                expect(close3)->toBeInstanceOf(Promise);
                std::async([=]() { Promise->all(std::tuple<std::shared_ptr<Promise<void>>, std::shared_ptr<Promise<void>>, std::shared_ptr<Promise<void>>>{ close1, close2, close3 }); });
                expect(manager->isShuttingDown())->toBe(true);
            }
            );
        }
        );
    }
    );
}

MAIN
