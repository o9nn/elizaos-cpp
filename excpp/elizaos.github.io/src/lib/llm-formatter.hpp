#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

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
std::string sanitizeBodyText(const std::string& text, auto maxLength);

std::string formatDataForLLM(IntervalMetrics metrics, const std::string& summaryContent, FormatOptions options);

} // namespace elizaos
