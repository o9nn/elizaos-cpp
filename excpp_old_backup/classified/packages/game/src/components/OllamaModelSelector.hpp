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


struct ModelInfo {
    std::string name;
    std::string description;
    double min_memory_gb;
    bool recommended;
    bool installed;
};

struct OllamaRecommendations {
    bool success;
    { system_info;
    double total_memory_gb;
    double total_memory_mb;
    double recommended_memory_mb;
    bool has_sufficient_memory;
    std::vector<ModelInfo> recommended_models;
    std::vector<ModelInfo> all_models;
    std::string default_model;
    std::vector<std::string> installed_models;
};

struct OllamaModelSelectorProps {
    std::string value;
    (model: string) => void onChange;
};

        // If no value set, use the default model


} // namespace elizaos
