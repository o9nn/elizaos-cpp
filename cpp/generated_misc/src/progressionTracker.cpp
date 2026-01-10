#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/services/progressionTracker.h"

ProgressionTracker::ProgressionTracker(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<CapabilityProgressionService> progressionService) {
    this->runtime = runtime;
    this->progressionService = progressionService;
    this->setupEventListeners();
}

void ProgressionTracker::stop()
{
}

void ProgressionTracker::setupEventListeners()
{
    logger->info(std::string("[PROGRESSION_TRACKER] Setting up event-based progression tracking"));
    if (this->progressionService->isUnlockedModeEnabled()) {
        logger->info(std::string("[PROGRESSION_TRACKER] Skipping event listeners - unlocked mode enabled"));
        return;
    }
    this->runtime->registerEvent(EventType->SHELL_COMMAND_EXECUTED, [=](auto params) mutable
    {
        std::async([=]() { this->handleShellCommandExecuted(params); });
    }
    );
    this->runtime->registerEvent(EventType->AGENT_NAMED, [=](auto params) mutable
    {
        std::async([=]() { this->handleAgentNamed(params); });
    }
    );
    this->runtime->registerEvent(EventType->GOAL_CREATED, [=](auto params) mutable
    {
        std::async([=]() { this->handleGoalCreated(params); });
    }
    );
    this->runtime->registerEvent(EventType->TODO_CREATED, [=](auto params) mutable
    {
        std::async([=]() { this->handleTodoCreated(params); });
    }
    );
    this->runtime->registerEvent(EventType->BROWSER_ACTION_PERFORMED, [=](auto params) mutable
    {
        std::async([=]() { this->handleBrowserActionPerformed(params); });
    }
    );
    this->runtime->registerEvent(EventType->FORM_SUBMITTED, [=](auto params) mutable
    {
        std::async([=]() { this->handleFormSubmitted(params); });
    }
    );
    this->runtime->registerEvent(EventType->VISION_ACTION_PERFORMED, [=](auto params) mutable
    {
        std::async([=]() { this->handleVisionActionPerformed(params); });
    }
    );
    this->runtime->registerEvent(EventType->MICROPHONE_USED, [=](auto params) mutable
    {
        std::async([=]() { this->handleMicrophoneUsed(params); });
    }
    );
    this->runtime->registerEvent(EventType->CAPABILITY_USED, [=](auto params) mutable
    {
        std::async([=]() { this->handleCapabilityUsed(params); });
    }
    );
    logger->info(std::string("[PROGRESSION_TRACKER] Event listeners registered"));
}

std::shared_ptr<Promise<void>> ProgressionTracker::handleShellCommandExecuted(object params)
{
    if (this->progressionService->isUnlockedModeEnabled()) {
        return std::shared_ptr<Promise<void>>();
    }
    if (params["exitCode"] == 0) {
        logger->info(std::string("[PROGRESSION_TRACKER] Shell command executed successfully, recording capability usage"));
        std::async([=]() { this->progressionService->recordCapabilityUsed(std::string("shell")); });
    }
}

std::shared_ptr<Promise<void>> ProgressionTracker::handleAgentNamed(object _params)
{
    if (this->progressionService->isUnlockedModeEnabled()) {
        return std::shared_ptr<Promise<void>>();
    }
    logger->info(std::string("[PROGRESSION_TRACKER] Agent named: ") + _params["name"] + string_empty);
    this->progressionService->recordAgentNamed(_params["name"]);
}

std::shared_ptr<Promise<void>> ProgressionTracker::handleGoalCreated(object _params)
{
    if (this->progressionService->isUnlockedModeEnabled()) {
        return std::shared_ptr<Promise<void>>();
    }
    logger->info(std::string("[PROGRESSION_TRACKER] Goal created, recording capability usage"));
    std::async([=]() { this->progressionService->recordCapabilityUsed(std::string("goals")); });
}

std::shared_ptr<Promise<void>> ProgressionTracker::handleTodoCreated(object _params)
{
    if (this->progressionService->isUnlockedModeEnabled()) {
        return std::shared_ptr<Promise<void>>();
    }
    logger->info(std::string("[PROGRESSION_TRACKER] Todo created, recording capability usage"));
    std::async([=]() { this->progressionService->recordCapabilityUsed(std::string("todo")); });
}

std::shared_ptr<Promise<void>> ProgressionTracker::handleBrowserActionPerformed(object params)
{
    if (this->progressionService->isUnlockedModeEnabled()) {
        return std::shared_ptr<Promise<void>>();
    }
    logger->info(std::string("[PROGRESSION_TRACKER] Browser action performed: ") + params["action"] + string_empty);
    std::async([=]() { this->progressionService->recordCapabilityUsed(std::string("browser")); });
}

std::shared_ptr<Promise<void>> ProgressionTracker::handleFormSubmitted(object params)
{
    if (this->progressionService->isUnlockedModeEnabled()) {
        return std::shared_ptr<Promise<void>>();
    }
    logger->info(std::string("[PROGRESSION_TRACKER] Form submitted"));
    std::async([=]() { this->progressionService->recordFormSubmitted(params["details"]); });
}

std::shared_ptr<Promise<void>> ProgressionTracker::handleVisionActionPerformed(object params)
{
    if (this->progressionService->isUnlockedModeEnabled()) {
        return std::shared_ptr<Promise<void>>();
    }
    logger->info(std::string("[PROGRESSION_TRACKER] Vision action performed: ") + params["action"] + string_empty);
    std::async([=]() { this->progressionService->recordCapabilityUsed(std::string("vision")); });
}

