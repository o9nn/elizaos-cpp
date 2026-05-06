#pragma once
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "service.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct SearchOptions {
    std::optional<double> limit;
    std::optional<double> offset;
    std::optional<std::string> language;
    std::optional<std::string> region;
    std::optional<std::chrono::system_clock::time_point> start;
    std::optional<std::chrono::system_clock::time_point> end;
    std::optional<std::string> fileType;
    std::optional<std::string> site;
    std::optional<std::string> sortBy;
    std::optional<std::string> safeSearch;
};

struct SearchResult {
    std::string title;
    std::string url;
    std::string description;
    std::optional<std::string> displayUrl;
    std::optional<std::string> thumbnail;
    std::optional<std::chrono::system_clock::time_point> publishedDate;
    std::optional<std::string> source;
    std::optional<double> relevanceScore;
    std::optional<std::string> snippet;
};

struct SearchResponse {
    std::string query;
    std::vector<SearchResult> results;
    std::optional<double> totalResults;
    std::optional<double> searchTime;
    std::optional<std::vector<std::string>> suggestions;
    std::optional<std::string> nextPageToken;
    std::optional<std::vector<std::string>> relatedSearches;
};

/**
 * Interface for web search services
 */
    std::future<SearchResponse> search(const std::string& query, std::optional<SearchOptions> options);


} // namespace elizaos
