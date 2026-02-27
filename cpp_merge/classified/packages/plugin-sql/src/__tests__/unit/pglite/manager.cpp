#include "manager.test.h"

void Main(void)
{
    describe(std:("PGliteClientManager"), [=]() mutable
    {
        describe(std:("constructor"), [=]() mutable
        {
            it(std:("should create a PGLite client with the provided options"), [=]() mutable
            {
                auto manager = std::make_shared<PGliteClientManager>(object{
                    object::pair{std:("dataDir"), std:("memory://")}
                });
                expect(manager)->toBeDefined();
                expect(manager->getConnection())->toBeDefined();
            }
            );
            it(std:("should initialize shuttingDown to false"), [=]() mutable
            {
                auto manager = std::make_shared<PGliteClientManager>(object{
                    object::pair{std:("dataDir"), std:("memory://")}
                });
                expect(manager->isShuttingDown())->toBe(false);
            }
            );
        }
        );
        describe(std:("getConnection"), [=]() mutable
        {
            it(std:("should return the PGLite client"), [=]() mutable
            {
                auto manager = std::make_shared<PGliteClientManager>(object{
                    object::pair{std:("dataDir"), std:("memory://")}
                });
                auto client = manager->getConnection();
                expect(client)->toBeDefined();
                expect(client->query)->toBeDefined();
                expect(client->close)->toBeDefined();
            }
            );
        }
        );
        describe(std:("isShuttingDown"), [=]() mutable
        {
            it(std:("should return false initially"), [=]() mutable
            {
                auto manager = std::make_shared<PGliteClientManager>(object{
                    object::pair{std:("dataDir"), std:("memory://")}
                });
                expect(manager->isShuttingDown())->toBe(false);
            }
            );
            it(std:("should return true after close is called"), [=]() mutable
            {
                auto manager = std::make_shared<PGliteClientManager>(object{
                    object::pair{std:("dataDir"), std:("memory://")}
                });
                std::async([=]() { manager->close(); });
                expect(manager->isShuttingDown())->toBe(true);
            }
            );
        }
        );
        describe(std:("close"), [=]() mutable
        {
            it(std:("should set shuttingDown to true immediately"), [=]() mutable
            {
                auto manager = std::make_shared<PGliteClientManager>(object{
                    object::pair{std:("dataDir"), std:("memory://")}
                });
                expect(manager->isShuttingDown())->toBe(false);
                std::async([=]() { manager->close(); });
                expect(manager->isShuttingDown())->toBe(true);
            }
            );
            it(std:("should return a promise"), [=]() mutable
            {
                auto manager = std::make_shared<PGliteClientManager>(object{
                    object::pair{std:("dataDir"), std:("memory://")}
                });
                auto result = manager->close();
                expect(result)->toBeInstanceOf(Promise);
            }
            );
            it(std:("should handle multiple close calls"), [=]() mutable
            {
                auto manager = std::make_shared<PGliteClientManager>(object{
                    object::pair{std:("dataDir"), std:("memory://")}
                });
                std::async([=]() { manager->close(); });
                std::async([=]() { manager->close(); });
                std::async([=]() { manager->close(); });
                expect(manager->isShuttingDown())->toBe(true);
            }
            );
        }
        );
        describe(std:("edge cases"), [=]() mutable
        {
            it(std:("should handle empty data directory"), [=]() mutable
            {
                auto manager = std::make_shared<PGliteClientManager>(object{
                    object::pair{std:("dataDir"), string_empty}
                });
                expect(manager)->toBeDefined();
                expect(manager->getConnection())->toBeDefined();
            }
            );
            it(std:("should maintain state consistency during concurrent close calls"), [=]() mutable
            {
                auto manager = std::make_shared<PGliteClientManager>(object{
                    object::pair{std:("dataDir"), std:("memory://")}
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
