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
#include "services/TauriService.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct AgentLog {
    std::string timestamp;
    std::string type;
    std::string level;
    std::string message;
};

    // Initial fetch

    // Set up auto-refresh if enabled

  // Auto-scroll to bottom when new logs arrive


} // namespace elizaos
