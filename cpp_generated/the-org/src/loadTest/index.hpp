#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_SRC_LOADTEST_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_SRC_LOADTEST_INDEX_H
#include "core.h"
#include "@elizaos/core.h"
#include "uuid.h"
using uuidv4 = v4;
#include "./types.h"
#include "fs.h"
using fs = _default;
#include "path.h"
using path = _default;
#include "url.h"
#include "os.h"
using os = _default;
#include "./utils.h"

class AgentLoadTestSuite;

class AgentLoadTestSuite : public TestSuite, public std::enable_shared_from_this<AgentLoadTestSuite> {
public:
    using std::enable_shared_from_this<AgentLoadTestSuite>::shared_from_this;
    string name = std::string("agent-load-test");

    string description = std::string("Load testing for agent scalability");

    any scenarioService;

    boolean breakingPointReached = false;

    std::shared_ptr<SystemMetrics> systemMetrics = object{
        object::pair{std::string("cpuUsage"), array<any>()}, 
        object::pair{std::string("memoryUsage"), array<any>()}, 
        object::pair{std::string("timestamp"), array<any>()}
    };

    array<std::shared_ptr<ScaleConfig>> SCALE_CONFIGS = array<std::shared_ptr<ScaleConfig>>{ object{
        object::pair{std::string("agents"), 2}, 
        object::pair{std::string("messageCount"), 10}, 
        object::pair{std::string("description"), std::string("Minimal Load (2 agents)")}
    }, object{
        object::pair{std::string("agents"), 5}, 
        object::pair{std::string("messageCount"), 20}, 
        object::pair{std::string("description"), std::string("Small Group (5 agents)")}
    }, object{
        object::pair{std::string("agents"), 10}, 
        object::pair{std::string("messageCount"), 50}, 
        object::pair{std::string("description"), std::string("Medium Group (10 agents)")}
    }, object{
        object::pair{std::string("agents"), 50}, 
        object::pair{std::string("messageCount"), 100}, 
        object::pair{std::string("description"), std::string("Large Group (50 agents)")}
    }, object{
        object::pair{std::string("agents"), 100}, 
        object::pair{std::string("messageCount"), 150}, 
        object::pair{std::string("description"), std::string("Very Large Group (100 agents)")}
    }, object{
        object::pair{std::string("agents"), 250}, 
        object::pair{std::string("messageCount"), 200}, 
        object::pair{std::string("description"), std::string("Mass Scale (250 agents)")}
    }, object{
        object::pair{std::string("agents"), 500}, 
        object::pair{std::string("messageCount"), 300}, 
        object::pair{std::string("description"), std::string("Extreme Scale (500 agents)")}
    }, object{
        object::pair{std::string("agents"), 1000}, 
        object::pair{std::string("messageCount"), 400}, 
        object::pair{std::string("description"), std::string("Maximum Load (1000 agents)")}
    }, object{
        object::pair{std::string("agents"), 2000}, 
        object::pair{std::string("messageCount"), 500}, 
        object::pair{std::string("description"), std::string("Breaking Point (2000 agents)")}
    } };

