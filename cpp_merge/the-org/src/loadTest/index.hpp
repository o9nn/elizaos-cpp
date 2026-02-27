#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_THE_ORG_SRC_LOADTEST_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_THE_ORG_SRC_LOADTEST_INDEX_H
#include "core.hpp"
// External dependency removed
#include "uuid.hpp"
using uuidv4 = v4;
#include "./types.h"
#include "fs.hpp"
// Using alias removed (invalid transpilation)
#include "path.hpp"
// Using alias removed (invalid transpilation)
#include "url.hpp"
#include "os.hpp"
// Using alias removed (invalid transpilation)
#include "./utils.h"

class AgentLoadTestSuite;

class AgentLoadTestSuite : public TestSuite, public std::enable_shared_from_this<AgentLoadTestSuite> {
public:
    using std::enable_shared_from_this<AgentLoadTestSuite>::shared_from_this;
    string name = std:("agent-load-test");

    string description = std:("Load testing for agent scalability");

    any scenarioService;

    boolean breakingPointReached = false;

    std::shared_ptr<SystemMetrics> systemMetrics = object{
        object::pair{std:("cpuUsage"), array<any>()}, 
        object::pair{std:("memoryUsage"), array<any>()}, 
        object::pair{std:("timestamp"), array<any>()}
    };

    array<std::shared_ptr<ScaleConfig>> SCALE_CONFIGS = array<std::shared_ptr<ScaleConfig>>{ object{
        object::pair{std:("agents"), 2}, 
        object::pair{std:("messageCount"), 10}, 
        object::pair{std:("description"), std:("Minimal Load (2 agents)")}
    }, object{
        object::pair{std:("agents"), 5}, 
        object::pair{std:("messageCount"), 20}, 
        object::pair{std:("description"), std:("Small Group (5 agents)")}
    }, object{
        object::pair{std:("agents"), 10}, 
        object::pair{std:("messageCount"), 50}, 
        object::pair{std:("description"), std:("Medium Group (10 agents)")}
    }, object{
        object::pair{std:("agents"), 50}, 
        object::pair{std:("messageCount"), 100}, 
        object::pair{std:("description"), std:("Large Group (50 agents)")}
    }, object{
        object::pair{std:("agents"), 100}, 
        object::pair{std:("messageCount"), 150}, 
        object::pair{std:("description"), std:("Very Large Group (100 agents)")}
    }, object{
        object::pair{std:("agents"), 250}, 
        object::pair{std:("messageCount"), 200}, 
        object::pair{std:("description"), std:("Mass Scale (250 agents)")}
    }, object{
        object::pair{std:("agents"), 500}, 
        object::pair{std:("messageCount"), 300}, 
        object::pair{std:("description"), std:("Extreme Scale (500 agents)")}
    }, object{
        object::pair{std:("agents"), 1000}, 
        object::pair{std:("messageCount"), 400}, 
        object::pair{std:("description"), std:("Maximum Load (1000 agents)")}
    }, object{
        object::pair{std:("agents"), 2000}, 
        object::pair{std:("messageCount"), 500}, 
        object::pair{std:("description"), std:("Breaking Point (2000 agents)")}
    } };

