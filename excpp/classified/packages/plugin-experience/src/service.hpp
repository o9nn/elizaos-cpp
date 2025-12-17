#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"
#include "types.hpp"
#include "utils/confidenceDecay.hpp"
#include "utils/experienceAnalyzer.hpp"
#include "utils/experienceRelationships.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class ExperienceService {
public:
    ExperienceService(IAgentRuntime runtime);
    static std::future<ExperienceService> start(IAgentRuntime runtime);
    std::future<void> loadExperiences();
    std::future<Experience> recordExperience(const std::optional<Experience>& experienceData);
    std::future<std::vector<Experience>> queryExperiences(ExperienceQuery query);
    std::future<std::vector<Experience>> findSimilarExperiences(const std::string& text, double limit = 5);
    std::future<ExperienceAnalysis> analyzeExperiences(std::optional<std::string> domain, std::optional<ExperienceType> type);
    double cosineSimilarity(const std::vector<double>& a, const std::vector<double>& b);
    std::vector<std::string> findCommonPatterns(const std::vector<std::string>& texts);
    double calculateOutcomeConsistency(const std::vector<Experience>& experiences);
    std::vector<std::string> extractAlternatives(const std::vector<Experience>& experiences);
    std::vector<std::string> generateRecommendations(const std::vector<Experience>& experiences, double reliability);
    std::future<void> pruneOldExperiences();
    std::future<void> stop();

private:
    ConfidenceDecayManager decayManager_;
    ExperienceRelationshipManager relationshipManager_;
};


} // namespace elizaos
