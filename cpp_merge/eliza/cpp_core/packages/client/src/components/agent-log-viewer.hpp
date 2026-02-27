#include ".hooks/use-query-hooks.hpp"
#include "lib/api-client-config.hpp"
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



// Types
struct LogEntry {
    double level;
    double time;
    std: msg;
    std::optional<std:> agentId;
    std::optional<std:> agentName;
    std::optional<std:> roomId;
};

struct LogResponse {
    std::vector<LogEntry> logs;
    double count;
    double total;
    std: level;
    std::vector<std::string> levels;
};

struct AgentLogViewerProps {
    std::optional<std:> agentName;
    std::optional<std:> level;
};

// Log level mappings

// Helper functions
std: getLevelName(double level);

std: getLevelColor(double level);

std: formatTimestamp(double timestamp);

void generateLogChart(const std::vector<LogEntry>& logs);

void LogChart({ data: ReturnType<typeof generateLogChart> } { data });

void LoadingIndicator();

void AgentLogViewer(auto { agentName, AgentLogViewerProps level });

} // namespace elizaos
