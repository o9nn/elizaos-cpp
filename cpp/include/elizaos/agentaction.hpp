#pragma once

/**
 * ElizaOS C++ - AgentAction Module
 *
 * Action dispatch and registry: add, use, search, and history of
 * agent actions backed by AgentMemoryManager.
 */

#include "elizaos.hpp"
#include "core.hpp"
#include "agentmemory.hpp"
#include "agentlogger.hpp"
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

struct ManagedAction {
    std::string name;
    std::string description;
    std::function<JsonValue(const JsonValue& arguments)> handler;

    // Optional metadata
    std::string category;
    std::vector<std::string> examples;
    JsonValue paramSchema;
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
    std::vector<JsonValue> getActionHistory(int n_results = 10);
    JsonValue              getLastAction();

    // Formatting
    std::string composeActionPrompt(const ManagedAction& action, const JsonValue& values);
    JsonValue   getFormattedActions(const std::string& search_text = "");

private:
    void addToActionHistory(const std::string& action_name, const JsonValue& arguments, bool success);
    JsonValue getActionFromMemory(const std::string& action_name);

    std::unordered_map<std::string, std::shared_ptr<ManagedAction>> actions_;
    std::shared_ptr<AgentMemoryManager> memory_;
    std::shared_ptr<AgentLogger>        logger_;
    mutable std::mutex                  actionsMutex_;
};

} // namespace elizaos
