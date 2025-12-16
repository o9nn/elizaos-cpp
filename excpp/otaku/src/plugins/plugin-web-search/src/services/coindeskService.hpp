#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"

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
    std::vector<CoinDeskArticle> articles;
    std::optional<double> total;
    std::optional<double> page;
    std::optional<double> pageSize;
    std::optional<std::string> error;
};

struct CoinDeskSearchOptions {
    std::optional<std::string> query;
    std::optional<std::vector<std::string>> keywords;
    std::optional<double> limit;
    std::optional<double> offset;
    std::optional<double> page;
    std::optional<std::vector<std::string>> categories;
    std::optional<std::vector<std::string>> tags;
    std::optional<std::vector<std::string>> authors;
    std::optional<std::string> startDate;
    std::optional<std::string> endDate;
    std::optional<std::string> publishedAfter;
    std::optional<std::string> publishedBefore;
    std::optional<std::variant<'published', 'updated', 'relevance'>> sortBy;
    std::optional<std::variant<'asc', 'desc'>> sortOrder;
    std::optional<bool> includeBody;
    std::optional<bool> includeSummary;
    std::optional<bool> includeThumbnail;
};

class CoinDeskService {
public:
    std::future<CoinDeskService> start(IAgentRuntime runtime);
    std::future<void> initialize(IAgentRuntime runtime);
    std::future<CoinDeskNewsResponse> searchNews(CoinDeskSearchOptions = {} options);
    std::future<CoinDeskNewsResponse> getLatestHeadlines(number = 10 limit);
    std::future<CoinDeskNewsResponse> searchByCategory(const std::string& category, number = 10 limit);
    std::future<CoinDeskNewsResponse> searchByDateRange(const std::string& query, const std::string& startDate, const std::string& endDate, number = 10 limit);
    bool isConfigured();
    std::future<void> stop();


} // namespace elizaos
