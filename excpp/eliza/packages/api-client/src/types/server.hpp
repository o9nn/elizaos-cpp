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

struct ServerHealth {
    'healthy' | 'degraded' | 'unhealthy' status;
    double uptime;
    Date timestamp;
    std::optional<std::string> version;
    std::optional<Record<> checks;
    'pass' | 'fail' status;
    std::optional<std::string> message;
};


struct ServerStatus {
    { agents;
    double total;
    double active;
    double inactive;
    { memory;
    double used;
    double total;
    double percentage;
    double uptime;
    std::string version;
};


struct ServerDebugInfo {
    { runtime;
    Array<{ agents;
    UUID id;
    std::string name;
    std::string status;
    double connections;
    std::any memory;
    std::unordered_map<std::string, std::string> environment;
};


struct LogSubmitParams {
    'debug' | 'info' | 'warn' | 'error' level;
    std::string message;
    std::optional<std::string> source;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};


} // namespace elizaos
