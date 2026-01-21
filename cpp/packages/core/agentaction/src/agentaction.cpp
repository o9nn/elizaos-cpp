#include "elizaos/agentaction.hpp"
#include <algorithm>
#include <sstream>
#include <random>

namespace elizaos {

// Simple UUID generator
std::string generateSimpleUUID() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    
    std::string uuid = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
    for (auto& c : uuid) {
        if (c == 'x') {
            c = "0123456789abcdef"[dis(gen)];
        } else if (c == 'y') {
            c = "89ab"[dis(gen) % 4];
        }
    }
    return uuid;
}

AgentAction::AgentAction() {
    memory_ = std::make_shared<AgentMemoryManager>();
    logger_ = std::make_shared<AgentLogger>();
}

AgentAction::~AgentAction() = default;

void AgentAction::addAction(const std::string& name, const ManagedAction& action) {
    actions_[name] = std::make_shared<ManagedAction>(action);
    
    // Create memory entry for action using CustomMetadata
    UUID memoryId = generateSimpleUUID();
    UUID entityId = generateSimpleUUID();
    UUID agentId = generateSimpleUUID();
    
    std::string content = action.name + " - " + action.description;
    
    // Create CustomMetadata for the action
    CustomMetadata customMeta;
    customMeta.customData["name"] = name;
    customMeta.customData["description"] = action.description;
    
    MemoryMetadata metadata = customMeta;
    
    auto memory = std::make_shared<Memory>(memoryId, content, entityId, agentId, metadata);
    
    memory_->createMemory(memory, "actions");
}

JsonValue AgentAction::useAction(const std::string& function_name, const JsonValue& arguments) {
    JsonValue result;
    
    auto it = actions_.find(function_name);
    if (it == actions_.end()) {
        addToActionHistory(function_name, arguments, false);
        logger_->log("Warning: action was hallucinated: " + function_name, "warning");
        
        result["success"] = false;
        result["error"] = std::string("Action not found");
        return result;
    }
    
    addToActionHistory(function_name, arguments, true);
    
    try {
        auto action = it->second;
        if (action->handler) {
            result = action->handler(arguments);
            result["success"] = true;
        } else {
            result["success"] = false;
            result["error"] = std::string("No handler for action");
        }
    } catch (const std::exception& e) {
        result["success"] = false;
        result["error"] = std::string("Exception: ") + e.what();
        addToActionHistory(function_name, arguments, false);
    }
    
    return result;
}

std::vector<JsonValue> AgentAction::searchActions(const std::string& search_text, int n_results) {
    // Simple search implementation
    std::vector<JsonValue> results;
    
    MemorySearchParams params;
    params.tableName = "actions";
    params.count = n_results;
    
    auto memories = memory_->getMemories(params);
    
    // Filter by search text (simple substring match)
    for (const auto& memory : memories) {
        if (memory->getContent().find(search_text) != std::string::npos) {
            JsonValue action_data;
            action_data["document"] = memory->getContent();
            
            // Extract metadata if it's CustomMetadata
            JsonValue metadata_map;
            if (std::holds_alternative<CustomMetadata>(memory->getMetadata())) {
                const auto& customMeta = std::get<CustomMetadata>(memory->getMetadata());
                for (const auto& pair : customMeta.customData) {
                    metadata_map[pair.first] = pair.second;
                }
            }
            action_data["metadata"] = metadata_map;
            
            results.push_back(action_data);
        }
    }
    
    return results;
}

std::vector<JsonValue> AgentAction::getAvailableActions(const std::string& search_text, int n_results) {
    return searchActions(search_text, n_results);
}

std::shared_ptr<ManagedAction> AgentAction::getAction(const std::string& name) {
    auto it = actions_.find(name);
    if (it != actions_.end()) {
        return it->second;
    }
    return nullptr;
}

bool AgentAction::removeAction(const std::string& name) {
    auto it = actions_.find(name);
    if (it != actions_.end()) {
        actions_.erase(it);
        return true;
    }
    return false;
}

const std::unordered_map<std::string, std::shared_ptr<ManagedAction>>& AgentAction::getActions() const {
    return actions_;
}

