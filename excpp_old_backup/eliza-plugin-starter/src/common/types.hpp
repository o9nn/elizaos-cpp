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



struct SearchResult {
    std::string title;
    std::string url;
    std::string snippet;
    std::optional<double> score;
    "tavily" | "exa" source;
    std::optional<std::unordered_map<std::string, unknown>> metadata;
};

struct SearchOptions {
    std::optional<double> maxResults;
    std::optional<std::string> searchType;
    std::optional<std::unordered_map<std::string, unknown>> filters;
};

struct SearchProvider {
};

struct SearchPluginConfig {
    std::string apiKey;
    std::optional<double> maxResults;
    std::optional<std::string> searchType;
    std::optional<std::unordered_map<std::string, unknown>> filters;
};


} // namespace elizaos
