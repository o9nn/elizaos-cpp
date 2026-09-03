#pragma once

/**
 * ElizaOS C++ - AgentAction Module
 *
 * Action dispatch and registry: add, use, search, and history of
 * agent actions backed by AgentMemoryManager.
 */

#include "elizaos/elizaos.hpp"
#include "elizaos/core.hpp"
#include "elizaos/agentmemory.hpp"
#include "elizaos/agentlogger.hpp"
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// ============================================================================
// ManagedAction
// ============================================================================

using ActionHandler = std::function<JsonValue(const JsonValue& arguments)>;
using ActionBuilder = std::function<std::string(const JsonValue& values)>;

struct ManagedAction {
    std::string name;
    std::string prompt;
    std::string description;
    ActionBuilder builder;
    ActionHandler handler;
    std::vector<std::string> suggestion_after_actions;
    std::vector<std::string> never_after_actions;
    JsonValue function_definition;

    // Backward-compatible optional metadata used by earlier hurdcog tests/tools.
    std::string category;
    std::vector<std::string> examples;
    JsonValue paramSchema;

    ManagedAction() = default;

    ManagedAction(const std::string& n,
                  const std::string& p,
                  const std::string& d,
                  ActionHandler h,
                  const JsonValue& func_def)
        : name(n),
          prompt(p),
          description(d),
          handler(std::move(h)),
          function_definition(func_def) {}
};

// ============================================================================
// AgentAction
// ============================================================================

class AgentAction {
public:
    AgentAction();
    ~AgentAction();

    // Registration
    void addAction(const std::string& name, const ManagedAction& action);
    bool removeAction(const std::string& name);
    void clearActions();

    // Dispatch
    JsonValue useAction(const std::string& function_name, const JsonValue& arguments);

    // Lookup
    std::shared_ptr<ManagedAction>      getAction(const std::string& name);
    const std::unordered_map<std::string, std::shared_ptr<ManagedAction>>& getActions() const;

    // Search
    std::vector<JsonValue> searchActions(const std::string& search_text, int n_results = 5);
    std::vector<JsonValue> getAvailableActions(const std::string& search_text = "", int n_results = 10);

    // History
    void addToActionHistory(const std::string& action_name, const JsonValue& arguments, bool success);
    std::vector<JsonValue> getActionHistory(int n_results = 10);
    JsonValue              getLastAction();

    // Formatting
    std::string composeActionPrompt(const ManagedAction& action, const JsonValue& values);
    JsonValue   getFormattedActions(const std::string& search_text = "");

private:
    JsonValue getActionFromMemory(const std::string& action_name);

    std::unordered_map<std::string, std::shared_ptr<ManagedAction>> actions_;
    std::shared_ptr<AgentMemoryManager> memory_;
    std::shared_ptr<AgentLogger>        logger_;
    mutable std::mutex                  actionsMutex_;
};

} // namespace elizaos
