#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct ExperienceChain {
    std::string rootExperience;
    std::vector<std::string> chain;
    double strength;
    bool validated;
};

struct ExperienceRelationship {
    std::string fromId;
    std::string toId;
    std::string type;
    double strength;
};

class ExperienceRelationshipManager {
public:
    void addRelationship(ExperienceRelationship relationship);
    std::vector<ExperienceRelationship> findRelationships(const std::string& experienceId, std::optional<std::string> type);
    std::vector<ExperienceChain> detectCausalChain(const std::vector<Experience>& experiences);
    bool isRelated(Experience exp1, Experience exp2);
    double contentSimilarity(Experience exp1, Experience exp2);
    std::vector<Experience> findContradictions(Experience experience, const std::vector<Experience>& allExperiences);
    double getExperienceImpact(const std::string& experienceId, const std::vector<Experience>& allExperiences);
};


} // namespace elizaos
