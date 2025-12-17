#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


/**
 * Templates for quick-start agent creation
 */
struct AgentTemplate {
    std::string id;
    std::string label;
    std::string description;
    std::optional<Agent> template;
};

/**
 * Predefined agent templates based on The Org agents and client types
 */

  // Client-specific agents

/**
 * Get a template by its ID
 */
AgentTemplate getTemplateById(const std::string& id);

} // namespace elizaos
