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


struct FormatOptions {
    bool includeStats;
    bool includeSummary;
    bool includePrData;
    bool includeIssueData;
    std::optional<bool> includeDetailedContributorSummaries;
};

// Helper to sanitize body content: remove HTML comments, make it single-line and escape newlines
std::string sanitizeBodyText(string | undefined | null text, auto maxLength = 500);

std::string formatDataForLLM(IntervalMetrics metrics, string | null summaryContent, FormatOptions options);

} // namespace elizaos
