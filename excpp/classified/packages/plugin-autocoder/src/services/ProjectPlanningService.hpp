#pragma once
#include <algorithm>
#include <any>
#include <chrono>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"
#include "elizaos/plugin-forms.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct ProjectPlan {
    std::string id;
    std::string name;
    std::string description;
    std::string type;
    std::optional<std::string> status;
    std::optional<std::string> formId;
    std::optional<std::string> projectName;
    std::optional<std::string> error;
    std::string path;
    std::string content;
    std::vector<ComponentSpec> components;
    std::vector<DependencySpec> dependencies;
    std::vector<IntegrationSpec> integrations;
    std::vector<RequirementSpec> requirements;
    std::vector<Milestone> milestones;
    std::vector<Risk> risks;
    TechStackSpec techStack;
    EffortEstimate estimatedEffort;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point updatedAt;
};

struct ComponentSpec {
    std::string name;
    std::string type;
    std::string description;
    std::vector<std::string> responsibilities;
    std::vector<std::string> interfaces;
    std::vector<std::string> dependencies;
};

struct DependencySpec {
    std::string name;
    std::string version;
    std::string type;
    std::string purpose;
};

struct IntegrationSpec {
    std::string service;
    std::string type;
    std::string purpose;
};

struct RequirementSpec {
    std::string id;
    std::string type;
    std::string priority;
    std::string description;
    std::vector<std::string> acceptanceCriteria;
    std::string status;
};

struct Milestone {
    std::string id;
    std::string name;
    std::string description;
    std::vector<std::string> deliverables;
    std::optional<std::chrono::system_clock::time_point> dueDate;
    std::string status;
    std::vector<std::string> dependencies;
};

struct Risk {
    std::string id;
    std::string description;
    std::string impact;
    std::string likelihood;
    std::string mitigation;
};

struct TechStackSpec {
    std::string language;
    std::optional<std::string> framework;
    std::optional<std::string> database;
    std::vector<std::string> testing;
    std::vector<std::string> tools;
};

struct EffortEstimate {
    double development;
    double testing;
    double documentation;
    double total;
    std::string confidence;
};

class ProjectPlanningService {
public:
    ProjectPlanningService(IAgentRuntime runtime);
    static std::future<Service> start(IAgentRuntime runtime);
    std::string capabilityDescription() const;
    std::future<void> stop();
    std::future<ProjectPlan> createProjectPlan(std::optional<std::any> request);
    std::future<std::any> analyzeRequirements(const std::any& request);
    std::future<std::any> generateArchitecture(const std::any& request, const std::any& analysis);
    std::future<std::vector<RequirementSpec>> generateRequirements(const std::any& request, const std::any& analysis);
    std::future<std::vector<std::string>> generateAcceptanceCriteria(const std::string& requirement);
    std::future<std::vector<Milestone>> planMilestones(const std::any& request, const std::vector<RequirementSpec>& requirements);
    std::future<std::vector<Risk>> identifyRisks(const std::any& request, const std::any& architecture);
    std::future<TechStackSpec> selectTechStack(const std::any& request, const std::any& _architecture);
    std::future<EffortEstimate> estimateEffort(const std::vector<RequirementSpec>& requirements, const std::any& architecture);
    std::variant<Promise<ProjectPlan, undefined>> getProjectPlan(const std::string& planId);
    std::variant<Promise<ProjectPlan, undefined>> updateProjectPlan(const std::string& planId, const std::optional<ProjectPlan>& updates);
    std::future<std::vector<ProjectPlan>> listProjectPlans();
    std::string extractJSON(const std::string& response);

private:
    IAgentRuntime runtime_;
};


} // namespace elizaos
