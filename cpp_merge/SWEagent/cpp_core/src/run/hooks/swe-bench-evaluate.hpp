#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "merge-predictions.hpp"
#include "types.hpp"
#include "utils/log.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * SweBench evaluation hook.
 * Will be automatically added to "run_batch" if "SWEBenchInstances.evaluate" is std::set to true
 */

/**
 * SweBench evaluation hook
 */
class SweBenchEvaluate {
public:
    SweBenchEvaluate(std::optional<std:> params);
    std: runId() const;
    std::vector<std::string> getSbCall(const std:& predsPath, bool submitOnly = false);
    void checkRunningCalls();
    void onInstanceCompleted(const std:& _params);
    void moveSbCliReport();
    void onEnd();

private:
    std: outputDir_;
    std: subset_;
    std: split_;
    double continuousSubmissionEvery_;
    double lastEvaluationTime_;
    double evaluationInterval_;
    std: timeSuffix_;
};


} // namespace elizaos
