#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


/**
 * Core types for Plugin Autocoder
 */

/**
 * Project type enumeration
 */
using ProjectType = std::variant<std::string, std::string, std::string, std::string, std::string>;

/**
 * Project status
 */
using ProjectStatus = std::variant<std::string, std::string, std::string, std::string, std::string>;

/**
 * WebSocket-related types
 */
  // Extend with autocoder-specific events if needed

/**
 * Autocoder WebSocket message types
 */
struct AutocoderWebSocketMessage {
    std::string type;
    std::variant<ProjectUpdateData, ProjectCompleteData, ProjectErrorData, StatusUpdateData> data;
    std::optional<double> timestamp;
};

struct ProjectUpdateData {
    std::string projectId;
    ProjectStatus status;
    std::optional<double> progress;
    std::optional<std::string> message;
    std::optional<std::string> currentStep;
};

struct ProjectCompleteData {
    std::string projectId;
    std::string summary;
};

struct ProjectErrorData {
    std::string projectId;
    std::string error;
    std::optional<std::string> details;
};

struct StatusUpdateData {
    double activeProjects;
    double completedProjects;
    double failedProjects;
};

/**
 * Project metadata
 */
struct ProjectMetadata {
    UUID id;
    std::string name;
    std::string description;
    ProjectType type;
    ProjectStatus status;
    std::optional<UUID> formId;
    double createdAt;
    double updatedAt;
    std::optional<double> completedAt;
    std::optional<std::string> error;
    std::optional<std::string> projectName;
    std::optional<std::string> projectDescription;
    std::optional<ProjectType> projectType;
    std::optional<std::string> primaryFunction;
    std::optional<std::string> targetUsers;
    std::optional<std::vector<std::string>> keyFeatures;
    std::optional<std::vector<std::string>> requiredPlugins;
    std::optional<std::vector<std::string>> externalServices;
    std::optional<std::string> complexity;
    std::optional<std::string> estimatedTime;
    std::string path;
    std::string content;
    bool passed;
    std::string summary;
};

/**
 * Project planning service interface
 */
struct IProjectPlanningService {
};

/**
 * Form step completion data
 */
struct StepCompletionData {
    std::string stepId;
    UUID projectId;
};

/**
 * Service type guards and utility types
 */
struct CodeGenerationRequest {
    std::string projectName;
    std::string description;
    ProjectType targetType;
    std::vector<std::string> requirements;
    std::vector<std::string> apis;
    std::optional<std::vector<std::string>> testScenarios;
    std::optional<std::string> githubRepo;
};

struct GenerationResult {
    bool success;
    std::optional<std::string> projectPath;
    std::optional<std::string> githubUrl;
    std::optional<std::vector<std::string>> errors;
    std::optional<std::vector<std::string>> warnings;
    std::optional<ExecutionResults> executionResults;
};

struct ExecutionResults {
    std::optional<bool> testsPass;
    std::optional<bool> lintPass;
    std::optional<bool> typesPass;
    std::optional<bool> buildPass;
    std::optional<bool> buildSuccess;
    std::optional<bool> securityPass;
};

struct ProjectStatusUpdate {
    std::string projectId;
    std::string projectName;
    ProjectType type;
    ProjectStatus status;
    std::optional<std::string> currentStep;
    std::optional<double> progress;
    std::optional<std::string> message;
    std::optional<std::string> error;
    std::optional<double> startedAt;
    std::optional<double> completedAt;
    std::optional<ValidationResults> validationResults;
};

struct ValidationResults {
};

struct ProjectHistory {
    std::string id;
    std::string name;
    ProjectType type;
    ProjectStatus status;
    double startedAt;
    std::optional<double> completedAt;
    std::optional<double> duration;
    std::optional<double> filesGenerated;
    std::optional<std::string> error;
};

/**
 * Service type guards
 */

// Forward declarations for service interfaces

struct ProjectPlan {
    std::string id;
    std::string name;
    std::string description;
    ProjectType type;
    std::optional<ProjectStatus> status;
    std::optional<std::string> formId;
    std::optional<std::string> projectName;
    std::optional<std::string> error;
    std::string path;
    std::string content;
};


} // namespace elizaos
