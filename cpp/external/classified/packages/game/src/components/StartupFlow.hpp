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
#include "types/shared.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct ModelDownloadProgress {
    std::string model_name;
    double current_mb;
    double total_mb;
    double percentage;
    double speed_mbps;
    double eta_seconds;
    std::string status;
};

struct StartupStatus {
    std::string stage;
    double progress;
    std::string description;
    std::optional<std::string> error;
    std::optional<ModelDownloadProgress> model_progress;
};

struct UserConfig {
    std::string ai_provider;
    std::optional<std::string> api_key;
    std::optional<bool> use_local_ollama;
    std::optional<bool> postgres_enabled;
};

struct StartupFlowProps {
};


} // namespace elizaos
