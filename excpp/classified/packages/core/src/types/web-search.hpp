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

;

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


interface NewsSearchOptions extends SearchOptions {
  category?:
    | 'general'
    | 'business'
    | 'entertainment'
    | 'health'
    | 'science'
    | 'sports'
    | 'technology';
  freshness?: 'day' | 'week' | 'month';
}

interface ImageSearchOptions extends SearchOptions {
  size?: 'small' | 'medium' | 'large' | 'wallpaper' | 'any';
  color?:
    | 'color'
    | 'monochrome'
    | 'red'
    | 'orange'
    | 'yellow'
    | 'green'
    | 'blue'
    | 'purple'
    | 'pink'
    | 'brown'
    | 'black'
    | 'gray'
    | 'white';
  type?: 'photo' | 'clipart' | 'line' | 'animated';
  layout?: 'square' | 'wide' | 'tall' | 'any';
  license?: 'any' | 'public' | 'share' | 'sharecommercially' | 'modify';
}

interface VideoSearchOptions extends SearchOptions {
  duration?: 'short' | 'medium' | 'long' | 'any';
  resolution?: 'high' | 'standard' | 'any';
  quality?: 'high' | 'standard' | 'any';
}

/**
 * Interface for web search services
 */
abstract class IWebSearchService extends Service {
  static override readonly serviceType = ServiceType.WEB_SEARCH;

  public readonly capabilityDescription = 'Web search and content discovery capabilities';

  /**
   * Perform a general web search
   * @param query - Search query
   * @param options - Search options
   * @returns Promise resolving to search results
   */
  abstract search(query: string, options?: SearchOptions): Promise<SearchResponse>;

  /**
   * Search for news articles
   * @param query - Search query
   * @param options - News search options
   * @returns Promise resolving to news search results
   */
  abstract searchNews(query: string, options?: NewsSearchOptions): Promise<SearchResponse>;

  /**
   * Search for images
   * @param query - Search query
   * @param options - Image search options
   * @returns Promise resolving to image search results
   */
  abstract searchImages(query: string, options?: ImageSearchOptions): Promise<SearchResponse>;

  /**
   * Search for videos
   * @param query - Search query
   * @param options - Video search options
   * @returns Promise resolving to video search results
   */
  abstract searchVideos(query: string, options?: VideoSearchOptions): Promise<SearchResponse>;

  /**
   * Get search suggestions for a query
   * @param query - Partial search query
   * @returns Promise resolving to array of suggestions
   */
  abstract getSuggestions(query: string): Promise<string[]>;

  /**
   * Get trending searches
   * @param region - Optional region code
   * @returns Promise resolving to trending search queries
   */
  abstract getTrendingSearches(region?: string): Promise<string[]>;

  /**
   * Get detailed information about a specific URL
   * @param url - URL to analyze
   * @returns Promise resolving to page information
   */
  abstract getPageInfo(url: string): Promise<{
    title: string;
    description: string;
    content: string;
    metadata: Record<string, string>;
    images: string[];
    links: string[];
  }>;
}

} // namespace elizaos
