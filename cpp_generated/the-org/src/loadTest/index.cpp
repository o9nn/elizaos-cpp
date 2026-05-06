#include "/home/runner/work/elizaos-cpp/elizaos-cpp/the-org/src/loadTest/index.h"

std::shared_ptr<Promise<std::shared_ptr<LoadTestMetrics>>> AgentLoadTestSuite::runSequentialLoadTest(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<ScaleConfig> config, ThresholdLevel thresholdLevel)
{
    auto startTime = Date->now();
    auto metrics = object{
        object::pair{std::string("totalTime"), 0}, 
        object::pair{std::string("errorCount"), 0}, 
        object::pair{std::string("messagesSent"), 0}, 
        object::pair{std::string("messagesProcessed"), 0}, 
        object::pair{std::string("avgResponseTime"), 0}, 
        object::pair{std::string("minResponseTime"), Number->MAX_SAFE_INTEGER}, 
        object::pair{std::string("maxResponseTime"), 0}, 
        object::pair{std::string("responseTimes"), array<any>()}, 
        object::pair{std::string("peakMemoryUsage"), 0}, 
        object::pair{std::string("memoryUsageStart"), process->memoryUsage()->heapUsed / 1024 / 1024}, 
        object::pair{std::string("memoryUsageEnd"), 0}, 
        object::pair{std::string("successRate"), 0}, 
        object::pair{std::string("throughput"), 0}, 
        object::pair{std::string("timeoutCount"), 0}, 
        object::pair{std::string("errorTypes"), object{}}
    };
    try
    {
        logger->info(std::string("Creating test world for ") + config->description + string_empty);
        auto worldId = std::async([=]() { this->scenarioService["createWorld"](std::string("Load Test - ") + config->agents + std::string(" agents"), std::string("Test Admin")); });
        auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std::string("load-test-") + config->agents + string_empty); });
        std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, runtime->agentId); });
        auto participants = array<std::shared_ptr<UUID>>();
        logger->info(std::string("Creating ") + config->agents + std::string(" participants..."));
        for (auto i = 0; i < config->agents; i++)
        {
            auto participantId = as<std::shared_ptr<UUID>>(uuidv4());
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, participantId); });
            participants->push(participantId);
            if (AND((config->agents > 20), (i % 10 == 0))) {
                logger->info(std::string("Created ") + (i + 1) + std::string("/") + config->agents + std::string(" participants..."));
            }
            if (config->agents >= 500) {
                if (i % 100 == 0) {
                    auto currentMemory = process->memoryUsage()->heapUsed / 1024 / 1024;
                    auto thresholds = const_(THRESHOLD_CONFIGS)[thresholdLevel];
                    if (currentMemory > thresholds->memoryThreshold) {
                        logger->warn(std::string("Memory threshold exceeded during participant creation: ") + currentMemory->toFixed(2) + std::string("MB (") + thresholdLevel + std::string(" threshold: ") + thresholds->memoryThreshold + std::string("MB)"));
                        metrics->errorTypes[std::string("excessive_memory")] = (OR((const_(metrics->errorTypes)[std::string("excessive_memory")]), (0))) + 1;
                        throw any(std::make_shared<Error>(std::string("Memory threshold exceeded during participant creation: ") + currentMemory->toFixed(2) + std::string("MB")));
                    }
                }
            }
        }
        logger->info(std::string("All ") + config->agents + std::string(" participants created successfully"));
        logger->info(std::string("Sending ") + config->messageCount + std::string(" test messages..."));
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
                        metrics->errorTypes[std::string("timeout")] = (OR((const_(metrics->errorTypes)[std::string("timeout")]), (0))) + 1;
                        throw any(std::make_shared<Error>(std::string("Message processing timeout for message ") + i + string_empty));
                    }
                    auto responseTime = Date->now() - messageStart;
                    metrics->messagesProcessed++;
                    metrics->responseTimes->push(responseTime);
                    metrics->minResponseTime = Math->min(metrics->minResponseTime, responseTime);
                    metrics->maxResponseTime = Math->max(metrics->maxResponseTime, responseTime);
                    if (OR((i % 10 == 0), (i == config->messageCount - 1))) {
                        logger->info(std::string("Processed ") + (i + 1) + std::string("/") + config->messageCount + std::string(" messages (last: ") + responseTime + std::string("ms)"));
                    }
                    auto thresholds = const_(THRESHOLD_CONFIGS)[thresholdLevel];
                    if (responseTime > thresholds->responseTimeThreshold) {
                        logger->warn(std::string("Response time threshold exceeded: ") + responseTime + std::string("ms (") + thresholdLevel + std::string(" threshold: ") + thresholds->responseTimeThreshold + std::string("ms)"));
                        metrics->errorTypes[std::string("slow_response")] = (OR((const_(metrics->errorTypes)[std::string("slow_response")]), (0))) + 1;
                        if (const_(metrics->errorTypes)[std::string("slow_response")] > 5) {
                            throw any(std::make_shared<Error>(std::string("Too many slow responses detected (>") + responseTime + std::string("ms)")));
                        }
                    }
                }
                catch (const any& timeoutError)
                {
                    metrics->errorCount++;
                    if (AND((is<Error>(timeoutError)), (timeoutError->message->includes(std::string("timeout"))))) {
                        metrics->timeoutCount++;
                        metrics->errorTypes[std::string("timeout")] = (OR((const_(metrics->errorTypes)[std::string("timeout")]), (0))) + 1;
                    } else {
                        metrics->errorTypes[std::string("processing")] = (OR((const_(metrics->errorTypes)[std::string("processing")]), (0))) + 1;
                    }
                    logger->error(std::string("Error processing message ") + i + std::string(":"), timeoutError);
                }
            }
            catch (const any& sendError)
            {
                metrics->errorCount++;
                metrics->errorTypes[std::string("send_failure")] = (OR((const_(metrics->errorTypes)[std::string("send_failure")]), (0))) + 1;
                logger->error(std::string("Error sending message ") + i + std::string(":"), sendError);
            }
            if (OR((i % 10 == 0), (i == config->messageCount - 1))) {
                auto currentMemory = process->memoryUsage()->heapUsed / 1024 / 1024;
                metrics->peakMemoryUsage = Math->max(metrics->peakMemoryUsage, currentMemory);
                this->snapshotSystemMetrics();
                auto thresholds = const_(THRESHOLD_CONFIGS)[thresholdLevel];
                if (OR((currentMemory > thresholds->memoryThreshold), (const_(this->systemMetrics->cpuUsage)[this->systemMetrics->cpuUsage->get_length() - 1] > thresholds->cpuThreshold))) {
                    logger->warn(std::string("System resource limits approaching (") + thresholdLevel + std::string(" threshold): Memory=") + currentMemory->toFixed(2) + std::string("MB/") + thresholds->memoryThreshold + std::string("MB, ") + std::string("CPU=") + const_(this->systemMetrics->cpuUsage)[this->systemMetrics->cpuUsage->get_length() - 1] + std::string("/") + thresholds->cpuThreshold + string_empty);
                    metrics->errorTypes[std::string("resource_limit")] = (OR((const_(metrics->errorTypes)[std::string("resource_limit")]), (0))) + 1;
                    if (const_(metrics->errorTypes)[std::string("resource_limit")] > 3) {
                        throw any(std::make_shared<Error>(std::string("System resource limits exceeded repeatedly")));
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
        logger->error(std::string("Load test failed:"), error);
        throw any(error);
    }
    metrics->totalTime = Date->now() - startTime;
    metrics->avgResponseTime = (metrics->responseTimes->get_length() > 0) ? any(metrics->responseTimes->reduce([=](auto a, auto b) mutable
    {
        return a + b;
    }
    , 0) / metrics->responseTimes->get_length()) : any(0);
    metrics->successRate = (metrics->messagesSent > 0) ? any((metrics->messagesProcessed / metrics->messagesSent) * 100) : any(0);
    metrics->throughput = (metrics->messagesProcessed > 0) ? any((metrics->messagesProcessed / metrics->totalTime) * 1000) : any(0);
    this->reportMetrics(config, metrics, thresholdLevel);
    return metrics;
}

void AgentLoadTestSuite::startSystemMetricsMonitoring()
{
    this->systemMetrics = object{
        object::pair{std::string("cpuUsage"), array<any>()}, 
        object::pair{std::string("memoryUsage"), array<any>()}, 
        object::pair{std::string("timestamp"), array<any>()}
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
        logger->error(std::string("Error capturing system metrics:"), error);
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
        auto content = std::string("System Metrics During Load Testing\
");
        content += std::string("======================\
\
");
        content += std::string("Timestamp,CPU Load,Memory Usage (%)\
");
        for (auto i = 0; i < this->systemMetrics->timestamp->get_length(); i++)
        {
            auto time = ((std::make_shared<Date>(const_(this->systemMetrics->timestamp)[i])))->toISOString();
            auto cpu = const_(this->systemMetrics->cpuUsage)[i]->toFixed(3);
            auto mem = (const_(this->systemMetrics->memoryUsage)[i] * 100)->toFixed(2);
            content += string_empty + time + std::string(",") + cpu + std::string(",") + mem + std::string("\
");
        }
        fs->writeFileSync(filePath, content);
    }
    catch (const any& error)
    {
        logger->error(std::string("Error writing system metrics:"), error);
    }
}

Record<ThresholdLevel, object> AgentLoadTestSuite::determineOptimalScalingPerThreshold(string summaryFile)
{
    auto results = object{
        object::pair{ThresholdLevel::LOW, object{
            object::pair{std::string("agents"), 0}, 
            object::pair{std::string("messageCount"), 0}, 
            object::pair{std::string("description"), string_empty}, 
            object::pair{std::string("throughput"), 0}
        }}, 
        object::pair{ThresholdLevel::MEDIUM, object{
            object::pair{std::string("agents"), 0}, 
            object::pair{std::string("messageCount"), 0}, 
            object::pair{std::string("description"), string_empty}, 
            object::pair{std::string("throughput"), 0}
        }}, 
        object::pair{ThresholdLevel::HIGH, object{
            object::pair{std::string("agents"), 0}, 
            object::pair{std::string("messageCount"), 0}, 
            object::pair{std::string("description"), string_empty}, 
            object::pair{std::string("throughput"), 0}
        }}
    };
    try
    {
        auto summary = fs->readFileSync(summaryFile, std::string("utf8"));
        auto& __array17959_19591 = Object->keys(ThresholdLevel);
        for (auto __indx17959_19591 = 0_N; __indx17959_19591 < __array17959_19591->get_length(); __indx17959_19591++)
        {
            auto& level = const_(__array17959_19591)[__indx17959_19591];
            {
                shared thresholdLevel = as<ThresholdLevel>(level);
                auto rows = summary->split(std::string("\
"))->filter([=](auto line) mutable
                {
                    return line["startsWith"](string_empty + thresholdLevel + std::string(" - "));
                }
                )->filter([=](auto line) mutable
                {
                    return AND((!line["includes"](std::string("FAILED"))), (!line["includes"](std::string("SKIPPED"))));
                }
                );
                if (rows->length > 0) {
                    auto lastSuccessful = const_(rows)[rows->length - 1];
                    auto parts = lastSuccessful->split(std::string("|"))->map([=](auto p) mutable
                    {
                        return p["trim"]();
                    }
                    );
                    auto fullDescription = const_(parts)[0];
                    auto description = fullDescription->substring(fullDescription->indexOf(std::string("-")) + 1)->trim();
                    auto agents = parseInt(const_(parts)[1]);
                    auto messageCount = parseInt(const_(parts)[2]);
                    auto throughput = parseFloat(const_(parts)[6]);
                    results[thresholdLevel] = object{
                        object::pair{std::string("agents"), std::string("agents")}, 
                        object::pair{std::string("messageCount"), std::string("messageCount")}, 
                        object::pair{std::string("description"), std::string("description")}, 
                        object::pair{std::string("throughput"), std::string("throughput")}
                    };
                    logger->info(std::string("Optimal configuration for ") + thresholdLevel + std::string(" threshold: ") + agents + std::string(" agents with throughput of ") + throughput->toFixed(2) + std::string(" msg/s"));
                } else {
                    results[thresholdLevel] = object{
                        object::pair{std::string("agents"), 2}, 
                        object::pair{std::string("messageCount"), 10}, 
                        object::pair{std::string("description"), std::string("Minimal Load (2 agents)")}, 
                        object::pair{std::string("throughput"), 0}
                    };
                    logger->warn(std::string("No successful tests for ") + thresholdLevel + std::string(" threshold, using default minimal configuration"));
                }
            }
        }
    }
    catch (const any& error)
    {
        logger->error(std::string("Error determining optimal scaling:"), error);
        return object{
            object::pair{ThresholdLevel::LOW, object{
                object::pair{std::string("agents"), 2}, 
                object::pair{std::string("messageCount"), 10}, 
                object::pair{std::string("description"), std::string("Minimal Load (2 agents)")}, 
                object::pair{std::string("throughput"), 0.5}
            }}, 
            object::pair{ThresholdLevel::MEDIUM, object{
                object::pair{std::string("agents"), 5}, 
                object::pair{std::string("messageCount"), 20}, 
                object::pair{std::string("description"), std::string("Small Group (5 agents)")}, 
                object::pair{std::string("throughput"), 1}
            }}, 
            object::pair{ThresholdLevel::HIGH, object{
                object::pair{std::string("agents"), 10}, 
                object::pair{std::string("messageCount"), 50}, 
                object::pair{std::string("description"), std::string("Medium Group (10 agents)")}, 
                object::pair{std::string("throughput"), 2}
            }}
        };
    }
    return results;
}

string AgentLoadTestSuite::generateTestMessage(double index, std::shared_ptr<ScaleConfig> config)
{
    return std::string("Test message ") + index + std::string(" from scalability test with ") + config->agents + std::string(" agents.");
}

void AgentLoadTestSuite::reportMetrics(std::shared_ptr<ScaleConfig> config, std::shared_ptr<LoadTestMetrics> metrics, ThresholdLevel thresholdLevel)
{
    logger->info(std::string("\
======= Test Results ======="));
    logger->info(std::string("Configuration: ") + config->description + std::string(" (") + thresholdLevel + std::string(" threshold)"));
    logger->info(std::string("Number of Agents: ") + config->agents + string_empty);
    logger->info(std::string("Messages Sent: ") + metrics->messagesSent + string_empty);
    logger->info(std::string("Messages Successfully Processed: ") + metrics->messagesProcessed + string_empty);
    logger->info(std::string("Error Count: ") + metrics->errorCount + string_empty);
    logger->info(std::string("Timeout Count: ") + metrics->timeoutCount + string_empty);
    logger->info(std::string("Success Rate: ") + metrics->successRate->toFixed(2) + std::string("%"));
    logger->info(std::string("Total Test Time: ") + metrics->totalTime + std::string("ms (") + (metrics->totalTime / 1000)->toFixed(2) + std::string("s)"));
    logger->info(std::string("Average Response Time: ") + metrics->avgResponseTime->toFixed(2) + std::string("ms"));
    logger->info(std::string("Min Response Time: ") + metrics->minResponseTime + std::string("ms"));
    logger->info(std::string("Max Response Time: ") + metrics->maxResponseTime + std::string("ms"));
    logger->info(std::string("Memory Usage: ") + metrics->memoryUsageStart->toFixed(2) + std::string("MB → ") + metrics->memoryUsageEnd->toFixed(2) + std::string("MB"));
    logger->info(std::string("Peak Memory Usage: ") + metrics->peakMemoryUsage->toFixed(2) + std::string("MB"));
    logger->info(std::string("Throughput: ") + metrics->throughput->toFixed(2) + std::string(" messages/second"));
    if (Object->keys(metrics->errorTypes)->get_length() > 0) {
        logger->info(std::string("Error Types:"));
        for (auto& [type, count] : Object->entries(metrics->errorTypes))
        {
            logger->info(std::string("  - ") + type + std::string(": ") + count + string_empty);
        }
    }
    logger->info(std::string("=======================\
"));
}

