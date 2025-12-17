#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "..types.hpp"
#include "..utils/log.hpp"
#include ".merge-predictions.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * SweBench evaluation hook.
 * Will be automatically added to `run_batch` if `SWEBenchInstances.evaluate` is set to true
 */

/**
 * SweBench evaluation hook
 */
class SweBenchEvaluate {
public:
    SweBenchEvaluate(std::optional<std::any> params);
    std::string runId() const;
    std::vector<std::string> getSbCall(const std::string& predsPath, bool submitOnly = false);
    void checkRunningCalls();
    void onInstanceCompleted(const std::any& _params);
    void moveSbCliReport();
    void onEnd();

private:
    std::string outputDir_;
    std::string subset_;
    std::string split_;
    double continuousSubmissionEvery_;
    double lastEvaluationTime_;
    double evaluationInterval_;
    std::string timeSuffix_;
};


} // namespace elizaos
