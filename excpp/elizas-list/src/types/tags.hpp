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



struct TagMetadata {
    std::optional<IconType> icon;
    std::optional<std::string> color;
    std::optional<bool> trending;
    std::optional<std::string> description;
    std::optional<double> popularity;
    std::string category;
    std::optional<std::vector<std::string>> relatedTags;
};

struct TagAnalytics {
    double usageCount;
    double trendingScore;
    double projectCount;
    double recentGrowth;
    double averageStars;
};

struct TagCategory {
    std::string id;
    std::string name;
    std::string description;
    std::optional<IconType> icon;
    std::optional<std::string> color;
    std::vector<std::string> tags;
};
 
} // namespace elizaos
