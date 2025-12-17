#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class LoadTestService {
public:
    LoadTestService(IAgentRuntime runtime);
    std::future<void> stop();
    void startTest(const std::string& testId);
    void recordMessage(const std::string& testId);
    void recordError(const std::string& testId);
    void getTestMetrics(const std::string& testId);
    void endTest(const std::string& testId);

private:
    double startTime_;
    double messageCount_;
    double errors_;
};


} // namespace elizaos
