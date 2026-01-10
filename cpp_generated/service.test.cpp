#include "/home/runner/work/elizaos-cpp/elizaos-cpp/the-org/src/loadTest/__tests__/service.test.h"

void Main(void)
{
    describe(std::string("LoadTestService"), [=]() mutable
    {
        shared<std::shared_ptr<LoadTestService>> service;
        shared<std::shared_ptr<AgentRuntime>> mockRuntime;
        shared<double> testStartTime;
        beforeEach([=]() mutable
        {
            testStartTime = Date->now();
            logger->info(std::string("====================================================="));
            logger->info(std::string("Test starting at: ") + ((std::make_shared<Date>(testStartTime)))->toISOString() + string_empty);
            logger->info(std::string("Setting up test environment for LoadTestService..."));
            mockRuntime = as<std::shared_ptr<AgentRuntime>>(as<any>(object{
                object::pair{std::string("agentId"), std::string("test-agent-id")}, 
                object::pair{std::string("getSetting"), [=]() mutable
                {
                    return nullptr;
                }
                }, 
                object::pair{std::string("emit"), [=](auto event, auto data) mutable
                {
                    logger->info(std::string("[MOCK] Event emitted: ") + event + string_empty, data);
                    return Promise->resolve();
                }
                }
            }));
            service = std::make_shared<LoadTestService>(mockRuntime);
            logger->info(std::string("Test environment setup complete in ") + (Date->now() - testStartTime) + std::string("ms"));
            logger->info(std::string("-----------------------------------------------------"));
        }
        );
        afterEach([=]() mutable
        {
            auto testDuration = Date->now() - testStartTime;
            logger->info(std::string("-----------------------------------------------------"));
            logger->info(std::string("Test completed in ") + testDuration + std::string("ms"));
            logger->info(std::string("====================================================="));
        }
        );
        it(std::string("should start a test and record metrics"), [=]() mutable
        {
            auto stepStartTime = Date->now();
            logger->info(std::string("Starting load test metrics test case..."));
            auto testId = std::string("test-1");
            logger->info(std::string("Creating test with ID: ") + testId + string_empty);
            service->startTest(testId);
            logger->info(std::string("Test created in ") + (Date->now() - stepStartTime) + std::string("ms"));
            logger->info(std::string("Recording messages and errors for test..."));
            auto messageStartTime = Date->now();
            service->recordMessage(testId);
            logger->info(std::string("Message 1 recorded at ") + Date->now() + std::string("ms"));
            service->recordMessage(testId);
            logger->info(std::string("Message 2 recorded at ") + Date->now() + std::string("ms"));
            service->recordError(testId);
            logger->info(std::string("Error recorded at ") + Date->now() + std::string("ms"));
            logger->info(std::string("All test messages recorded in ") + (Date->now() - messageStartTime) + std::string("ms"));
            logger->info(std::string("Retrieving and validating test metrics..."));
            auto metrics = service->getTestMetrics(testId);
            logger->info(std::string("Current test metrics:"), object{
                object::pair{std::string("testId"), std::string("testId")}, 
                object::pair{std::string("startTime"), metrics["startTime"]}, 
                object::pair{std::string("messageCount"), metrics["messageCount"]}, 
                object::pair{std::string("errors"), metrics["errors"]}, 
                object::pair{std::string("testDuration"), Date->now() - metrics["startTime"]}
            });
            expect(metrics)->toBeDefined();
            expect(metrics["messageCount"])->toBe(2);
            expect(metrics["errors"])->toBe(1);
            logger->info(std::string("Metrics validation successful"));
            logger->info(std::string("Testing service stop functionality..."));
            auto stopStartTime = Date->now();
            std::async([=]() { service->stop(); });
            logger->info(std::string("Service stopped in ") + (Date->now() - stopStartTime) + std::string("ms"));
            auto afterStopMetrics = service->getTestMetrics(testId);
            logger->info(std::string("After stop metrics:"), afterStopMetrics);
            expect(afterStopMetrics)->toBeUndefined();
            logger->info(std::string("Test complete - service stop cleared all test data as expected"));
        }
        );
    }
    );
}

MAIN
