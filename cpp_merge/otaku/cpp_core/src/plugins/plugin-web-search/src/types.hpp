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



using SearchResult = {

using SearchImage = {

using SearchResponse = {

struct SearchOptions {
    std::optional<bool> auto_parameters;
    std::optional<std::string> topic;
    std::optional<std::string> search_depth;
    std::optional<double> chunks_per_source;
    std::optional<double> max_results;
    std::optional<std::string> time_range;
    std::optional<std::string> start_date;
    std::optional<std::string> end_date;
    std::optional<bool> include_answer;
    std::optional<bool> include_raw_content;
    std::optional<bool> include_images;
    std::optional<bool> include_image_descriptions;
    std::optional<bool> include_favicon;
    std::optional<std::vector<std::string>> include_domains;
    std::optional<std::vector<std::string>> exclude_domains;
    std::optional<std::string> country;
};


} // namespace elizaos
