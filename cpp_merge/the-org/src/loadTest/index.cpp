#include "index.hpp"

std::shared_ptr<Promise<std::shared_ptr<LoadTestMetrics>>> AgentLoadTestSuite::runSequentialLoadTest(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<ScaleConfig> config, ThresholdLevel thresholdLevel)
{
    auto startTime = Date->now();
    auto metrics = object{
        object::pair{std:("totalTime"), 0}, 
        object::pair{std:("errorCount"), 0}, 
        object::pair{std:("messagesSent"), 0}, 
        object::pair{std:("messagesProcessed"), 0}, 
        object::pair{std:("avgResponseTime"), 0}, 
        object::pair{std:("minResponseTime"), Number->MAX_SAFE_INTEGER}, 
        object::pair{std:("maxResponseTime"), 0}, 
        object::pair{std:("responseTimes"), array<any>()}, 
        object::pair{std:("peakMemoryUsage"), 0}, 
        object::pair{std:("memoryUsageStart"), process->memoryUsage()->heapUsed / 1024 / 1024}, 
        object::pair{std:("memoryUsageEnd"), 0}, 
        object::pair{std:("successRate"), 0}, 
        object::pair{std:("throughput"), 0}, 
        object::pair{std:("timeoutCount"), 0}, 
        object::pair{std:("errorTypes"), object{}}
    };
    try
    {
        logger->info(std:("Creating test world for ") + config->description + string_empty);
        auto worldId = std::async([=]() { this->scenarioService["createWorld"](std:("Load Test - ") + config->agents + std:(" agents"), std:("Test Admin")); });
        auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std:("load-test-") + config->agents + string_empty); });
        std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, runtime->agentId); });
        auto participants = array<std::shared_ptr<UUID>>();
        logger->info(std:("Creating ") + config->agents + std:(" participants..."));
        for (auto i = 0; i < config->agents; i++)
        {
            auto participantId = as<std::shared_ptr<UUID>>(uuidv4());
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, participantId); });
            participants->push(participantId);
            if (AND((config->agents > 20), (i % 10 == 0))) {
                logger->info(std:("Created ") + (i + 1) + std:("/") + config->agents + std:(" participants..."));
            }
            if (config->agents >= 500) {
                if (i % 100 == 0) {
                    auto currentMemory = process->memoryUsage()->heapUsed / 1024 / 1024;
                    auto thresholds = const_(THRESHOLD_CONFIGS)[thresholdLevel];
                    if (currentMemory > thresholds->memoryThreshold) {
                        logger->warn(std:("Memory threshold exceeded during participant creation: ") + currentMemory->toFixed(2) + std:("MB (") + thresholdLevel + std:(" threshold: ") + thresholds->memoryThreshold + std:("MB)"));
                        metrics->errorTypes[std:("excessive_memory")] = (OR((const_(metrics->errorTypes)[std:("excessive_memory")]), (0))) + 1;
                        throw any(std::make_shared<Error>(std:("Memory threshold exceeded during participant creation: ") + currentMemory->toFixed(2) + std:("MB")));
                    }
                }
            }
        }
        logger->info(std:("All ") + config->agents + std:(" participants created successfully"));
        logger->info(std:("Sending ") + config->messageCount + std:(" test messages..."));
        for (auto i = 0; i < config->messageCount; i++)
        {
            auto messageStart = Date->now();
            auto senderId = const_(participants)[i % participants->get_length()];
            auto messageText = this->generateTestMessage(i, config);
            try
            {
                std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, messageText, senderId); });
                metrics->messagesSent++;
                try
                {
                    auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](10000); });
                    if (!completed) {
                        metrics->timeoutCount++;
                        metrics->errorTypes[std:("timeout")] = (OR((const_(metrics->errorTypes)[std:("timeout")]), (0))) + 1;
                        throw any(std::make_shared<Error>(std:("Message processing timeout for message ") + i + string_empty));
                    }
                    auto responseTime = Date->now() - messageStart;
                    metrics->messagesProcessed++;
                    metrics->responseTimes->push(responseTime);
                    metrics->minResponseTime = Math->min(metrics->minResponseTime, responseTime);
                    metrics->maxResponseTime = Math->max(metrics->maxResponseTime, responseTime);
                    if (OR((i % 10 == 0), (i == config->messageCount - 1))) {
                        logger->info(std:("Processed ") + (i + 1) + std:("/") + config->messageCount + std:(" messages (last: ") + responseTime + std:("ms)"));
                    }
                    auto thresholds = const_(THRESHOLD_CONFIGS)[thresholdLevel];
                    if (responseTime > thresholds->responseTimeThreshold) {
                        logger->warn(std:("Response time threshold exceeded: ") + responseTime + std:("ms (") + thresholdLevel + std:(" threshold: ") + thresholds->responseTimeThreshold + std:("ms)"));
                        metrics->errorTypes[std:("slow_response")] = (OR((const_(metrics->errorTypes)[std:("slow_response")]), (0))) + 1;
                        if (const_(metrics->errorTypes)[std:("slow_response")] > 5) {
                            throw any(std::make_shared<Error>(std:("Too many slow responses detected (>") + responseTime + std:("ms)")));
                        }
                    }
                }
                catch (const any& timeoutError)
                {
                    metrics->errorCount++;
                    if (AND((is<Error>(timeoutError)), (timeoutError->message->includes(std:("timeout"))))) {
                        metrics->timeoutCount++;
                        metrics->errorTypes[std:("timeout")] = (OR((const_(metrics->errorTypes)[std:("timeout")]), (0))) + 1;
                    } else {
                        metrics->errorTypes[std:("processing")] = (OR((const_(metrics->errorTypes)[std:("processing")]), (0))) + 1;
                    }
                    logger->error(std:("Error processing message ") + i + std:(":"), timeoutError);
                }
            }
            catch (const any& sendError)
            {
                metrics->errorCount++;
                metrics->errorTypes[std:("send_failure")] = (OR((const_(metrics->errorTypes)[std:("send_failure")]), (0))) + 1;
                logger->error(std:("Error sending message ") + i + std:(":"), sendError);
            }
            if (OR((i % 10 == 0), (i == config->messageCount - 1))) {
                auto currentMemory = process->memoryUsage()->heapUsed / 1024 / 1024;
                metrics->peakMemoryUsage = Math->max(metrics->peakMemoryUsage, currentMemory);
                this->snapshotSystemMetrics();
                auto thresholds = const_(THRESHOLD_CONFIGS)[thresholdLevel];
                if (OR((currentMemory > thresholds->memoryThreshold), (const_(this->systemMetrics->cpuUsage)[this->systemMetrics->cpuUsage->get_length() - 1] > thresholds->cpuThreshold))) {
                    logger->warn(std:("System resource limits approaching (") + thresholdLevel + std:(" threshold): Memory=") + currentMemory->toFixed(2) + std:("MB/") + thresholds->memoryThreshold + std:("MB, ") + std:("CPU=") + const_(this->systemMetrics->cpuUsage)[this->systemMetrics->cpuUsage->get_length() - 1] + std:("/") + thresholds->cpuThreshold + string_empty);
                    metrics->errorTypes[std:("resource_limit")] = (OR((const_(metrics->errorTypes)[std:("resource_limit")]), (0))) + 1;
                    if (const_(metrics->errorTypes)[std:("resource_limit")] > 3) {
                        throw any(std::make_shared<Error>(std:("System resource limits exceeded repeatedly")));
                    }
                }
            }
            shared delayTime = (config->agents > 500) ? 200 : 100;
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, delayTime);
            }
            ); });
        }
        metrics->memoryUsageEnd = process->memoryUsage()->heapUsed / 1024 / 1024;
    }
    catch (const any& error)
    {
        metrics->errorCount++;
        logger->error(std:("Load test failed:"), error);
        throw any(error);
    }
    metrics->totalTime = Date->now() - startTime;
    metrics->avgResponseTime = (metrics->responseTimes->get_length() > 0) ? any(metrics->responseTimes->reduce([=](auto a, auto b) mutable
    {
        return a + b;
    }
    , 0) / metrics->responseTimes->get_length()) (0);
    metrics->successRate = (metrics->messagesSent > 0) ? any((metrics->messagesProcessed / metrics->messagesSent) * 100) (0);
    metrics->throughput = (metrics->messagesProcessed > 0) ? any((metrics->messagesProcessed / metrics->totalTime) * 1000) (0);
    this->reportMetrics(config, metrics, thresholdLevel);
    return metrics;
}

