#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct AutocoderProjectPanelProps {
    std::optional<AutocoderSocket> socket;
};

struct ProjectStatus {
    std::vector<ProjectStatusUpdate> active;
    std::vector<ProjectHistory> history;
    double active;
    double completed;
    double failed;
    double totalGenerated;
    double averageDuration;
};

    // Listen for project updates

          // If completed or failed, move to history


} // namespace elizaos
