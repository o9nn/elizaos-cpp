#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct ProgressionLevel {
    std::string id;
    std::string name;
    std::string description;
    std::vector<std::string> unlockedCapabilities;
    std::vector<UnlockCondition> unlockConditions;
    bool isUnlocked;
};

struct UnlockCondition {
    std::variant<'action_performed', 'form_submitted', 'capability_used', 'agent_named'> type;
    std::optional<std::string> capability;
    std::optional<std::string> action;
    std::string description;
};

struct ProgressionState {
    double currentLevel;
    std::vector<std::string> unlockedLevels;
    std::vector<std::string> completedActions;
    bool agentNamed;
    std::optional<Date> lastUnlockTime;
};

class CapabilityProgressionService {
public:
    CapabilityProgressionService(std::optional<IAgentRuntime> runtime);
    std::future<CapabilityProgressionService> start(IAgentRuntime runtime);
    std::vector<ProgressionLevel> initializeProgressionLevels();
    std::future<void> unlockAllLevels();
    std::future<void> checkProgressionConditions();
    std::future<bool> areConditionsMet(const std::vector<UnlockCondition>& conditions);
    std::future<bool> isConditionMet(UnlockCondition condition);
    std::future<void> unlockLevel(ProgressionLevel level);
    std::future<void> registerLevelCapabilities(ProgressionLevel level);
    std::future<void> injectUnlockMessage(ProgressionLevel level);
    std::future<void> recordAgentNamed(const std::string& name);
    std::future<void> recordCapabilityUsed(const std::string& capability);
    std::future<void> recordFormSubmitted(std::optional<Record<string> formData, auto unknown>);
    std::future<void> recordActionPerformed(const std::string& actionName);
    double getCurrentLevel();
    std::vector<std::string> getUnlockedCapabilities();
    ProgressionState getProgressionState();
    std::vector<ProgressionLevel> getAvailableLevels();
    bool isCapabilityUnlocked(const std::string& capability);
    bool isUnlockedModeEnabled();
    std::future<void> setProgressionMode(const std::variant<'progression', 'unlocked'>& mode);
    void resetProgression();
    std::future<void> stop();

private:
    ProgressionState progressionState_;
    std::vector<ProgressionLevel> levels_;
};


} // namespace elizaos