void AgentLoadTestSuite::startSystemMetricsMonitoring()
{
    this->systemMetrics = object{
        object::pair{std:("cpuUsage"), array<any>()}, 
        object::pair{std:("memoryUsage"), array<any>()}, 
        object::pair{std:("timestamp"), array<any>()}
    };
    this->snapshotSystemMetrics();
}

void AgentLoadTestSuite::snapshotSystemMetrics()
{
    try
    {
        auto cpuLoad = const_(os->loadavg())[0] / os->cpus()->length;
        auto totalMem = os->totalmem();
        auto freeMem = os->freemem();
        auto usedMemPercentage = (totalMem - freeMem) / totalMem;
        this->systemMetrics->cpuUsage->push(cpuLoad);
        this->systemMetrics->memoryUsage->push(usedMemPercentage);
        this->systemMetrics->timestamp->push(Date->now());
    }
    catch (const any& error)
    {
        logger->error(std:("Error capturing system metrics:"), error);
    }
}

void AgentLoadTestSuite::stopSystemMetricsMonitoring()
{
    this->snapshotSystemMetrics();
}

void AgentLoadTestSuite::writeSystemMetrics(string filePath)
{
    try
    {
        auto content = std:("System Metrics During Load Testing\
");
        content += std:("===\
\
");
        content += std:("Timestamp,CPU Load,Memory Usage (%)\
");
        for (auto i = 0; i < this->systemMetrics->timestamp->get_length(); i++)
        {
            auto time = ((std::make_shared<Date>(const_(this->systemMetrics->timestamp)[i])))->toISOString();
            auto cpu = const_(this->systemMetrics->cpuUsage)[i]->toFixed(3);
            auto mem = (const_(this->systemMetrics->memoryUsage)[i] * 100)->toFixed(2);
            content += string_empty + time + std:(",") + cpu + std:(",") + mem + std:("\
");
        }
        fs->writeFileSync(filePath, content);
    }
    catch (const any& error)
    {
        logger->error(std:("Error writing system metrics:"), error);
    }
}

