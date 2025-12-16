#include ".types.hpp"
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



struct PluginSearchResult {
    std::optional<std::string> id;
    std::string name;
    std::string description;
    std::optional<double> score;
    std::optional<std::vector<std::string>> tags;
    std::optional<std::vector<std::string>> features;
    std::optional<std::vector<std::string>> requiredConfig;
    std::optional<std::string> version;
    std::optional<std::string> npmPackage;
    std::optional<std::string> repository;
    std::optional<std::string> relevantSection;
};

struct PluginDetails {
    std::optional<std::string> id;
    std::string name;
    std::string description;
    std::optional<std::string> readme;
    std::optional<std::string> capabilitiesSummary;
    std::optional<std::string> repository;
    std::optional<std::string> version;
    std::optional<std::vector<std::string>> tags;
    std::optional<std::vector<std::string>> features;
    std::optional<std::vector<std::string>> requiredConfig;
    std::optional<std::unordered_map<std::string, std::string>> dependencies;
    std::optional<std::vector<std::string>> dependedBy;
    std::optional<std::string> npmPackage;
    std::optional<double> installCount;
    std::optional<double> searchCount;
    std::optional<std::vector<std::string>> categories;
    std::optional<std::string> lastIndexed;
    std::optional<std::string> createdAt;
    std::optional<std::string> updatedAt;
};

struct CloneResult {
    bool success;
    std::optional<std::string> error;
    std::optional<std::string> pluginName;
    std::optional<std::string> localPath;
    std::optional<bool> hasTests;
    std::optional<std::unordered_map<std::string, std::string>> dependencies;
};

std::future<PluginMetadata | null> getPluginDetails(const std::string& name);

std::future<CloneResult> clonePlugin(const std::string& pluginName);

} // namespace elizaos
