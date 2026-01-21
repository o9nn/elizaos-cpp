#include "service.hpp"
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



struct SearchOptions {
    std::optional<double> limit;
    std::optional<double> offset;
    std::optional<std::string> language;
    std::optional<std::string> region;
    std::optional<{> dateRange;
    std::optional<Date> start;
    std::optional<Date> end;
    std::optional<std::string> fileType;
    std::optional<std::string> site;
    std::optional<'relevance' | 'date' | 'popularity'> sortBy;
    std::optional<'strict' | 'moderate' | 'off'> safeSearch;
};

struct SearchResult {
    std::string title;
    std::string url;
    std::string description;
    std::optional<std::string> displayUrl;
    std::optional<std::string> thumbnail;
    std::optional<Date> publishedDate;
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

  /**
   * Perform a general web search
   * @param query - Search query
   * @param options - Search options
   * @returns Promise resolving to search results
   */

  /**
   * Search for news articles
   * @param query - Search query
   * @param options - News search options
   * @returns Promise resolving to news search results
   */

  /**
   * Search for images
   * @param query - Search query
   * @param options - Image search options
   * @returns Promise resolving to image search results
   */

  /**
   * Search for videos
   * @param query - Search query
   * @param options - Video search options
   * @returns Promise resolving to video search results
   */

  /**
   * Get search suggestions for a query
   * @param query - Partial search query
   * @returns Promise resolving to array of suggestions
   */

  /**
   * Get trending searches
   * @param region - Optional region code
   * @returns Promise resolving to trending search queries
   */

  /**
   * Get detailed information about a specific URL
   * @param url - URL to analyze
   * @returns Promise resolving to page information
   */

} // namespace elizaos