    array<object> tests = array<object>{ object{
        object::pair{std:("name"), std:("Scale Testing - Sequential Load")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std:("scenario"));
            if (!this->scenarioService) {
                throw any(std::make_shared<Error>(std:("Scenario service not found")));
            }
            auto logsDir = ensureLogsDirectory();
            auto timestamp = generateTimestamp();
            auto summaryLogFile = path->join(logsDir, std:("load-test-summary-") + timestamp + std:(".log"));
            auto systemMetricsFile = path->join(logsDir, std:("system-metrics-") + timestamp + std:(".log"));
            this->startSystemMetricsMonitoring();
            fs->writeFileSync(summaryLogFile, createLoadTestSummaryHeader());
            logger->info(std:("Starting sequential load tests to find breaking points"));
            auto thresholdLevels = array<ThresholdLevel>{ ThresholdLevel::LOW, ThresholdLevel::MEDIUM, ThresholdLevel::HIGH };
            for (auto& thresholdLevel : thresholdLevels)
            {
                logger->info(std:("\
Starting test run with ") + thresholdLevel + std:(" threshold settings"));
                auto breakingPointIdentified = false;
                for (auto& config : this->SCALE_CONFIGS)
                {
                    if (breakingPointIdentified) {
                        fs->appendFileSync(summaryLogFile, string_empty + config->description->padEnd(13) + std:(" | ") + String(config->agents)->padEnd(6) + std:(" | ") + string_empty + String(config->messageCount)->padEnd(8) + std:(" | SKIPPED (") + thresholdLevel + std:(") | - | - | - | -\
"));
                        continue;
                    }
                    auto detailedLogFile = path->join(logsDir, std:("load-test-") + thresholdLevel + std:("-") + config->agents + std:("-agents-") + timestamp + std:(".log"));
                    try
                    {
                        fs->writeFileSync(detailedLogFile, createDetailedTestLogHeader(config));
                        fs->appendFileSync(detailedLogFile, std:("Threshold Level: ") + thresholdLevel + std:("\
\
"));
                        logger->info(std:("\
Starting load test for ") + config->description + std:(" with ") + thresholdLevel + std:(" threshold"));
                        auto metrics = std::async([=]() { this->runSequentialLoadTest(runtime, config, thresholdLevel); });
                        if (isBreakingPoint(metrics, thresholdLevel)) {
                            breakingPointIdentified = true;
                            logger->warn(std:("BREAKING POINT IDENTIFIED at ") + config->agents + std:(" agents with ") + thresholdLevel + std:(" threshold!"));
                            fs->appendFileSync(detailedLogFile, formatBreakingPointNotification(metrics, thresholdLevel));
                        }
                        fs->appendFileSync(detailedLogFile, formatDetailedMetrics(config, metrics));
                        fs->appendFileSync(summaryLogFile, string_empty + thresholdLevel + std:(" - ") + formatSummaryLogLine(config, metrics));
                    }
                    catch (const any& error)
                    {
                        breakingPointIdentified = true;
                        logger->error(std:("BREAKING POINT: Error running test for ") + config->description + std:(" with ") + thresholdLevel + std:(" threshold:"), error);
                        fs->appendFileSync(detailedLogFile, std:("\
*** FATAL ERROR - BREAKING POINT IDENTIFIED (") + thresholdLevel + std:(" threshold) ***\
") + std:("Test failed catastrophically at ") + config->agents + std:(" agents with error:\
") + string_empty + (is<Error>(error)) ? error->message : String(error) + std:("\
"));
                        fs->appendFileSync(summaryLogFile, string_empty + thresholdLevel + std:(" - ") + config->description->padEnd(13) + std:(" | ") + String(config->agents)->padEnd(6) + std:(" | ") + string_empty + String(config->messageCount)->padEnd(8) + std:(" | FAILED | - | - | - | FATAL\
"));
                    }
                }
                fs->appendFileSync(summaryLogFile, std:("\
------------------------------------------------\
\
"));
            }
            this->stopSystemMetricsMonitoring();
            this->writeSystemMetrics(systemMetricsFile);
            auto optimalConfigs = this->determineOptimalScalingPerThreshold(summaryLogFile);
            fs->appendFileSync(summaryLogFile, std:("\
\
SCALABILITY ANALYSIS\
") + std:("==\
"));
            for (auto& [level, config] : Object->entries(optimalConfigs))
            {
                fs->appendFileSync(summaryLogFile, string_empty + level + std:(" Threshold:\
") + std:("- Breaking Point: Identified\
") + std:("- Last stable configuration: ") + config["description"] + std:("\
") + std:("- Recommended maximum agents: ") + config["agents"] + std:("\
") + std:("- Recommended maximum messages: ") + config["messageCount"] + std:("\
") + std:("- Optimal throughput: ~") + config["throughput"]->toFixed(2) + std:(" messages/second\
\
"));
            }
            fs->appendFileSync(summaryLogFile, std:("Test Completed: ") + ((std::make_shared<Date>()))->toISOString() + std:("\
") + std:("==\
"));
        }
        }
    } };

    virtual std::shared_ptr<Promise<std::shared_ptr<LoadTestMetrics>>> runSequentialLoadTest(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<ScaleConfig> config, ThresholdLevel thresholdLevel = ThresholdLevel::MEDIUM);
    virtual void startSystemMetricsMonitoring();
    virtual void snapshotSystemMetrics();
    virtual void stopSystemMetricsMonitoring();
    virtual void writeSystemMetrics(string filePath);
    virtual Record<ThresholdLevel, object> determineOptimalScalingPerThreshold(string summaryFile);
    virtual string generateTestMessage(double index, std::shared_ptr<ScaleConfig> config);
    virtual void reportMetrics(std::shared_ptr<ScaleConfig> config, std::shared_ptr<LoadTestMetrics> metrics, ThresholdLevel thresholdLevel = ThresholdLevel::MEDIUM);
};

#endif
