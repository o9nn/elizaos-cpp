#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTOCODER_SRC_TYPES_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTOCODER_SRC_TYPES_INDEX_H
#include "core.h"
#include "@elizaos/core.h"
#include "socket.io.h"

typedef any ProjectType;

typedef any ProjectStatus;

class AutocoderSocket;
class AutocoderWebSocketMessage;
class ProjectUpdateData;
class ProjectCompleteData;
class ProjectErrorData;
class StatusUpdateData;
class ProjectMetadata;
class IProjectPlanningService;
class StepCompletionData;
class CodeGenerationRequest;
class GenerationResult;
class ExecutionResults;
class ProjectStatusUpdate;
class ValidationResults;
class ProjectHistory;
class CodeGenerationService;
class ProjectPlanningServiceInterface;
class ProjectStatusManagerInterface;
class ProjectPlan;

class AutocoderSocket : public Socket, public std::enable_shared_from_this<AutocoderSocket> {
public:
    using std::enable_shared_from_this<AutocoderSocket>::shared_from_this;
};

class AutocoderWebSocketMessage : public object, public std::enable_shared_from_this<AutocoderWebSocketMessage> {
public:
    using std::enable_shared_from_this<AutocoderWebSocketMessage>::shared_from_this;
    any type;

    any data;

    double timestamp;
};

class ProjectUpdateData : public object, public std::enable_shared_from_this<ProjectUpdateData> {
public:
    using std::enable_shared_from_this<ProjectUpdateData>::shared_from_this;
    string projectId;

    ProjectStatus status;

    double progress;

    string message;

    string currentStep;
};

class ProjectCompleteData : public object, public std::enable_shared_from_this<ProjectCompleteData> {
public:
    using std::enable_shared_from_this<ProjectCompleteData>::shared_from_this;
    string projectId;

    array<object> files;

    string summary;
};

class ProjectErrorData : public object, public std::enable_shared_from_this<ProjectErrorData> {
public:
    using std::enable_shared_from_this<ProjectErrorData>::shared_from_this;
    string projectId;

    string error;

    string details;
};

class StatusUpdateData : public object, public std::enable_shared_from_this<StatusUpdateData> {
public:
    using std::enable_shared_from_this<StatusUpdateData>::shared_from_this;
    double activeProjects;

    double completedProjects;

    double failedProjects;
};

class ProjectMetadata : public object, public std::enable_shared_from_this<ProjectMetadata> {
public:
    using std::enable_shared_from_this<ProjectMetadata>::shared_from_this;
    std::shared_ptr<UUID> id;

    string name;

    string description;

    ProjectType type;

    ProjectStatus status;

    std::shared_ptr<UUID> formId;

    double createdAt;

    double updatedAt;

    double completedAt;

    string error;

    object details;

    object artifacts;
};

class IProjectPlanningService : public object, public std::enable_shared_from_this<IProjectPlanningService> {
public:
    using std::enable_shared_from_this<IProjectPlanningService>::shared_from_this;
    virtual std::shared_ptr<Promise<std::shared_ptr<ProjectMetadata>>> createProject(P0 type, Record<string, any> initialData = undefined) = 0;
    virtual std::shared_ptr<Promise<std::shared_ptr<ProjectMetadata>>> updateProject(std::shared_ptr<UUID> projectId, Partial<std::shared_ptr<ProjectMetadata>> updates) = 0;
    virtual std::shared_ptr<Promise<any>> getProject(std::shared_ptr<UUID> projectId) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<ProjectMetadata>>>> listProjects(P0 status = undefined) = 0;
    virtual std::shared_ptr<Promise<void>> generateProject(std::shared_ptr<UUID> projectId) = 0;
};

class StepCompletionData : public object, public std::enable_shared_from_this<StepCompletionData> {
public:
    using std::enable_shared_from_this<StepCompletionData>::shared_from_this;
    string stepId;

    Record<string, any> formData;

    std::shared_ptr<UUID> projectId;
};

