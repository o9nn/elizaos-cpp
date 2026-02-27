#include "agent-templates.hpp"
#include <string>
#include <iostream>
#include <stdexcept>

namespace elizaos {

AgentTemplate getTemplateById(const std::string& id) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return agentTemplates.find[&]((template) { return template.id == id); };

}

} // namespace elizaos
