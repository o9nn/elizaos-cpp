#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



using SearchResult = {

using SearchImage = {

using SearchResponse = {

struct SearchOptions {
    std::optional<bool> auto_parameters;
    std::optional<std::variant<"general", "news", "finance">> topic;
    std::optional<std::variant<"basic", "advanced">> search_depth;
    std::optional<double> chunks_per_source;
    std::optional<double> max_results;
    std::optional<std::variant<"day", "week", "month", "year", "d", "w", "m", "y">> time_range;
    std::optional<std::string> start_date;
    std::optional<std::string> end_date;
    std::optional<std::variant<bool, "basic", "advanced">> include_answer;
    std::optional<std::variant<bool, "markdown", "text">> include_raw_content;
    std::optional<bool> include_images;
    std::optional<bool> include_image_descriptions;
    std::optional<bool> include_favicon;
    std::optional<std::vector<std::string>> include_domains;
    std::optional<std::vector<std::string>> exclude_domains;
    std::optional<std::string> country;
};


} // namespace elizaos
