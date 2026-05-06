#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

struct TrustScoreMetrics {
    double trustScore;
    double totalRecommendations;
    double successfulRecs;
    double avgTokenPerformance;
    double riskScore;
    double consistencyScore;
    double virtualConfidence;
    double trustDecay;
    double transparency;
    double accuracy;
    double relevance;
    double explainability;
    double adaptability;
    double security;
    double learning;
    std::string shilling_feedback;
};

} // namespace elizaos
