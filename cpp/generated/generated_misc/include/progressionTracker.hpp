#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_AGENTSERVER_SRC_SERVICES_PROGRESSIONTRACKER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_AGENTSERVER_SRC_SERVICES_PROGRESSIONTRACKER_H
#include "core.h"
#include "@elizaos/core.h"
#include "./capabilityProgressionService.h"

class ProgressionTracker;

class ProgressionTracker : public object, public std::enable_shared_from_this<ProgressionTracker> {
public:
    using std::enable_shared_from_this<ProgressionTracker>::shared_from_this;
    std::shared_ptr<IAgentRuntime> runtime;

    std::shared_ptr<CapabilityProgressionService> progressionService;

    ProgressionTracker(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<CapabilityProgressionService> progressionService);
    virtual void stop();
    virtual void setupEventListeners();
    virtual std::shared_ptr<Promise<void>> handleShellCommandExecuted(object params);
    virtual std::shared_ptr<Promise<void>> handleAgentNamed(object _params);
    virtual std::shared_ptr<Promise<void>> handleGoalCreated(object _params);
    virtual std::shared_ptr<Promise<void>> handleTodoCreated(object _params);
    virtual std::shared_ptr<Promise<void>> handleBrowserActionPerformed(object params);
    virtual std::shared_ptr<Promise<void>> handleFormSubmitted(object params);
    virtual std::shared_ptr<Promise<void>> handleVisionActionPerformed(object params);
    virtual std::shared_ptr<Promise<void>> handleMicrophoneUsed(object _params);
    virtual std::shared_ptr<Promise<void>> handleCapabilityUsed(object params);
    virtual std::shared_ptr<Promise<void>> trackShellCommand(string command, double exitCode);
    virtual std::shared_ptr<Promise<void>> trackGoalCreation(Record<string, any> goalData);
    virtual std::shared_ptr<Promise<void>> trackTodoCreation(Record<string, any> todoData);
    virtual std::shared_ptr<Promise<void>> trackAgentNaming(string name);
    virtual std::shared_ptr<Promise<void>> trackAction(string actionType, Record<string, any> details = undefined);
    virtual Record<string, any> getProgressionStatus();
    virtual void cleanup();
};

#endif
