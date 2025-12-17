#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types.hpp"
#include "confidenceDecay.hpp"
#include "experienceRelationships.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct Experiment {
    std::string id;
    std::string hypothesis;
    std::string action;
    std::string expectedOutcome;
    std::string domain;
    double priority;
    std::vector<std::string> relatedExperiences;
    double suggestedAt;
};

struct LearningGap {
    std::string domain;
    std::string description;
    double uncertainty;
    std::vector<Experiment> suggestedExperiments;
};

class ActiveLearningManager {
public:
    ActiveLearningManager();
    std::vector<LearningGap> identifyLearningGaps(const std::vector<Experience>& experiences);
    std::optional<Experiment> suggestNextExperiment(const std::vector<Experience>& experiences);
    std::unordered_map<std::string, std::vector<Experience>> groupByDomain(const std::vector<Experience>& experiences);
    double averageConfidence(const std::vector<Experience>& experiences);
    std::vector<[Experience, Experience]> findContradictions(const std::vector<Experience>& experiences);
    std::vector<Experience> findUnvalidatedHypotheses(const std::vector<Experience>& experiences);
    std::vector<Experiment> generateExperiments(const std::vector<Experience>& lowConfidenceExperiences, const std::string& domain);
    std::vector<Experiment> generateContradictionExperiments(Array<[Experience contradictions, auto Experience]>, const std::string& domain);
    std::vector<Experiment> generateValidationExperiments(const std::vector<Experience>& unvalidatedHypotheses, const std::string& domain);
    std::vector<Experiment> generateLearningCurriculum(const std::vector<Experience>& experiences, std::optional<std::string> targetDomain);

private:
    ExperienceRelationshipManager relationshipManager_;
    ConfidenceDecayManager decayManager_;
};


} // namespace elizaos
