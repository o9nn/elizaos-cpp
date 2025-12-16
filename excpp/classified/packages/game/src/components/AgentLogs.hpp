#include ".services/TauriService.hpp"
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



struct AgentLog {
    std::string timestamp;
    'agent' | 'system' | 'error' type;
    'info' | 'warn' | 'error' | 'debug' level;
    std::string message;
    std::optional<std::unordered_map<std::string, unknown>> metadata;
};

    // Initial fetch

    // Set up auto-refresh if enabled

  // Auto-scroll to bottom when new logs arrive


} // namespace elizaos
