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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct SearchResult {
    std::string title;
    std::string url;
    std::string snippet;
    std::optional<double> score;
    std::string source;
};

struct SearchOptions {
    std::optional<double> maxResults;
    std::optional<std::string> searchType;
};

struct SearchProvider {
};

struct SearchPluginConfig {
    std::string apiKey;
    std::optional<double> maxResults;
    std::optional<std::string> searchType;
};


} // namespace elizaos
