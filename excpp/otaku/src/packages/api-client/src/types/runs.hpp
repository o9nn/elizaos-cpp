#include "elizaos/core.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;

using RunStatus = std::variant<'completed', 'timeout', 'error', 'started'>;

struct RunCounts {
    double actions;
    double modelCalls;
    double errors;
    double evaluators;
};


struct RunSummary {
    UUID runId;
    RunStatus status;
    number | null startedAt;
    std::optional<number | null> endedAt;
    std::optional<number | null> durationMs;
    std::optional<UUID> messageId;
    std::optional<UUID> roomId;
    std::optional<UUID> entityId;
    std::optional<RunCounts> counts;
    std::optional<std::unordered_map<std::string, unknown>> metadata;
};


using RunEventType = std::variant<, 'RUN_STARTED', 'RUN_ENDED', 'ACTION_STARTED', 'ACTION_COMPLETED', 'MODEL_USED', 'EVALUATOR_COMPLETED', 'EMBEDDING_EVENT'>;

struct RunEvent {
    RunEventType type;
    double timestamp;
    std::unordered_map<std::string, unknown> data;
};


struct RunDetail {
    RunSummary summary;
    std::vector<RunEvent> events;
};


struct ListRunsParams {
    std::optional<UUID> roomId;
    std::optional<RunStatus | 'all'> status;
    std::optional<double> limit;
    std::optional<number; // epoch ms> from;
    std::optional<number; // epoch ms> to;
};


} // namespace elizaos
