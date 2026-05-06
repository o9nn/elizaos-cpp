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



using SearchResult = {

using SearchImage = {

using SearchResponse = {

struct SearchOptions {
    std::optional<bool> auto_parameters;
    std::optional<"general" | "news" | "finance"> topic;
    std::optional<"basic" | "advanced"> search_depth;
    std::optional<number; // 1-3, only available when search_depth is "advanced"> chunks_per_source;
    std::optional<number; // 0-20> max_results;
    std::optional<"day" | "week" | "month" | "year" | "d" | "w" | "m" | "y"> time_range;
    std::optional<string; // Format: YYYY-MM-DD> start_date;
    std::optional<string; // Format: YYYY-MM-DD> end_date;
    std::optional<boolean | "basic" | "advanced"> include_answer;
    std::optional<boolean | "markdown" | "text"> include_raw_content;
    std::optional<bool> include_images;
    std::optional<bool> include_image_descriptions;
    std::optional<bool> include_favicon;
    std::optional<string[]; // Maximum 300 domains> include_domains;
    std::optional<string[]; // Maximum 150 domains> exclude_domains;
    std::optional<string; // Available only if topic is "general"> country;
};


} // namespace elizaos
