#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
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
    std::variant<'plugin', 'agent', 'workflow', 'mcp', 'full-stack'> type;
    std::optional<std::variant<'planning', 'generating', 'testing', 'completed', 'failed'>> status;
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
    Date createdAt;
    Date updatedAt;
};

struct ComponentSpec {
    std::string name;
    std::variant<'service', 'action', 'provider', 'ui', 'database', 'api'> type;
    std::string description;
    std::vector<std::string> responsibilities;
    std::vector<std::string> interfaces;
    std::vector<std::string> dependencies;
};

struct DependencySpec {
    std::string name;
    std::string version;
    std::variant<'runtime', 'dev', 'peer'> type;
    std::string purpose;
};

struct IntegrationSpec {
    std::string service;
    std::variant<'api', 'webhook', 'database', 'event'> type;
    std::string purpose;
};

struct RequirementSpec {
    std::string id;
    std::variant<'functional', 'non-functional', 'technical'> type;
    std::variant<'must-have', 'should-have', 'nice-to-have'> priority;
    std::string description;
    std::vector<std::string> acceptanceCriteria;
    std::variant<'pending', 'in-progress', 'completed'> status;
};

struct Milestone {
    std::string id;
    std::string name;
    std::string description;
    std::vector<std::string> deliverables;
    std::optional<Date> dueDate;
    std::variant<'pending', 'in-progress', 'completed'> status;
    std::vector<std::string> dependencies;
};

struct Risk {
    std::string id;
    std::string description;
    std::variant<'low', 'medium', 'high'> impact;
    std::variant<'low', 'medium', 'high'> likelihood;
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
    std::variant<'low', 'medium', 'high'> confidence;
};

class ProjectPlanningService {
public:
    ProjectPlanningService(IAgentRuntime runtime);
    std::future<Service> start(IAgentRuntime runtime);
    std::string capabilityDescription() const;
    std::future<void> start();
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
