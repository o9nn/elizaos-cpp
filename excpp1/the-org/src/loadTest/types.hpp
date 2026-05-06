#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

struct LoadTestMetrics {
    double totalTime;
    double errorCount;
    double messagesSent;
    double messagesProcessed;
    double avgResponseTime;
    double minResponseTime;
    double maxResponseTime;
    std::vector<double> responseTimes;
    double peakMemoryUsage;
    double memoryUsageStart;
    double memoryUsageEnd;
    double successRate;
    double throughput;
    double timeoutCount;
};

struct ScaleConfig {
    double agents;
    double messageCount;
    std::string description;
};

struct SystemMetrics {
    std::vector<double> cpuUsage;
    std::vector<double> memoryUsage;
    std::vector<double> timestamp;
};


} // namespace elizaos