void AgentAction::addToActionHistory(const std::string& action_name, const JsonValue& arguments, bool success) {
    UUID memoryId = generateSimpleUUID();
    UUID entityId = generateSimpleUUID();
    UUID agentId = generateSimpleUUID();
    
    CustomMetadata customMeta;
    customMeta.customData["success"] = success ? "true" : "false";
    
    // Convert JsonValue arguments to string metadata (simplified)
    for (const auto& arg : arguments) {
        customMeta.customData[arg.first] = "value"; // Simplified serialization
    }
    
    MemoryMetadata metadata = customMeta;
    
    auto memory = std::make_shared<Memory>(memoryId, action_name, entityId, agentId, metadata);
    
    memory_->createMemory(memory, "action_history");
}

std::vector<JsonValue> AgentAction::getActionHistory(int n_results) {
    std::vector<JsonValue> results;
    
    MemorySearchParams params;
    params.tableName = "action_history";
    params.count = n_results;
    
    auto memories = memory_->getMemories(params);
    
    for (const auto& memory : memories) {
        JsonValue action_data;
        action_data["document"] = memory->getContent();
        
        // Extract metadata if it's CustomMetadata
        JsonValue metadata_map;
        if (std::holds_alternative<CustomMetadata>(memory->getMetadata())) {
            const auto& customMeta = std::get<CustomMetadata>(memory->getMetadata());
            for (const auto& pair : customMeta.customData) {
                metadata_map[pair.first] = pair.second;
            }
        }
        action_data["metadata"] = metadata_map;
        
        results.push_back(action_data);
    }
    
    return results;
}

JsonValue AgentAction::getLastAction() {
    auto history = getActionHistory(1);
    if (history.empty()) {
        return JsonValue{};
    }
    return history[0];
}

void AgentAction::clearActions() {
    actions_.clear();
    memory_->clear();
}

std::string AgentAction::composeActionPrompt(const ManagedAction& action, const JsonValue& values) {
    std::string prompt = action.prompt;
    
    if (action.builder) {
        prompt = action.builder(values);
    }
    
    return prompt;
}

JsonValue AgentAction::getFormattedActions(const std::string& search_text) {
    JsonValue result;
    
    auto available_actions = getAvailableActions(search_text, 5);
    
    // Create formatted actions string
    std::stringstream formatted_actions;
    formatted_actions << "Available actions for me to choose from:\n";
    
    for (const auto& action : available_actions) {
        std::string document;
        if (action.count("document")) {
            try {
                document = std::any_cast<std::string>(action.at("document"));
            } catch (const std::bad_any_cast&) {}
        }
        
        formatted_actions << document << "\n";
    }
    
    // Create short actions string
    std::stringstream short_actions;
    short_actions << "Available actions (name): ";
    
    for (size_t i = 0; i < available_actions.size(); ++i) {
        if (i > 0) short_actions << ", ";
        
        std::string name;
        if (available_actions[i].count("metadata")) {
            try {
                auto metadata = std::any_cast<JsonValue>(available_actions[i].at("metadata"));
                if (metadata.count("name")) {
                    name = std::any_cast<std::string>(metadata.at("name"));
                }
            } catch (const std::bad_any_cast&) {}
        }
        
        short_actions << name;
    }
    
    result["available_actions"] = available_actions;
    result["formatted_actions"] = formatted_actions.str();
    result["short_actions"] = short_actions.str();
    
    return result;
}

JsonValue AgentAction::getActionFromMemory(const std::string& action_name) {
    // Search for action in memory by name
    MemorySearchParams params;
    params.tableName = "actions";
    params.count = 100; // Get all actions
    
    auto memories = memory_->getMemories(params);
    
    for (const auto& memory : memories) {
        if (std::holds_alternative<CustomMetadata>(memory->getMetadata())) {
            const auto& customMeta = std::get<CustomMetadata>(memory->getMetadata());
            auto it = customMeta.customData.find("name");
            if (it != customMeta.customData.end() && it->second == action_name) {
                JsonValue action_data;
                action_data["document"] = memory->getContent();
                
                // Convert metadata to JsonValue
                JsonValue metadata_map;
                for (const auto& pair : customMeta.customData) {
                    metadata_map[pair.first] = pair.second;
                }
                action_data["metadata"] = metadata_map;
                
                return action_data;
            }
        }
    }
    
    return JsonValue{};
}

} // namespace elizaos