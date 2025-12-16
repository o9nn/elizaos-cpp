#include ".types.js.hpp"
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



struct ExperienceAnalysis {
    bool isSignificant;
    std::optional<std::string> learning;
    double confidence;
    std::optional<std::vector<std::string>> relatedExperiences;
    std::optional<std::vector<std::string>> actionableInsights;
};

std::future<ExperienceAnalysis> analyzeExperience(const std::optional<Experience>& partialExperience, const std::vector<Experience>& recentExperiences);

std::vector<Experience> findSimilarExperiences(const std::optional<Experience>& partial, const std::vector<Experience>& experiences);

std::vector<Experience> findContradictions(const std::optional<Experience>& partial, const std::vector<Experience>& experiences);

bool similarContext(const std::string& context1, const std::string& context2);

struct FailurePattern {
    std::string learning;
    std::vector<std::string> relatedIds;
    std::vector<std::string> insights;
};

  // Group experiences by action

  // Detect success/failure patterns

  // Detect time-based patterns

  // Detect learning velocity


} // namespace elizaos
