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
#include "dbCompatibility.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Component service that provides database-compatible component operations
 */
class ComponentService {
public:
    ComponentService();
    std::variant<Promise<Component, null>> getComponent(UUID entityId, const std::string& componentType);
    std::future<bool> createComponent(const std::any& params);
    std::future<bool> updateComponent(const std::any& params);
    Component normalizeComponent(Component component);
    void normalizeBooleans(const std::any& obj);
};

/**
 * Create a component service instance for the runtime
 */
ComponentService createComponentService(IAgentRuntime runtime); 
} // namespace elizaos
