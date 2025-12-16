#include ".hooks/use-query-hooks.hpp"
#include ".lib/api-client-config.hpp"
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
    std::string msg;
    std::optional<std::string> agentId;
    std::optional<std::string> agentName;
    std::optional<std::string> roomId;
};

struct LogResponse {
    std::vector<LogEntry> logs;
    double count;
    double total;
    std::string level;
    std::vector<std::string> levels;
};

struct AgentLogViewerProps {
    std::optional<std::string> agentName;
    std::optional<std::string> level;
};

// Log level mappings

// Helper functions
std::string getLevelName(double level);

std::string getLevelColor(double level);

std::string formatTimestamp(double timestamp);

void generateLogChart(const std::vector<LogEntry>& logs);

void LogChart({ data: ReturnType<typeof generateLogChart> } { data });

void LoadingIndicator();

void AgentLogViewer(auto { agentName, AgentLogViewerProps level });

} // namespace elizaos
