#include "ContributorsListModalContent.hpp"
#include "IssuesListModalContent.hpp"
#include "PullRequestsListModalContent.hpp"
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



struct StatCardsDisplayProps {
    IntervalMetrics metrics;
};

struct Contributor {
    std::string username;
    double totalScore;
};

std::future<void> StatCardsDisplay(StatCardsDisplayProps { metrics });

} // namespace elizaos
