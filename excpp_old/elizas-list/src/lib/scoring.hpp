#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "analytics.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class ProjectScorer {
public:
    std::future<double> calculateScore(Project currentProject, Project candidateProject);
    std::future<double> calculateTagScore(Project current, Project candidate);
    double calculateMetricsScore(Project project);
    double calculateRecencyScore(Project project);
    std::future<double> calculateUserBehaviorScore(const std::string& currentId, const std::string& candidateId);
    std::future<double> calculatePopularityTrendScore(Project project);
};
 
} // namespace elizaos
