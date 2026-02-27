#include "/home/runner/work/elizaos-cpp/elizaos-cpp/the-org/src/loadTest/__tests__/service.test.h"

void Main(void)
{
    describe(std:("LoadTestService"), [=]() mutable
    {
        shared<std::shared_ptr<LoadTestService>> service;
        shared<std::shared_ptr<AgentRuntime>> mockRuntime;
        shared<double> testStartTime;
        beforeEach([=]() mutable
        {
            testStartTime = Date->now();
            logger->info(std:("================"));
            logger->info(std:("Test starting at: ") + ((std::make_shared<Date>(testStartTime)))->toISOString() + string_empty);
            logger->info(std:("Setting up test environment for LoadTestService..."));
            mockRuntime = as<std::shared_ptr<AgentRuntime>>(as<any>(object{
                object::pair{std:("agentId"), std:("test-agent-id")}, 
                object::pair{std:("getSetting"), [=]() mutable
                {
                    return nullptr;
                }
                }, 
                object::pair{std:("emit"), [=](auto event, auto data) mutable
                {
                    logger->info(std:("[MOCK] Event emitted: ") + event + string_empty, data);
                    return Promise->resolve();
                }
                }
            }));
            service = std::make_shared<LoadTestService>(mockRuntime);
            logger->info(std:("Test environment setup complete in ") + (Date->now() - testStartTime) + std:("ms"));
            logger->info(std:("-----------------------------------------------------"));
        }
        );
        afterEach([=]() mutable
        {
            auto testDuration = Date->now() - testStartTime;
            logger->info(std:("-----------------------------------------------------"));
            logger->info(std:("Test completed in ") + testDuration + std:("ms"));
            logger->info(std:("================"));
        }
        );
        it(std:("should start a test and record metrics"), [=]() mutable
        {
            auto stepStartTime = Date->now();
            logger->info(std:("Starting load test metrics test case..."));
            auto testId = std:("test-1");
            logger->info(std:("Creating test with ID: ") + testId + string_empty);
            service->startTest(testId);
            logger->info(std:("Test created in ") + (Date->now() - stepStartTime) + std:("ms"));
            logger->info(std:("Recording messages and errors for test..."));
            auto messageStartTime = Date->now();
            service->recordMessage(testId);
            logger->info(std:("Message 1 recorded at ") + Date->now() + std:("ms"));
            service->recordMessage(testId);
            logger->info(std:("Message 2 recorded at ") + Date->now() + std:("ms"));
            service->recordError(testId);
            logger->info(std:("Error recorded at ") + Date->now() + std:("ms"));
            logger->info(std:("All test messages recorded in ") + (Date->now() - messageStartTime) + std:("ms"));
            logger->info(std:("Retrieving and validating test metrics..."));
            auto metrics = service->getTestMetrics(testId);
            logger->info(std:("Current test metrics:"), object{
                object::pair{std:("testId"), std:("testId")}, 
                object::pair{std:("startTime"), metrics["startTime"]}, 
                object::pair{std:("messageCount"), metrics["messageCount"]}, 
                object::pair{std:("errors"), metrics["errors"]}, 
                object::pair{std:("testDuration"), Date->now() - metrics["startTime"]}
            });
            expect(metrics)->toBeDefined();
            expect(metrics["messageCount"])->toBe(2);
            expect(metrics["errors"])->toBe(1);
            logger->info(std:("Metrics validation successful"));
            logger->info(std:("Testing service stop functionality..."));
            auto stopStartTime = Date->now();
            std::async([=]() { service->stop(); });
            logger->info(std:("Service stopped in ") + (Date->now() - stopStartTime) + std:("ms"));
            auto afterStopMetrics = service->getTestMetrics(testId);
            logger->info(std:("After stop metrics:"), afterStopMetrics);
            expect(afterStopMetrics)->toBeUndefined();
            logger->info(std:("Test complete - service stop cleared all test data as expected"));
        }
        );
    }
    );
}

MAIN