std::shared_ptr<Promise<void>> ProgressionTracker::handleMicrophoneUsed(object _params)
{
    if (this->progressionService->isUnlockedModeEnabled()) {
        return std::shared_ptr<Promise<void>>();
    }
    logger->info(std::string("[PROGRESSION_TRACKER] Microphone used"));
    std::async([=]() { this->progressionService->recordCapabilityUsed(std::string("microphone")); });
}

std::shared_ptr<Promise<void>> ProgressionTracker::handleCapabilityUsed(object params)
{
    if (this->progressionService->isUnlockedModeEnabled()) {
        return std::shared_ptr<Promise<void>>();
    }
    logger->info(std::string("[PROGRESSION_TRACKER] Capability used: ") + params["capability"] + string_empty);
    std::async([=]() { this->progressionService->recordCapabilityUsed(params["capability"]); });
}

std::shared_ptr<Promise<void>> ProgressionTracker::trackShellCommand(string command, double exitCode)
{
    std::async([=]() { this->runtime->emitEvent(EventType->SHELL_COMMAND_EXECUTED, object{
        object::pair{std::string("command"), std::string("command")}, 
        object::pair{std::string("exitCode"), std::string("exitCode")}
    }); });
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> ProgressionTracker::trackGoalCreation(Record<string, any> goalData)
{
    std::async([=]() { this->runtime->emitEvent(EventType->GOAL_CREATED, object{
        object::pair{std::string("goalData"), std::string("goalData")}
    }); });
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> ProgressionTracker::trackTodoCreation(Record<string, any> todoData)
{
    std::async([=]() { this->runtime->emitEvent(EventType->TODO_CREATED, object{
        object::pair{std::string("todoData"), std::string("todoData")}
    }); });
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> ProgressionTracker::trackAgentNaming(string name)
{
    std::async([=]() { this->runtime->emitEvent(EventType->AGENT_NAMED, object{
        object::pair{std::string("name"), std::string("name")}
    }); });
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> ProgressionTracker::trackAction(string actionType, Record<string, any> details)
{
    logger->info(std::string("[PROGRESSION_TRACKER] Manual action tracked: ") + actionType + string_empty, details);
    static switch_type __switch6914_7798 = {
        { any(std::string("form_submitted")), 1 },
        { any(std::string("browser_used")), 2 },
        { any(std::string("vision_used")), 3 },
        { any(std::string("microphone_used")), 4 }
    };
    switch (__switch6914_7798[actionType])
    {
    case 1:
        std::async([=]() { this->runtime->emitEvent(EventType->FORM_SUBMITTED, object{
            object::pair{std::string("details"), std::string("details")}
        }); });
        break;
    case 2:
        std::async([=]() { this->runtime->emitEvent(EventType->BROWSER_ACTION_PERFORMED, object{
            object::pair{std::string("action"), actionType}, 
            object::pair{std::string("details"), std::string("details")}
        }); });
        break;
    case 3:
        std::async([=]() { this->runtime->emitEvent(EventType->VISION_ACTION_PERFORMED, object{
            object::pair{std::string("action"), actionType}, 
            object::pair{std::string("details"), std::string("details")}
        }); });
        break;
    case 4:
        std::async([=]() { this->runtime->emitEvent(EventType->MICROPHONE_USED, object{
            object::pair{std::string("details"), std::string("details")}
        }); });
        break;
    default:
        std::async([=]() { this->runtime->emitEvent(EventType->CAPABILITY_USED, object{
            object::pair{std::string("capability"), actionType}, 
            object::pair{std::string("details"), std::string("details")}
        }); });
        break;
    }
    return std::shared_ptr<Promise<void>>();
}

Record<string, any> ProgressionTracker::getProgressionStatus()
{
    auto state = this->progressionService->getProgressionState();
    shared unlockedCapabilities = this->progressionService->getUnlockedCapabilities();
    auto availableLevels = this->progressionService->getAvailableLevels();
    auto isUnlockedMode = this->progressionService->isUnlockedModeEnabled();
    return object{
        object::pair{std::string("mode"), (isUnlockedMode) ? std::string("unlocked") : std::string("progression")}, 
        object::pair{std::string("isUnlockedMode"), std::string("isUnlockedMode")}, 
        object::pair{std::string("currentLevel"), state->currentLevel}, 
        object::pair{std::string("unlockedLevels"), state->unlockedLevels}, 
        object::pair{std::string("completedActions"), state->completedActions}, 
        object::pair{std::string("agentNamed"), state->agentNamed}, 
        object::pair{std::string("unlockedCapabilities"), std::string("unlockedCapabilities")}, 
        object::pair{std::string("availableLevels"), availableLevels->map([=](auto level) mutable
        {
            return (object{
                object::pair{std::string("id"), level->id}, 
                object::pair{std::string("name"), level->name}, 
                object::pair{std::string("description"), level->description}, 
                object::pair{std::string("isUnlocked"), level->isUnlocked}, 
                object::pair{std::string("capabilities"), level->unlockedCapabilities}
            });
        }
        )}
    };
}

void ProgressionTracker::cleanup()
{
    if (this->trackingInterval) {
        clearInterval(this->trackingInterval);
        this->trackingInterval = nullptr;
    }
    if (this->namingInterval) {
        clearInterval(this->namingInterval);
        this->namingInterval = nullptr;
    }
    logger->info(std::string("[PROGRESSION_TRACKER] Cleaned up tracking intervals"));
}

