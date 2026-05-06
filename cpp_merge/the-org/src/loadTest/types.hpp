#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_THE_ORG_SRC_LOADTEST_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_THE_ORG_SRC_LOADTEST_TYPES_H
#include "core.hpp"

class LoadTestMetrics;
class ScaleConfig;
class SystemMetrics;

class LoadTestMetrics : public object, public std::enable_shared_from_this<LoadTestMetrics> {
public:
    using std::enable_shared_from_this<LoadTestMetrics>::shared_from_this;
    double totalTime;

    double errorCount;

    double messagesSent;

    double messagesProcessed;

    double avgResponseTime;

    double minResponseTime;

    double maxResponseTime;

    array<double> responseTimes;

    double peakMemoryUsage;

    double memoryUsageStart;

    double memoryUsageEnd;

    double successRate;

    double throughput;

    double timeoutCount;

    object errorTypes;
};

class ScaleConfig : public object, public std::enable_shared_from_this<ScaleConfig> {
public:
    using std::enable_shared_from_this<ScaleConfig>::shared_from_this;
    double agents;

    double messageCount;

    string description;
};

class SystemMetrics : public object, public std::enable_shared_from_this<SystemMetrics> {
public:
    using std::enable_shared_from_this<SystemMetrics>::shared_from_this;
    array<double> cpuUsage;

    array<double> memoryUsage;

    array<double> timestamp;
};

#endif
