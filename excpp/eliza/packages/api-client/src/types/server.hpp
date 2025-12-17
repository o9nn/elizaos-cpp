#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct ServerHealth {
    std::string status;
    double uptime;
    Date timestamp;
    std::optional<std::string> version;
    std::optional<Record<> checks;
    std::string status;
    std::optional<std::string> message;
};

struct ServerStatus {
    double total;
    double active;
    double inactive;
    double used;
    double total;
    double percentage;
    double uptime;
    std::string version;
};

struct ServerDebugInfo {
    UUID id;
    std::string name;
    std::string status;
    double connections;
    std::any memory;
};

struct LogSubmitParams {
    std::string level;
    std::string message;
    std::optional<std::string> source;
};


} // namespace elizaos
