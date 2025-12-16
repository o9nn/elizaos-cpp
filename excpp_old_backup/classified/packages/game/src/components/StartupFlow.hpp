#include ".types/shared.hpp"
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



struct ModelDownloadProgress {
    std::string model_name;
    double current_mb;
    double total_mb;
    double percentage;
    double speed_mbps;
    double eta_seconds;
    'Downloading' | 'Completed' | 'Failed' | 'AlreadyExists' status;
};

struct StartupStatus {
    std::string stage;
    double progress;
    std::string description;
    std::optional<std::string> error;
    std::optional<ModelDownloadProgress> model_progress;
};

struct UserConfig {
    'OpenAI' | 'Anthropic' | 'Ollama' ai_provider;
    std::optional<std::string> api_key;
    std::optional<bool> use_local_ollama;
    std::optional<bool> postgres_enabled;
};

struct StartupFlowProps {
    () => void onComplete;
};


} // namespace elizaos
