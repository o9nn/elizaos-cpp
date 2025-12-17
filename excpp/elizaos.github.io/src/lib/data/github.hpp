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
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



using GitHubUser = z::infer<typeof GitHubUserSchema>;

using GitHubRepoOwner = z::infer<typeof GitHubRepoOwnerSchema>;

using GitHubRepo = z::infer<typeof GitHubRepoSchema>;

using GitHubFileContent = z::infer<typeof GitHubFileContentSchema>;

using UpdateFileResponse = z::infer<typeof UpdateFileResponseSchema>;

struct BatchFileContentResult {
    std::string content;
    bool repoExists;
    bool fileExists;
};

struct FetchOptions {
    std::optional<std::string> startDate;
    std::optional<std::string> endDate;
};

struct RateLimitInfo {
    double limit;
    double remaining;
    std::chrono::system_clock::time_point resetAt;
    std::optional<double> cost;
};

struct RetryConfig {
    double maxRetries;
    double minTimeout;
    double maxTimeout;
    double factor;
};

// Token bucket for rate limiting
struct TokenBucket {
    double tokens;
    double lastRefill;
    double capacity;
    double refillRate;
};

// Define interfaces for the GraphQL response
struct GitHubPageInfo {
    bool hasNextPage;
    std::string endCursor;
};

class RateLimitExceededError {
public:
    RateLimitExceededError(const std::string& message);
};

class SecondaryRateLimitError {
public:
    SecondaryRateLimitError(const std::string& message);
};

/**
 * Enhanced GitHub API client with robust rate limiting and retry handling
 */
class GitHubClient {
public:
    GitHubClient(const std::string& token, std::optional<Logger> logger);
    std::future<void> wait(double ms);
    RateLimitInfo parseRateLimitHeaders(RawAxiosResponseHeaders headers);
    std::future<void> checkRateLimit();
    void refillTokenBucket(TokenBucket bucket);
    std::future<void> consumeTokens(TokenBucket bucket, double tokens);
    std::future<void> checkSecondaryRateLimits(double cost = 1);
    $ Error();
    void getAuthenticatedUser();
    void getRepo(const std::string& owner, const std::string& repoName);
    void createRepo(const std::string& repoName, const std::string& description, bool autoInit = true);
    void updateFile(const std::string& owner, const std::string& repoName, const std::string& filePath, const std::string& message, const std::string& content);
    void fetchPullRequests(RepositoryConfig repository, FetchOptions options = {});
    void fetchIssues(RepositoryConfig repository, FetchOptions options = {});
    void fetchCommits(RepositoryConfig repository, FetchOptions options = {});
    void fetchFileContent(const std::string& owner, const std::string& repo, const std::string& path);


} // namespace elizaos
