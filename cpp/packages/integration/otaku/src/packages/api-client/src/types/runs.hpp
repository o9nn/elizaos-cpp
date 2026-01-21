#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



using RunStatus = std::string;

struct RunCounts {
    double actions;
    double modelCalls;
    double errors;
    double evaluators;
};

struct RunSummary {
    UUID runId;
    RunStatus status;
    std::optional<double> startedAt;
    std::optional<std::optional<double>> endedAt;
    std::optional<std::optional<double>> durationMs;
    std::optional<UUID> messageId;
    std::optional<UUID> roomId;
    std::optional<UUID> entityId;
    std::optional<RunCounts> counts;
};

using RunEventType = std::variant<, std::string>;

struct RunEvent {
    RunEventType type;
    double timestamp;
};

struct RunDetail {
    RunSummary summary;
    std::vector<RunEvent> events;
};

struct ListRunsParams {
    std::optional<UUID> roomId;
    std::optional<std::variant<RunStatus, std::string>> status;
    std::optional<double> limit;
    std::optional<double> from;
    std::optional<double> to;
};


} // namespace elizaos
