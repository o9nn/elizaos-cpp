#include "types.hpp"
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



using GitHubUser = z.infer<typeof GitHubUserSchema>;

using GitHubRepoOwner = z.infer<typeof GitHubRepoOwnerSchema>;

using GitHubRepo = z.infer<typeof GitHubRepoSchema>;

using GitHubFileContent = z.infer<typeof GitHubFileContentSchema>;

using UpdateFileResponse = z.infer<typeof UpdateFileResponseSchema>;

struct BatchFileContentResult {
    string | null content;
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
    Date resetAt;
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
    string | null endCursor;
};

class RateLimitExceededError extends Error {
  constructor(
    message: string,
    public resetAt: Date,
  ) {
    super(message);
    this.name = "RateLimitExceededError";
  }

class SecondaryRateLimitError extends Error {
  constructor(
    message: string,
    public waitTime: number,
  ) {
    super(message);
    this.name = "SecondaryRateLimitError";
  }

/**
 * Enhanced GitHub API client with robust rate limiting and retry handling
 */
class GitHubClient {
  private logger: Logger;
  private token: string;
  private rateLimitInfo: RateLimitInfo | null = null;
  private readonly retryConfig: RetryConfig = {
    maxRetries: 5,
    minTimeout: 1000,
    maxTimeout: 120000,
    factor: 2,
  };

  // Token bucket for points-based rate limiting (900 points per minute)

  // Token bucket for concurrent requests (max 100, but we'll be conservative with 50)

    // Set up axios defaults

      // Wait for enough tokens to be available

    // Handle points-based rate limiting

    // Handle concurrent requests rate limiting

      using PRSearchResult = z.infer<typeof RawPullRequestSchema>;

      using IssueSearchResult = z.infer<typeof RawIssueSchema>;

      using CommitSearchResult = z.infer<typeof RawCommitSchema>;

    using GraphQLBatchResponse = {

                // For 404s, don't retry - just return null


} // namespace elizaos
