#include "elizaos/core.hpp"
#include "elizaos/plugin-forms.hpp"
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



struct ProjectPlan {
    std::string id;
    std::string name;
    std::string description;
    'plugin' | 'agent' | 'workflow' | 'mcp' | 'full-stack' type;
    std::optional<'planning' | 'generating' | 'testing' | 'completed' | 'failed'> status;
    std::optional<std::string> formId;
    std::optional<{> details;
    std::optional<std::string> projectName;
    std::optional<std::string> error;
    std::optional<{> artifacts;
    std::optional<Array<{> files;
    std::string path;
    std::string content;
    { architecture;
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
    'service' | 'action' | 'provider' | 'ui' | 'database' | 'api' type;
    std::string description;
    std::vector<std::string> responsibilities;
    std::vector<std::string> interfaces;
    std::vector<std::string> dependencies;
};

struct DependencySpec {
    std::string name;
    std::string version;
    'runtime' | 'dev' | 'peer' type;
    std::string purpose;
};

struct IntegrationSpec {
    std::string service;
    'api' | 'webhook' | 'database' | 'event' type;
    std::string purpose;
    std::unordered_map<std::string, std::any> configuration;
};

struct RequirementSpec {
    std::string id;
    'functional' | 'non-functional' | 'technical' type;
    'must-have' | 'should-have' | 'nice-to-have' priority;
    std::string description;
    std::vector<std::string> acceptanceCriteria;
    'pending' | 'in-progress' | 'completed' status;
};

struct Milestone {
    std::string id;
    std::string name;
    std::string description;
    std::vector<std::string> deliverables;
    std::optional<Date> dueDate;
    'pending' | 'in-progress' | 'completed' status;
    std::vector<std::string> dependencies;
};

struct Risk {
    std::string id;
    std::string description;
    'low' | 'medium' | 'high' impact;
    'low' | 'medium' | 'high' likelihood;
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
    number; // hours development;
    double testing;
    double documentation;
    double total;
    'low' | 'medium' | 'high' confidence;
};

class ProjectPlanningService extends Service {
  static serviceName: string = 'project-planning';
  static serviceType: ServiceTypeName = ServiceType.UNKNOWN;

  protected runtime: IAgentRuntime;
  private formsService?: FormsService;
  private plans: Map<string, ProjectPlan> = new Map();

  /**
   * Static method to start the service
   */
  static async start(runtime: IAgentRuntime): Promise<Service> {
    elizaLogger.info('Starting ProjectPlanningService...');
    const service = new ProjectPlanningService(runtime);
    await service.start();
    return service;
  }

    // Get forms service for interactive planning

  /**
   * Create a comprehensive project plan
   */

    // Analyze project requirements

    // Generate architecture

    // Create detailed requirements

    // Plan milestones

    // Identify risks

    // Determine tech stack

    // Estimate effort

  /**
   * Analyze project requirements using AI
   */

  /**
   * Generate system architecture
   */

  /**
   * Generate detailed requirements
   */

    // Convert high-level requirements to detailed specs

    // Add technical requirements from analysis

  /**
   * Generate acceptance criteria for a requirement
   */

  /**
   * Plan project milestones
   */

  /**
   * Identify project risks
   */

  /**
   * Select appropriate tech stack
   */

  /**
   * Estimate project effort
   */

    // Base estimates

  /**
   * Get project plan by ID
   */

  /**
   * Update project plan
   */

  /**
   * List all project plans
   */

  /**
   * Extract JSON from LLM response
   */

    // Remove markdown code blocks

    // Try to find JSON object or array


} // namespace elizaos