class CodeGenerationRequest : public object, public std::enable_shared_from_this<CodeGenerationRequest> {
public:
    using std::enable_shared_from_this<CodeGenerationRequest>::shared_from_this;
    string projectName;

    string description;

    ProjectType targetType;

    array<string> requirements;

    array<string> apis;

    array<string> testScenarios;

    string githubRepo;
};

class GenerationResult : public object, public std::enable_shared_from_this<GenerationResult> {
public:
    using std::enable_shared_from_this<GenerationResult>::shared_from_this;
    boolean success;

    string projectPath;

    string githubUrl;

    array<object> files;

    array<string> errors;

    array<string> warnings;

    std::shared_ptr<ExecutionResults> executionResults;
};

class ExecutionResults : public object, public std::enable_shared_from_this<ExecutionResults> {
public:
    using std::enable_shared_from_this<ExecutionResults>::shared_from_this;
    boolean testsPass;

    boolean lintPass;

    boolean typesPass;

    boolean buildPass;

    boolean buildSuccess;

    boolean securityPass;
};

class ProjectStatusUpdate : public object, public std::enable_shared_from_this<ProjectStatusUpdate> {
public:
    using std::enable_shared_from_this<ProjectStatusUpdate>::shared_from_this;
    string projectId;

    string projectName;

    ProjectType type;

    ProjectStatus status;

    string currentStep;

    double progress;

    string message;

    string error;

    double startedAt;

    double completedAt;

    array<object> files;

    std::shared_ptr<ValidationResults> validationResults;
};

class ValidationResults : public object, public std::enable_shared_from_this<ValidationResults> {
public:
    using std::enable_shared_from_this<ValidationResults>::shared_from_this;
    object lint;

    object typeCheck;

    object tests;

    object build;
};

class ProjectHistory : public object, public std::enable_shared_from_this<ProjectHistory> {
public:
    using std::enable_shared_from_this<ProjectHistory>::shared_from_this;
    string id;

    string name;

    ProjectType type;

    ProjectStatus status;

    double startedAt;

    double completedAt;

    double duration;

    double filesGenerated;

    string error;
};

any isCodeGenerationService(any service);

any isProjectPlanningService(any service);

any isProjectStatusManager(any service);

class CodeGenerationService : public Service, public std::enable_shared_from_this<CodeGenerationService> {
public:
    using std::enable_shared_from_this<CodeGenerationService>::shared_from_this;
    virtual std::shared_ptr<Promise<std::shared_ptr<GenerationResult>>> generateCode(std::shared_ptr<CodeGenerationRequest> request) = 0;
};

class ProjectPlanningServiceInterface : public Service, public std::enable_shared_from_this<ProjectPlanningServiceInterface> {
public:
    using std::enable_shared_from_this<ProjectPlanningServiceInterface>::shared_from_this;
    virtual std::shared_ptr<Promise<std::shared_ptr<ProjectPlan>>> createProjectPlan(object request) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<ProjectPlan>>>> listProjectPlans() = 0;
};

class ProjectStatusManagerInterface : public Service, public std::enable_shared_from_this<ProjectStatusManagerInterface> {
public:
    using std::enable_shared_from_this<ProjectStatusManagerInterface>::shared_from_this;
    virtual string createProject(string name, P1 type) = 0;
    virtual void updateStatus(string projectId, Partial<std::shared_ptr<ProjectStatusUpdate>> updates) = 0;
    virtual void updateStep(string projectId, string step, string message = undefined) = 0;
    virtual void updateValidation(string projectId, P1 type, boolean passed, array<string> errors = undefined) = 0;
};

class ProjectPlan : public object, public std::enable_shared_from_this<ProjectPlan> {
public:
    using std::enable_shared_from_this<ProjectPlan>::shared_from_this;
    string id;

    string name;

    string description;

    ProjectType type;

    ProjectStatus status;

    string formId;

    object details;

    string error;

    object artifacts;
};

#endif
