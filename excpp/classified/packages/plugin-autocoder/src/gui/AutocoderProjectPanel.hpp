#include ".types.hpp"
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



struct AutocoderProjectPanelProps {
    std::optional<AutocoderSocket; // WebSocket connection if available> socket;
    std::optional<(projectId: string) => void> onProjectSelect;
};

struct ProjectStatus {
    std::vector<ProjectStatusUpdate> active;
    std::vector<ProjectHistory> history;
    { summary;
    double active;
    double completed;
    double failed;
    double totalGenerated;
    double averageDuration;
};

    // Listen for project updates

          // If completed or failed, move to history


} // namespace elizaos
