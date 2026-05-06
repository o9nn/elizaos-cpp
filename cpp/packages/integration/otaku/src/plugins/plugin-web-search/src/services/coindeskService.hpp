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



struct CoinDeskArticle {
    std::string id;
    std::string title;
    std::string url;
    std::optional<std::string> summary;
    std::optional<std::string> body;
    std::optional<std::string> publishedAt;
    std::optional<std::string> updatedAt;
    std::optional<std::vector<std::string>> authors;
    std::optional<std::vector<std::string>> categories;
    std::optional<std::vector<std::string>> tags;
    std::optional<std::string> thumbnail;
    std::optional<std::string> source;
};

struct CoinDeskNewsResponse {
    bool success;
    std::optional<{> data;
    std::vector<CoinDeskArticle> articles;
    std::optional<double> total;
    std::optional<double> page;
    std::optional<double> pageSize;
    std::optional<std::string> error;
};

struct CoinDeskSearchOptions {
    std::optional<std::string;                    // Search query for article content> query;
    std::optional<std::string[];               // Specific keywords to filter by> keywords;
    std::optional<number;                    // Number of results (1-100)> limit;
    std::optional<number;                   // Pagination offset> offset;
    std::optional<number;                     // Page number> page;
    std::optional<std::string[];             // Filter by categories (e.g., 'markets', 'tech', 'policy', 'defi')> categories;
    std::optional<std::string[];                   // Filter by tags> tags;
    std::optional<std::string[];                // Filter by author names> authors;
    std::optional<std::string;                // Start date (YYYY-MM-DD or ISO 8601)> startDate;
    std::optional<std::string;                  // End date (YYYY-MM-DD or ISO 8601)> endDate;
    std::optional<std::string;           // Articles published after this timestamp> publishedAfter;
    std::optional<std::string;          // Articles published before this timestamp> publishedBefore;
    std::optional<'published' | 'updated' | 'relevance';  // Sort field> sortBy;
    std::optional<'asc' | 'desc';        // Sort direction> sortOrder;
    std::optional<boolean;             // Include full article body> includeBody;
    std::optional<boolean;          // Include article summary> includeSummary;
    std::optional<boolean;        // Include thumbnail image URL> includeThumbnail;
};

class CoinDeskService extends Service {
    static serviceType = "COINDESK_NEWS" as const;
    capabilityDescription = "Fetch cryptocurrency news articles from CoinDesk API";
    
    private apiKey: std::string;
    private baseUrl: std::string = "https://data-api.coindesk.com";

    static std::async start(runtime: IAgentRuntime): Promise<CoinDeskService> {
        const service = new CoinDeskService();
        await service.initialize(runtime);
        return service;
    }

    /**
     * Search for news articles using CoinDesk API
     * @param options - Comprehensive search and filter options
     * @returns Promise with article results
     */

            // Search and filtering
            
            // Pagination
            
            // Categorization
            
            // Date filtering - support multiple formats
            
            // Sorting
            
            // Content options

            // Handle different possible response formats

    /**
     * Get latest crypto news headlines (convenience method)
     * @param limit - Number of headlines to fetch (default: 10)
     * @returns Promise with article results
     */

    /**
     * Search news by category
     * @param category - Category name (e.g., 'markets', 'tech', 'policy', 'defi')
     * @param limit - Number of results
     * @returns Promise with article results
     */

    /**
     * Search news within date range
     * @param query - Search query
     * @param startDate - Start date (YYYY-MM-DD)
     * @param endDate - End date (YYYY-MM-DD)
     * @param limit - Number of results
     * @returns Promise with article results
     */

    /**
     * Check if the service is properly configured
     */

    /**
     * Stop the service (cleanup if needed)
     */


} // namespace elizaos
