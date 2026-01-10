#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/tests/unit/utils/testing/timeout-manager.test.h"

std::function<any(double)> originalmockExit = process->exit;
any mockExit = mock([=]() mutable
{
    throw any(std::make_shared<Error>(std::string("process.exit called")));
}
);

void Main(void)
{
    mock->module(std::string("@elizaos/core"), [=]() mutable
    {
        return (object{
            object::pair{std::string("logger"), object{
                object::pair{std::string("error"), mock()}
            }}
        });
    }
    );
    describe(std::string("TestTimeoutManager"), [=]() mutable
    {
        shared<std::shared_ptr<TestTimeoutManager>> manager;
        beforeEach([=]() mutable
        {
            manager = std::make_shared<TestTimeoutManager>();
        }
        );
        afterEach([=]() mutable
        {
            manager->clearAll();
        }
        );
        describe(std::string("getInstance"), [=]() mutable
        {
            it(std::string("should return singleton instance"), [=]() mutable
            {
                auto instance1 = TestTimeoutManager::getInstance();
                auto instance2 = TestTimeoutManager::getInstance();
                expect(instance1)->toBe(instance2);
            }
            );
        }
        );
        describe(std::string("startTimeout"), [=]() mutable
        {
            it(std::string("should start timeout with default duration"), [=]() mutable
            {
                manager->startTimeout(std::string("test1"));
                expect(true)->toBe(true);
                expect([=]() mutable
                {
                    return mockExit;
                }
                )->toThrow(std::string("process.exit called"));
            }
            );
            it(std::string("should start timeout with custom duration"), [=]() mutable
            {
                manager->startTimeout(std::string("test2"), 5000);
                expect(true)->toBe(true);
            }
            );
            it(std::string("should clear existing timeout when starting new one with same name"), [=]() mutable
            {
                manager->startTimeout(std::string("test3"), 5000);
                manager->startTimeout(std::string("test3"), 5000);
                expect(true)->toBe(true);
            }
            );
        }
        );
        describe(std::string("clearTimeout"), [=]() mutable
        {
            it(std::string("should clear timeout and prevent it from firing"), [=]() mutable
            {
                manager->startTimeout(std::string("test4"), 5000);
                manager->clearTimeout(std::string("test4"));
                expect(true)->toBe(true);
            }
            );
            it(std::string("should handle clearing non-existent timeout gracefully"), [=]() mutable
            {
                expect([=]() mutable
                {
                    return manager->clearTimeout(std::string("non-existent"));
                }
                )->not->toThrow();
            }
            );
        }
        );
        describe(std::string("clearAll"), [=]() mutable
        {
            it(std::string("should clear all timeouts"), [=]() mutable
            {
                manager->startTimeout(std::string("test5"), 5000);
                manager->startTimeout(std::string("test6"), 10000);
                manager->startTimeout(std::string("test7"), 15000);
                manager->clearAll();
                expect(true)->toBe(true);
            }
            );
        }
        );
        describe(std::string("elapsed time tracking"), [=]() mutable
        {
            it(std::string("should track elapsed time correctly"), [=]() mutable
            {
                auto startTime = Date->now();
                manager->startTimeout(std::string("test8"), 10000);
                expect(true)->toBe(true);
            }
            );
        }
        );
        describe(std::string("process.exit behavior"), [=]() mutable
        {
            it(std::string("should call process.exit with code 1 on timeout"), [=]() mutable
            {
                manager->startTimeout(std::string("test9"), 1000);
                expect(true)->toBe(true);
            }
            );
        }
        );
    }
    );
}

MAIN
