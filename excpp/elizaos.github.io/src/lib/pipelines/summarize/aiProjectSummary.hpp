#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "callAIService.hpp"
#include "codeAreaHelpers.hpp"
#include "config.hpp"
#include "export/queries.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct CompletedItem {
    std::string title;
    double prNumber;
    WorkItemType type;
};

struct FocusArea {
    std::string area;
    double count;
};

struct PullRequestMetrics {
    double total;
    double merged;
    double open;
    std::optional<std::vector<std::any>> items;
    std::optional<double> mergedThisPeriod;
};

struct IssueMetrics {
    double total;
    double opened;
    double closed;
    std::optional<std::vector<std::any>> items;
    std::optional<std::vector<std::any>> closedThisPeriod;
};

struct CodeChangeMetrics {
    double additions;
    double deletions;
    double files;
    std::optional<double> commitCount;
};

struct ContributorInfo {
    std::string username;
};

struct ProjectMetricsForSummary {
    PullRequestMetrics pullRequests;
    IssueMetrics issues;
    double uniqueContributors;
    std::vector<ContributorInfo> topContributors;
    CodeChangeMetrics codeChanges;
    std::vector<FocusArea> focusAreas;
    std::vector<CompletedItem> completedItems;
};

std::future<std::string> generateProjectSummary(RepositoryMetrics metrics, AISummaryConfig config, const std::any& dateInfo, IntervalType intervalType);

/**
 * Calculate appropriate max tokens based on prompt length and interval type
 * Returns a value using a basic scaling approach
 */
double calculateMaxTokens(const std::string& prompt, IntervalType intervalType, AISummaryConfig config);

/**
 * Format project metrics into a structured prompt for analysis based on interval type
 */
std::string formatAnalysisPrompt(RepositoryMetrics metrics, const std::any& dateInfo, IntervalType intervalType, AISummaryConfig config);

} // namespace elizaos