Record<ThresholdLevel, object> AgentLoadTestSuite::determineOptimalScalingPerThreshold(string summaryFile)
{
    auto results = object{
        object::pair{ThresholdLevel::LOW, object{
            object::pair{std:("agents"), 0}, 
            object::pair{std:("messageCount"), 0}, 
            object::pair{std:("description"), string_empty}, 
            object::pair{std:("throughput"), 0}
        }}, 
        object::pair{ThresholdLevel::MEDIUM, object{
            object::pair{std:("agents"), 0}, 
            object::pair{std:("messageCount"), 0}, 
            object::pair{std:("description"), string_empty}, 
            object::pair{std:("throughput"), 0}
        }}, 
        object::pair{ThresholdLevel::HIGH, object{
            object::pair{std:("agents"), 0}, 
            object::pair{std:("messageCount"), 0}, 
            object::pair{std:("description"), string_empty}, 
            object::pair{std:("throughput"), 0}
        }}
    };
    try
    {
        auto summary = fs->readFileSync(summaryFile, std:("utf8"));
        auto& __array17959_19591 = Object->keys(ThresholdLevel);
        for (auto __indx17959_19591 = 0_N; __indx17959_19591 < __array17959_19591->get_length(); __indx17959_19591++)
        {
            auto& level = const_(__array17959_19591)[__indx17959_19591];
            {
                shared thresholdLevel = as<ThresholdLevel>(level);
                auto rows = summary->split(std:("\
"))->filter([=](auto line) mutable
                {
                    return line["startsWith"](string_empty + thresholdLevel + std:(" - "));
                }
                )->filter([=](auto line) mutable
                {
                    return AND((!line["includes"](std:("FAILED"))), (!line["includes"](std:("SKIPPED"))));
                }
                );
                if (rows->length > 0) {
                    auto lastSuccessful = const_(rows)[rows->length - 1];
                    auto parts = lastSuccessful->split(std:("|"))->map([=](auto p) mutable
                    {
                        return p["trim"]();
                    }
                    );
                    auto fullDescription = const_(parts)[0];
                    auto description = fullDescription->substring(fullDescription->indexOf(std:("-")) + 1)->trim();
                    auto agents = parseInt(const_(parts)[1]);
                    auto messageCount = parseInt(const_(parts)[2]);
                    auto throughput = parseFloat(const_(parts)[6]);
                    results[thresholdLevel] = object{
                        object::pair{std:("agents"), std:("agents")}, 
                        object::pair{std:("messageCount"), std:("messageCount")}, 
                        object::pair{std:("description"), std:("description")}, 
                        object::pair{std:("throughput"), std:("throughput")}
                    };
                    logger->info(std:("Optimal configuration for ") + thresholdLevel + std:(" threshold: ") + agents + std:(" agents with throughput of ") + throughput->toFixed(2) + std:(" msg/s"));
                } else {
                    results[thresholdLevel] = object{
                        object::pair{std:("agents"), 2}, 
                        object::pair{std:("messageCount"), 10}, 
                        object::pair{std:("description"), std:("Minimal Load (2 agents)")}, 
                        object::pair{std:("throughput"), 0}
                    };
                    logger->warn(std:("No successful tests for ") + thresholdLevel + std:(" threshold, using default minimal configuration"));
                }
            }
        }
    }
    catch (const any& error)
    {
        logger->error(std:("Error determining optimal scaling:"), error);
        return object{
            object::pair{ThresholdLevel::LOW, object{
                object::pair{std:("agents"), 2}, 
                object::pair{std:("messageCount"), 10}, 
                object::pair{std:("description"), std:("Minimal Load (2 agents)")}, 
                object::pair{std:("throughput"), 0.5}
            }}, 
            object::pair{ThresholdLevel::MEDIUM, object{
                object::pair{std:("agents"), 5}, 
                object::pair{std:("messageCount"), 20}, 
                object::pair{std:("description"), std:("Small Group (5 agents)")}, 
                object::pair{std:("throughput"), 1}
            }}, 
            object::pair{ThresholdLevel::HIGH, object{
                object::pair{std:("agents"), 10}, 
                object::pair{std:("messageCount"), 50}, 
                object::pair{std:("description"), std:("Medium Group (10 agents)")}, 
                object::pair{std:("throughput"), 2}
            }}
        };
    }
    return results;
}

