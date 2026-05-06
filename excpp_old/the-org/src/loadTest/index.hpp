#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"
#include "types.hpp"
#include "utils.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Load Testing Suite for Agent Scaling
 * Tests agent performance and stability at different scales
 */
class AgentLoadTestSuite {
public:
    void if(auto !this.scenarioService);
    void for(auto const thresholdLevel of thresholdLevels);
    std::future<LoadTestMetrics> runSequentialLoadTest(IAgentRuntime runtime, ScaleConfig config, ThresholdLevel = ThresholdLevel::MEDIUM thresholdLevel);
    void startSystemMetricsMonitoring();
    void snapshotSystemMetrics();
    void stopSystemMetricsMonitoring();
    void writeSystemMetrics(const std::string& filePath);
    ThresholdLevel, determineOptimalScalingPerThreshold(const std::string& summaryFile);
    void if(auto rows.length > 0);
    void catch(auto error);
    std::string generateTestMessage(double index, ScaleConfig config);
    void reportMetrics(ScaleConfig config, LoadTestMetrics metrics, ThresholdLevel = ThresholdLevel::MEDIUM thresholdLevel);

private:
    std::any scenarioService_;
};


} // namespace elizaos
