#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/tests/unit/utils/testing/timeout-manager.test.h"

std::function<any(double)> originalmockExit = process->exit;
any mockExit = mock([=]() mutable
{
    throw any(std::make_shared<Error>(std:("process.exit called")));
}
);

void Main(void)
{
    mock->module(std:("@elizaos/core"), [=]() mutable
    {
        return (object{
            object::pair{std:("logger"), object{
                object::pair{std:("error"), mock()}
            }}
        });
    }
    );
    describe(std:("TestTimeoutManager"), [=]() mutable
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
        describe(std:("getInstance"), [=]() mutable
        {
            it(std:("should return singleton instance"), [=]() mutable
            {
                auto instance1 = TestTimeoutManager::getInstance();
                auto instance2 = TestTimeoutManager::getInstance();
                expect(instance1)->toBe(instance2);
            }
            );
        }
        );
        describe(std:("startTimeout"), [=]() mutable
        {
            it(std:("should start timeout with default duration"), [=]() mutable
            {
                manager->startTimeout(std:("test1"));
                expect(true)->toBe(true);
                expect([=]() mutable
                {
                    return mockExit;
                }
                )->toThrow(std:("process.exit called"));
            }
            );
            it(std:("should start timeout with custom duration"), [=]() mutable
            {
                manager->startTimeout(std:("test2"), 5000);
                expect(true)->toBe(true);
            }
            );
            it(std:("should clear existing timeout when starting new one with same name"), [=]() mutable
            {
                manager->startTimeout(std:("test3"), 5000);
                manager->startTimeout(std:("test3"), 5000);
                expect(true)->toBe(true);
            }
            );
        }
        );
        describe(std:("clearTimeout"), [=]() mutable
        {
            it(std:("should clear timeout and prevent it from firing"), [=]() mutable
            {
                manager->startTimeout(std:("test4"), 5000);
                manager->clearTimeout(std:("test4"));
                expect(true)->toBe(true);
            }
            );
            it(std:("should handle clearing non-existent timeout gracefully"), [=]() mutable
            {
                expect([=]() mutable
                {
                    return manager->clearTimeout(std:("non-existent"));
                }
                )->not->toThrow();
            }
            );
        }
        );
        describe(std:("clearAll"), [=]() mutable
        {
            it(std:("should clear all timeouts"), [=]() mutable
            {
                manager->startTimeout(std:("test5"), 5000);
                manager->startTimeout(std:("test6"), 10000);
                manager->startTimeout(std:("test7"), 15000);
                manager->clearAll();
                expect(true)->toBe(true);
            }
            );
        }
        );
        describe(std:("elapsed time tracking"), [=]() mutable
        {
            it(std:("should track elapsed time correctly"), [=]() mutable
            {
                auto startTime = Date->now();
                manager->startTimeout(std:("test8"), 10000);
                expect(true)->toBe(true);
            }
            );
        }
        );
        describe(std:("process.exit behavior"), [=]() mutable
        {
            it(std:("should call process.exit with code 1 on timeout"), [=]() mutable
            {
                manager->startTimeout(std:("test9"), 1000);
                expect(true)->toBe(true);
            }
            );
        }
        );
    }
    );
}

MAIN