string AgentLoadTestSuite::generateTestMessage(double index, std::shared_ptr<ScaleConfig> config)
{
    return std:("Test message ") + index + std:(" from scalability test with ") + config->agents + std:(" agents.");
}

void AgentLoadTestSuite::reportMetrics(std::shared_ptr<ScaleConfig> config, std::shared_ptr<LoadTestMetrics> metrics, ThresholdLevel thresholdLevel)
{
    logger->info(std:("\
== Test Results =="));
    logger->info(std:("Configuration: ") + config->description + std:(" (") + thresholdLevel + std:(" threshold)"));
    logger->info(std:("Number of Agents: ") + config->agents + string_empty);
    logger->info(std:("Messages Sent: ") + metrics->messagesSent + string_empty);
    logger->info(std:("Messages Successfully Processed: ") + metrics->messagesProcessed + string_empty);
    logger->info(std:("Error Count: ") + metrics->errorCount + string_empty);
    logger->info(std:("Timeout Count: ") + metrics->timeoutCount + string_empty);
    logger->info(std:("Success Rate: ") + metrics->successRate->toFixed(2) + std:("%"));
    logger->info(std:("Total Test Time: ") + metrics->totalTime + std:("ms (") + (metrics->totalTime / 1000)->toFixed(2) + std:("s)"));
    logger->info(std:("Average Response Time: ") + metrics->avgResponseTime->toFixed(2) + std:("ms"));
    logger->info(std:("Min Response Time: ") + metrics->minResponseTime + std:("ms"));
    logger->info(std:("Max Response Time: ") + metrics->maxResponseTime + std:("ms"));
    logger->info(std:("Memory Usage: ") + metrics->memoryUsageStart->toFixed(2) + std:("MB → ") + metrics->memoryUsageEnd->toFixed(2) + std:("MB"));
    logger->info(std:("Peak Memory Usage: ") + metrics->peakMemoryUsage->toFixed(2) + std:("MB"));
    logger->info(std:("Throughput: ") + metrics->throughput->toFixed(2) + std:(" messages/second"));
    if (Object->keys(metrics->errorTypes)->get_length() > 0) {
        logger->info(std:("Error Types:"));
        for (auto& [type, count] : Object->entries(metrics->errorTypes))
        {
            logger->info(std:("  - ") + type + std:(": ") + count + string_empty);
        }
    }
    logger->info(std:("===\
"));
}