    array<object> tests = array<object>{ object{
        object::pair{std::string("name"), std::string("Scale Testing - Sequential Load")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std::string("scenario"));
            if (!this->scenarioService) {
                throw any(std::make_shared<Error>(std::string("Scenario service not found")));
            }
            auto logsDir = ensureLogsDirectory();
            auto timestamp = generateTimestamp();
            auto summaryLogFile = path->join(logsDir, std::string("load-test-summary-") + timestamp + std::string(".log"));
            auto systemMetricsFile = path->join(logsDir, std::string("system-metrics-") + timestamp + std::string(".log"));
            this->startSystemMetricsMonitoring();
            fs->writeFileSync(summaryLogFile, createLoadTestSummaryHeader());
            logger->info(std::string("Starting sequential load tests to find breaking points"));
            auto thresholdLevels = array<ThresholdLevel>{ ThresholdLevel::LOW, ThresholdLevel::MEDIUM, ThresholdLevel::HIGH };
            for (auto& thresholdLevel : thresholdLevels)
            {
                logger->info(std::string("\
Starting test run with ") + thresholdLevel + std::string(" threshold settings"));
                auto breakingPointIdentified = false;
                for (auto& config : this->SCALE_CONFIGS)
                {
                    if (breakingPointIdentified) {
                        fs->appendFileSync(summaryLogFile, string_empty + config->description->padEnd(13) + std::string(" | ") + String(config->agents)->padEnd(6) + std::string(" | ") + string_empty + String(config->messageCount)->padEnd(8) + std::string(" | SKIPPED (") + thresholdLevel + std::string(") | - | - | - | -\
"));
                        continue;
                    }
                    auto detailedLogFile = path->join(logsDir, std::string("load-test-") + thresholdLevel + std::string("-") + config->agents + std::string("-agents-") + timestamp + std::string(".log"));
                    try
                    {
                        fs->writeFileSync(detailedLogFile, createDetailedTestLogHeader(config));
                        fs->appendFileSync(detailedLogFile, std::string("Threshold Level: ") + thresholdLevel + std::string("\
\
"));
                        logger->info(std::string("\
Starting load test for ") + config->description + std::string(" with ") + thresholdLevel + std::string(" threshold"));
                        auto metrics = std::async([=]() { this->runSequentialLoadTest(runtime, config, thresholdLevel); });
                        if (isBreakingPoint(metrics, thresholdLevel)) {
                            breakingPointIdentified = true;
                            logger->warn(std::string("BREAKING POINT IDENTIFIED at ") + config->agents + std::string(" agents with ") + thresholdLevel + std::string(" threshold!"));
                            fs->appendFileSync(detailedLogFile, formatBreakingPointNotification(metrics, thresholdLevel));
                        }
                        fs->appendFileSync(detailedLogFile, formatDetailedMetrics(config, metrics));
                        fs->appendFileSync(summaryLogFile, string_empty + thresholdLevel + std::string(" - ") + formatSummaryLogLine(config, metrics));
                    }
                    catch (const any& error)
                    {
                        breakingPointIdentified = true;
                        logger->error(std::string("BREAKING POINT: Error running test for ") + config->description + std::string(" with ") + thresholdLevel + std::string(" threshold:"), error);
                        fs->appendFileSync(detailedLogFile, std::string("\
*** FATAL ERROR - BREAKING POINT IDENTIFIED (") + thresholdLevel + std::string(" threshold) ***\
") + std::string("Test failed catastrophically at ") + config->agents + std::string(" agents with error:\
") + string_empty + (is<Error>(error)) ? error->message : String(error) + std::string("\
"));
                        fs->appendFileSync(summaryLogFile, string_empty + thresholdLevel + std::string(" - ") + config->description->padEnd(13) + std::string(" | ") + String(config->agents)->padEnd(6) + std::string(" | ") + string_empty + String(config->messageCount)->padEnd(8) + std::string(" | FAILED | - | - | - | FATAL\
"));
                    }
                }
                fs->appendFileSync(summaryLogFile, std::string("\
------------------------------------------------\
\
"));
            }
            this->stopSystemMetricsMonitoring();
            this->writeSystemMetrics(systemMetricsFile);
            auto optimalConfigs = this->determineOptimalScalingPerThreshold(summaryLogFile);
            fs->appendFileSync(summaryLogFile, std::string("\
\
SCALABILITY ANALYSIS\
") + std::string("==============\
"));
            for (auto& [level, config] : Object->entries(optimalConfigs))
            {
                fs->appendFileSync(summaryLogFile, string_empty + level + std::string(" Threshold:\
") + std::string("- Breaking Point: Identified\
") + std::string("- Last stable configuration: ") + config["description"] + std::string("\
") + std::string("- Recommended maximum agents: ") + config["agents"] + std::string("\
") + std::string("- Recommended maximum messages: ") + config["messageCount"] + std::string("\
") + std::string("- Optimal throughput: ~") + config["throughput"]->toFixed(2) + std::string(" messages/second\
\
"));
            }
            fs->appendFileSync(summaryLogFile, std::string("Test Completed: ") + ((std::make_shared<Date>()))->toISOString() + std::string("\
") + std::string("==============\
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
